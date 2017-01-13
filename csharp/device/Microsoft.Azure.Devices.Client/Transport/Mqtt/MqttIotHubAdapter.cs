// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;
    using System.Diagnostics.Contracts;
    using System.IO;
    using System.Threading.Tasks;
    using DotNetty.Buffers;
    using DotNetty.Codecs.Mqtt.Packets;
    using DotNetty.Common.Concurrency;
    using DotNetty.Handlers.Tls;
    using DotNetty.Transport.Channels;
    using Microsoft.Azure.Devices.Client;
    using Microsoft.Azure.Devices.Client.Common;
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;
    
    sealed class MqttIotHubAdapter : ChannelHandlerAdapter
    {
        [Flags]
        enum StateFlags
        {
            NotConnected = 0,
            Connecting = 1,
            Connected = 2,
            Subscribing = 4,
            Receiving = 8,
            Closed = 16
        }
        const string CommandTopicFilterFormat = "devices/{0}/messages/devicebound/#";
        const string TelemetryTopicFormat = "devices/{0}/messages/events/";
            

        static readonly Action<object> PingServerCallback = PingServer;
        static readonly Action<object> CheckConnAckTimeoutCallback = ShutdownIfNotReady;
        static readonly Func<IChannelHandlerContext, Exception, bool> ShutdownOnWriteErrorHandler = (ctx, ex) => { ShutdownOnError(ctx, ex); return false; };

        readonly Action onConnected;
        readonly Action<Message> onMessageReceived;
        readonly Action<Exception> onError;

        readonly string deviceId;
        readonly SimpleWorkQueue<PublishPacket> deviceBoundOneWayProcessor;
        readonly OrderedTwoPhaseWorkQueue<int, PublishPacket> deviceBoundTwoWayProcessor;
        readonly string iotHubHostName;
        readonly MqttTransportSettings mqttTransportSettings;
        readonly TimeSpan pingRequestInterval;
        readonly string password;
        readonly SimpleWorkQueue<PublishWorkItem> serviceBoundOneWayProcessor;
        readonly OrderedTwoPhaseWorkQueue<int, PublishWorkItem> serviceBoundTwoWayProcessor;
        readonly IWillMessage willMessage;
        
        DateTime lastChannelActivityTime;
        StateFlags stateFlags;
        TaskCompletionSource subscribeCompletion;

        int InboundBacklogSize => this.deviceBoundOneWayProcessor.BacklogSize + this.deviceBoundTwoWayProcessor.BacklogSize;

        public MqttIotHubAdapter(
            string deviceId, 
            string iotHubHostName, 
            string password, 
            MqttTransportSettings mqttTransportSettings, 
            IWillMessage willMessage,
            Action onConnected,
            Action<Message> onMessageReceived,
            Action<Exception> onError)
        {
            Contract.Requires(deviceId != null);
            Contract.Requires(iotHubHostName != null);
            Contract.Requires(password != null);
            Contract.Requires(mqttTransportSettings != null);
            Contract.Requires(!mqttTransportSettings.HasWill || willMessage != null);

            this.deviceId = deviceId;
            this.iotHubHostName = iotHubHostName;
            this.password = password;
            this.mqttTransportSettings = mqttTransportSettings;
            this.willMessage = willMessage;
            this.onConnected = onConnected;
            this.onError = onError;
            this.onMessageReceived = onMessageReceived;
            this.pingRequestInterval = this.mqttTransportSettings.KeepAliveInSeconds > 0 ? TimeSpan.FromSeconds(this.mqttTransportSettings.KeepAliveInSeconds / 2d) : TimeSpan.MaxValue;

            this.deviceBoundOneWayProcessor = new SimpleWorkQueue<PublishPacket>(this.AcceptMessageAsync);
            this.deviceBoundTwoWayProcessor = new OrderedTwoPhaseWorkQueue<int, PublishPacket>(this.AcceptMessageAsync, p => p.PacketId, this.SendAckAsync);

            this.serviceBoundOneWayProcessor = new SimpleWorkQueue<PublishWorkItem>(this.SendMessageToServerAsync);
            this.serviceBoundTwoWayProcessor = new OrderedTwoPhaseWorkQueue<int, PublishWorkItem>(this.SendMessageToServerAsync, p => p.Value.PacketId, this.ProcessAckAsync);
        }

        #region IChannelHandler overrides

        public override void ChannelActive(IChannelHandlerContext context)
        {
            this.stateFlags = StateFlags.NotConnected;

            this.Connect(context);

            base.ChannelActive(context);
        }

        public override async Task WriteAsync(IChannelHandlerContext context, object data)
        {
            try
            {
                if (this.IsInState(StateFlags.Closed))
                {
                    return;
                }

                var message = data as Message;
                if (message != null)
                {
                    await this.SendMessageAsync(context, message);
                    return;
                }

                string packetIdString = data as string;
                if (packetIdString != null)
                {
                    await this.AcknowledgeAsync(context, packetIdString);
                    return;
                }

                if (data is DisconnectPacket)
                {
                    await Util.WriteMessageAsync(context, data, ShutdownOnWriteErrorHandler);
                    return;
                }

                if (data is SubscribePacket)
                {
                    await this.SubscribeAsync(context);
                    return;
                }

                throw new InvalidOperationException($"Unexpected data type: '{data.GetType().Name}'");
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                ShutdownOnError(context, ex);
                throw;
            }
        }

        public override void ChannelRead(IChannelHandlerContext context, object message)
        {
            var packet = message as Packet;
            if (packet == null)
            {
                return;
            }

            this.lastChannelActivityTime = DateTime.UtcNow; // notice last client activity - used in handling disconnects on keep-alive timeout

            if (this.IsInState(StateFlags.Connected) || (this.IsInState(StateFlags.Connecting) && packet.PacketType == PacketType.CONNACK))
            {
                this.ProcessMessage(context, packet);
            }
            else
            {
                // we did not start processing CONNACK yet which means we haven't received it yet but the packet of different type has arrived.
                ShutdownOnError(context, new InvalidOperationException($"Invalid state: {this.stateFlags}, packet: {packet.PacketType}"));
            }
        }

        public override void ChannelReadComplete(IChannelHandlerContext context)
        {
            base.ChannelReadComplete(context);
            if (this.InboundBacklogSize < this.mqttTransportSettings.MaxPendingInboundMessages)
            {
                context.Read();
            }
        }

        public override void ChannelInactive(IChannelHandlerContext context)
        {
            this.Shutdown(context);

            base.ChannelInactive(context);
        }

        public override void ExceptionCaught(IChannelHandlerContext context, Exception exception)
        {
            ShutdownOnError(context, exception);
        }

        public override void UserEventTriggered(IChannelHandlerContext context, object @event)
        {
            var handshakeCompletionEvent = @event as TlsHandshakeCompletionEvent;
            if (handshakeCompletionEvent != null && !handshakeCompletionEvent.IsSuccessful)
            {
                ShutdownOnError(context, handshakeCompletionEvent.Exception);
            }
        }

        #endregion

        #region Connect
        async void Connect(IChannelHandlerContext context)
        {
            try
            {
                var connectPacket = new ConnectPacket
                {
                    ClientId = this.deviceId,
                    HasUsername = true,
                    Username = this.iotHubHostName + "/" + this.deviceId,
                    HasPassword = !string.IsNullOrEmpty(this.password),
                    Password = this.password,
                    KeepAliveInSeconds = this.mqttTransportSettings.KeepAliveInSeconds,
                    CleanSession = this.mqttTransportSettings.CleanSession,
                    HasWill = this.mqttTransportSettings.HasWill
                };
                if (connectPacket.HasWill)
                {
                    Message message = this.willMessage.Message;
                    QualityOfService publishToServerQoS = this.mqttTransportSettings.PublishToServerQoS;
                    string topicName = string.Format(TelemetryTopicFormat, this.deviceId);
                    PublishPacket will = await Util.ComposePublishPacketAsync(context, message, publishToServerQoS, topicName);

                    connectPacket.WillMessage = will.Payload;
                    connectPacket.WillQualityOfService = this.willMessage.QoS;
                    connectPacket.WillRetain = false;
                    connectPacket.WillTopicName = will.TopicName;
                }
                this.stateFlags = StateFlags.Connecting;

                await Util.WriteMessageAsync(context, connectPacket, ShutdownOnWriteErrorHandler);
                this.lastChannelActivityTime = DateTime.UtcNow;
                this.ScheduleKeepConnectionAlive(context);

                this.ScheduleCheckConnectTimeout(context);
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                ShutdownOnError(context, ex);
            }
        }

        async void ScheduleKeepConnectionAlive(IChannelHandlerContext context)
        {
            try
            {
                await context.Channel.EventLoop.ScheduleAsync(PingServerCallback, context, this.pingRequestInterval);
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                ShutdownOnError(context, ex);
            }
        }

        async void ScheduleCheckConnectTimeout(IChannelHandlerContext context)
        {
            try
            {
                await context.Channel.EventLoop.ScheduleAsync(CheckConnAckTimeoutCallback, context, this.mqttTransportSettings.ConnectArrivalTimeout);
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                ShutdownOnError(context, ex);
            }
        }

        static void ShutdownIfNotReady(object state)
        {
            var context = (IChannelHandlerContext)state;
            var handler = (MqttIotHubAdapter)context.Handler;
            if (handler.IsInState(StateFlags.Connecting))
            {
                ShutdownOnError(context, new TimeoutException("Connection hasn't been established in time."));
            }
        }

        static async void PingServer(object ctx)
        {
            var context = (IChannelHandlerContext)ctx;
            try
            {
                var self = (MqttIotHubAdapter)context.Handler;

                if (!self.IsInState(StateFlags.Connected))
                {
                    return;
                }

                TimeSpan idleTime = DateTime.UtcNow - self.lastChannelActivityTime;

                if (idleTime > self.pingRequestInterval)
                {
                    // We've been idle for too long, send a ping!
                    await Util.WriteMessageAsync(context, PingReqPacket.Instance, ShutdownOnWriteErrorHandler);
                }

                self.ScheduleKeepConnectionAlive(context);
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                ShutdownOnError(context, ex);
            }
        }

        async Task ProcessConnectAckAsync(IChannelHandlerContext context, ConnAckPacket packet)
        {
            if (packet.ReturnCode != ConnectReturnCode.Accepted)
            {
                string reason = "CONNECT failed: " + packet.ReturnCode;
                var iotHubException = new UnauthorizedException(reason);
                ShutdownOnError(context, iotHubException);
                return;
            }

            if (!this.IsInState(StateFlags.Connecting))
            {
                string reason = "CONNECT has been received, however a session has already been established. Only one CONNECT/CONNACK pair is expected per session.";
                var iotHubException = new IotHubException(reason);
                ShutdownOnError(context, iotHubException);
                return;
            }

            this.stateFlags = StateFlags.Connected;

            this.onConnected();

            this.ResumeReadingIfNecessary(context);

            if (packet.SessionPresent)
            {
                await this.SubscribeAsync(context);
            }
        }
        #endregion

        #region Subscribe

        async Task SubscribeAsync(IChannelHandlerContext context)
        {
            if (this.IsInState(StateFlags.Receiving) || this.IsInState(StateFlags.Subscribing))
            {
                return;
            }

            this.stateFlags |= StateFlags.Subscribing;

            this.subscribeCompletion = new TaskCompletionSource();

            string topicFilter = CommandTopicFilterFormat.FormatInvariant(this.deviceId);

            var subscribePacket = new SubscribePacket(Util.GetNextPacketId(), new SubscriptionRequest(topicFilter, this.mqttTransportSettings.ReceivingQoS));

            await Util.WriteMessageAsync(context, subscribePacket, ShutdownOnWriteErrorHandler);

            await this.subscribeCompletion.Task;
        }

        void ProcessSubAck()
        {
            this.stateFlags &= ~StateFlags.Subscribing;
            this.stateFlags |= StateFlags.Receiving;
            this.subscribeCompletion.TryComplete();
        }

        #endregion

        #region Receiving

        async void ProcessMessage(IChannelHandlerContext context, Packet packet)
        {
            if (this.IsInState(StateFlags.Closed))
            {
                return;
            }

            try
            {
                switch (packet.PacketType)
                {
                    case PacketType.CONNACK:
                        await this.ProcessConnectAckAsync(context, (ConnAckPacket)packet);
                        break;
                    case PacketType.SUBACK:
                        this.ProcessSubAck();
                        break;
                    case PacketType.PUBLISH:
                        this.ProcessPublish(context, (PublishPacket)packet);
                        break;
                    case PacketType.PUBACK:
                        await this.serviceBoundTwoWayProcessor.CompleteWorkAsync(context, ((PubAckPacket)packet).PacketId);
                        break;
                    case PacketType.PINGRESP:
                        break;
                    default:
                        ShutdownOnError(context, new InvalidOperationException($"Unexpected packet type {packet.PacketType}"));
                        break;
                }
            }
            catch (Exception ex) when(!ex.IsFatal())
            {
                ShutdownOnError(context, ex);
            }
        }

        Task AcceptMessageAsync(IChannelHandlerContext context, PublishPacket publish)
        {
            Message message;
            try
            {
                var bodyStream = new ReadOnlyByteBufferStream(publish.Payload, true);

                message = new Message(bodyStream, true);

                Util.PopulateMessagePropertiesFromPacket(message, publish);
            }
            catch (Exception ex)
            {
                ShutdownOnError(context, ex);
                return TaskConstants.Completed;
            }
            this.onMessageReceived(message);
            return TaskConstants.Completed;
        }

        Task ProcessAckAsync(IChannelHandlerContext context, PublishWorkItem publish)
        {
            publish.Completion.Complete();
            return TaskConstants.Completed;
        }
        #endregion

        #region Sending
        void ProcessPublish(IChannelHandlerContext context, PublishPacket packet)
        {
            if (this.IsInState(StateFlags.Closed))
            {
                return;
            }

            switch (packet.QualityOfService)
            {
                case QualityOfService.AtMostOnce:
                    this.deviceBoundOneWayProcessor.Post(context, packet);
                    break;
                case QualityOfService.AtLeastOnce:
                    this.deviceBoundTwoWayProcessor.Post(context, packet);
                    break;
                default:
                    throw new NotSupportedException($"Unexpected QoS: '{packet.QualityOfService}'");
            }
            this.ResumeReadingIfNecessary(context);
        }

        async Task SendMessageAsync(IChannelHandlerContext context, Message message)
        {
            string topicName = string.Format(TelemetryTopicFormat, this.deviceId);

            PublishPacket packet = await Util.ComposePublishPacketAsync(context, message, this.mqttTransportSettings.PublishToServerQoS, topicName);
            var publishCompletion = new TaskCompletionSource();
            var workItem = new PublishWorkItem
            {
                Completion = publishCompletion,
                Value = packet
            };
            switch (this.mqttTransportSettings.PublishToServerQoS)
            {
                case QualityOfService.AtMostOnce:
                    this.serviceBoundOneWayProcessor.Post(context, workItem);
                    break;
                case QualityOfService.AtLeastOnce:
                    this.serviceBoundTwoWayProcessor.Post(context, workItem);
                    break;
                default:
                    throw new NotSupportedException($"Unsupported telemetry QoS: '{this.mqttTransportSettings.PublishToServerQoS}'");
            }
            await publishCompletion.Task;
        }

        Task AcknowledgeAsync(IChannelHandlerContext context, string packetIdString)
        {
            int packetId;
            if (int.TryParse(packetIdString, out packetId))
            {
                return this.deviceBoundTwoWayProcessor.CompleteWorkAsync(context, packetId);
            }
            return TaskConstants.Completed;
        }

        Task SendAckAsync(IChannelHandlerContext context, PublishPacket publish)
        {
            this.ResumeReadingIfNecessary(context);
            return Util.WriteMessageAsync(context, PubAckPacket.InResponseTo(publish), ShutdownOnWriteErrorHandler);
        }

        async Task SendMessageToServerAsync(IChannelHandlerContext context, PublishWorkItem publish)
        {
            if (!this.IsInState(StateFlags.Connected))
            {
                publish.Completion.TrySetCanceled();
            }
            try
            {
                await Util.WriteMessageAsync(context, publish.Value, ShutdownOnWriteErrorHandler);
                if (publish.Value.QualityOfService == QualityOfService.AtMostOnce)
                {
                    publish.Completion.TryComplete();
                }
            }
            catch (Exception ex)
            {
                publish.Completion.TrySetException(ex);
            }
        }
        #endregion

        #region Shutdown
        static async void ShutdownOnError(IChannelHandlerContext context, Exception exception)
        {
            var self = (MqttIotHubAdapter)context.Handler;
            if (!self.IsInState(StateFlags.Closed))
            {
                self.stateFlags |= StateFlags.Closed;
                self.subscribeCompletion?.TrySetException(exception);
                self.deviceBoundOneWayProcessor.Abort(exception);
                self.deviceBoundTwoWayProcessor.Abort(exception);
                self.serviceBoundOneWayProcessor.Abort(exception);
                self.serviceBoundTwoWayProcessor.Abort(exception);
                self.onError(exception);
                try
                {
                    await context.CloseAsync();
                }
                catch (Exception ex) when (!ex.IsFatal())
                {
                    //ignored
                }
            }
        }

        async void Shutdown(IChannelHandlerContext context)
        {
            if (this.IsInState(StateFlags.Closed))
            {
                return;
            }

            try
            {
                this.stateFlags |= StateFlags.Closed; // "or" not to interfere with ongoing logic which has to honor Closed state when it's right time to do (case by case)

                this.CloseIotHubConnection();
                await context.CloseAsync();
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                //ignored
            }
        }

        async void CloseIotHubConnection()
        {
            if (this.IsInState(StateFlags.NotConnected) || this.IsInState(StateFlags.Connecting))
            {
                // closure has happened before IoT Hub connection was established or it was initiated due to disconnect
                return;
            }

            try
            {
                this.serviceBoundOneWayProcessor.Complete();
                this.deviceBoundOneWayProcessor.Complete();
                this.serviceBoundTwoWayProcessor.Complete();
                this.deviceBoundTwoWayProcessor.Complete();
                await Task.WhenAll(
                    this.serviceBoundOneWayProcessor.Completion, 
                    this.serviceBoundTwoWayProcessor.Completion,
                    this.deviceBoundOneWayProcessor.Completion,
                    this.deviceBoundTwoWayProcessor.Completion);
            }
            catch (Exception completeEx) when (!completeEx.IsFatal())
            {
                try
                {
                    this.serviceBoundOneWayProcessor.Abort();
                    this.deviceBoundOneWayProcessor.Abort();
                    this.serviceBoundTwoWayProcessor.Abort();
                    this.deviceBoundTwoWayProcessor.Abort();
                }
                catch (Exception abortEx) when (!abortEx.IsFatal())
                {
                    // ignored on closing
                }
            }
        }
        #endregion

        #region helper methods

        void ResumeReadingIfNecessary(IChannelHandlerContext context)
        {
            if (this.InboundBacklogSize == this.mqttTransportSettings.MaxPendingInboundMessages - 1) // we picked up a packet from full queue - now we have more room so order another read
            {
                context.Read();
            }
        }

        bool IsInState(StateFlags stateFlagsToCheck)
        {
            return (this.stateFlags & stateFlagsToCheck) == stateFlagsToCheck;
        }

        IByteBuffer GetWillMessageBody(Message message)
        {
            Stream bodyStream = message.GetBodyStream();
            var buffer = new byte[bodyStream.Length];
            bodyStream.Read(buffer, 0, buffer.Length);
            IByteBuffer copiedBuffer = Unpooled.CopiedBuffer(buffer);
            return copiedBuffer;
        }
        #endregion
    }
}