// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Generic;
    using System.Text.RegularExpressions;
    using Microsoft.Azure.Devices.Client.Extensions;
#if !WINDOWS_UWP
    using System.Linq;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client.Exceptions;
#endif

    // C# using aliases cannot name an unbound generic type declaration without supplying type arguments
    // Therefore, define a separate alias for each type argument
#if WINDOWS_UWP
    using AsyncTask = Windows.Foundation.IAsyncAction;
    using AsyncTaskOfMessage = Windows.Foundation.IAsyncOperation<Message>;
#else
    using AsyncTask = System.Threading.Tasks.Task;
    using AsyncTaskOfMessage = System.Threading.Tasks.Task<Message>;
#endif

    /// <summary>
    /// Transport types supported by DeviceClient - Amqp and HTTP 1.1
    /// </summary>
    public enum TransportType
    {
        /// <summary>
        /// Advanced Message Queuing Protocol transport.
        /// Try Amqp over TCP first and fallback to Amqp over WebSocket if that fails
        /// </summary>
        Amqp = 0,

        /// <summary>
        /// HyperText Transfer Protocol version 1 transport.
        /// </summary>
        Http1 = 1,

        /// <summary>
        /// Advanced Message Queuing Protocol transport over WebSocket only.
        /// </summary>
        Amqp_WebSocket_Only = 2,

        /// <summary>
        /// Advanced Message Queuing Protocol transport over native TCP only
        /// </summary>
        Amqp_Tcp_Only = 3
    }

    /// <summary>
    /// Contains methods that a device can use to send messages to and receive from the service.
    /// </summary>
    public sealed class DeviceClient
    {
        const string DeviceId = "DeviceId";
        const string DeviceIdParameterPattern = @"(^\s*?|.*;\s*?)" + DeviceId + @"\s*?=.*";
        const RegexOptions RegexOptions = System.Text.RegularExpressions.RegexOptions.Compiled | System.Text.RegularExpressions.RegexOptions.IgnoreCase;

        static readonly Regex DeviceIdParameterRegex = new Regex(DeviceIdParameterPattern, RegexOptions);
        DeviceClientHelper impl;
#if !WINDOWS_UWP
        readonly IotHubConnectionString iotHubConnectionString;
        readonly ITransportSettings[] transportSettings;
        volatile TaskCompletionSource<object> openTaskCompletionSource;
        bool openCalled;

        DeviceClient(IotHubConnectionString iotHubConnectionString, ITransportSettings[] transportSettings)
        {
            this.iotHubConnectionString = iotHubConnectionString;
            this.transportSettings = transportSettings;
            this.ThisLock = new object();
            this.openTaskCompletionSource = new TaskCompletionSource<object>(this);
        }

        object ThisLock { get; }
#else
        DeviceClient(DeviceClientHelper impl, TransportType transportType)
        {
            this.impl = impl;
            this.TransportTypeInUse = transportType;
        }
#endif
        public TransportType TransportTypeInUse { get; private set; }

        /// <summary>
        /// Create an Amqp DeviceClient from individual parameters
        /// </summary>
        /// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        /// <param name="authenticationMethod">The authentication method that is used</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient Create(string hostname, IAuthenticationMethod authenticationMethod)
        {
            return Create(hostname, authenticationMethod, TransportType.Amqp);
        }

        /// <summary>
        /// Create a DeviceClient from individual parameters
        /// </summary>
        /// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        /// <param name="authenticationMethod">The authentication method that is used</param>
        /// <param name="transportType">The transportType used (Http1 or Amqp)</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient Create(string hostname, IAuthenticationMethod authenticationMethod, TransportType transportType)
        {
            if (hostname == null)
            {
                throw new ArgumentNullException("hostname");
            }

            if (authenticationMethod == null)
            {
                throw new ArgumentNullException("authenticationMethod");
            }

            var connectionStringBuilder = IotHubConnectionStringBuilder.Create(hostname, authenticationMethod);
            return CreateFromConnectionString(connectionStringBuilder.ToString(), transportType);
        }

        /// <summary>
        /// Create a DeviceClient using Amqp transport from the specified connection string
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (including DeviceId)</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString)
        {
#if WINDOWS_UWP
            return CreateFromConnectionString(connectionString, TransportType.Http1);
#else
            return CreateFromConnectionString(connectionString, TransportType.Amqp);
#endif

        }

        /// <summary>
        /// Create a DeviceClient using Amqp transport from the specified connection string
        /// (WinRT) Create a DeviceClient using Http transport from the specified connection string
        /// </summary>
        /// <param name="connectionString">IoT Hub-Scope Connection string for the IoT hub (without DeviceId)</param>
        /// <param name="deviceId">Id of the Device</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, string deviceId)
        {
#if WINDOWS_UWP
            return CreateFromConnectionString(connectionString, deviceId, TransportType.Http1);    
#else
            return CreateFromConnectionString(connectionString, deviceId, TransportType.Amqp);
#endif
        }

#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute()]
#endif
        /// <summary>
        /// Create DeviceClient from the specified connection string using the specified transport type
        /// (WinRT) Only Http transport is allowed
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (including DeviceId)</param>
        /// <param name="transportType">Specifies whether Amqp or Http transport is used</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, TransportType transportType)
        {
            if (connectionString  == null)
            {
                throw new ArgumentNullException("connectionString");
            }

            switch (transportType)
            {
                case TransportType.Amqp:
#if WINDOWS_UWP
                    throw new NotImplementedException("Amqp protocol is not supported");
#else
                    return CreateFromConnectionString(connectionString, new ITransportSettings[]
                    {
                        new AmqpTransportSettings(TransportType.Amqp_Tcp_Only),
                        new AmqpTransportSettings(TransportType.Amqp_WebSocket_Only)
                    });
#endif
                case TransportType.Amqp_WebSocket_Only:
                case TransportType.Amqp_Tcp_Only:
#if WINDOWS_UWP
                    throw new NotImplementedException("Amqp protocol is not supported");
#else
                    return CreateFromConnectionString(connectionString, new ITransportSettings[] { new AmqpTransportSettings(transportType) });
#endif
                case TransportType.Http1:
#if WINDOWS_UWP
                    var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
                    return new DeviceClient(new HttpDeviceClient(iotHubConnectionString), TransportType.Http1);
#else
                    return CreateFromConnectionString(connectionString, new ITransportSettings[] { new Http1TransportSettings() });
#endif
                default:
                    throw new InvalidOperationException("Unsupported Transport Type {0}".FormatInvariant(transportType));
            }
        }

        /// <summary>
        /// Create DeviceClient from the specified connection string using the specified transport type
        /// </summary>
        /// <param name="connectionString">IoT Hub-Scope Connection string for the IoT hub (without DeviceId)</param>
        /// <param name="deviceId">Id of the device</param>
        /// <param name="transportType">The transportType used (Http1 or Amqp)</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, string deviceId, TransportType transportType)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException("connectionString");
            }

            if (deviceId == null)
            {
                throw new ArgumentNullException("deviceId");
            }

            if (DeviceIdParameterRegex.IsMatch(connectionString))
            {
                throw new ArgumentException("connectionString must not contain DeviceId keyvalue parameter", "connectionString");
            }

            return CreateFromConnectionString(connectionString + ";" + DeviceId + "=" + deviceId, transportType);
        }
        
#if !WINDOWS_UWP
        /// <summary>
        /// Create DeviceClient from the specified connection string using a prioritized list of transports
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (with DeviceId)</param>
        /// <param name="transportSettings">Prioritized list of transportSettings</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, ITransportSettings[] transportSettings)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException("connectionString");
            }

            if (transportSettings == null) 
            {
                throw new ArgumentNullException("transportSettings");
            }

            if (transportSettings.Length == 0)
            {
                throw new ArgumentOutOfRangeException("connectionString", "Must specify atleast one TransportSetting instance");
            }

            var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);

            foreach (var transportSetting in transportSettings)
            {
                switch (transportSetting.GetTransportType())
                {
                    case TransportType.Amqp_WebSocket_Only:
                    case TransportType.Amqp_Tcp_Only:
                        var amqpTransportSetting = transportSetting as AmqpTransportSettings;
                        if (amqpTransportSetting == null)
                        {
                            throw new InvalidOperationException("Unknown implementation of ITransportSettings type");
                        }
                        break;
                    case TransportType.Http1:
                        var httpTransportSetting = transportSetting as Http1TransportSettings;
                        if (httpTransportSetting == null)
                        {
                            throw new InvalidOperationException("Unknown implementation of ITransportSettings type");
                        }
                        break;
                    case TransportType.Amqp:
                    default:
                        throw new InvalidOperationException("Unsupported Transport Setting {0}".FormatInvariant(transportSetting));
                }
            }

            // Defer concrete DeviceClient creation to OpenAsync
            return new DeviceClient(iotHubConnectionString, transportSettings);
        }

        /// <summary>
        /// Create DeviceClient from the specified connection string using the prioritized list of transports
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (without DeviceId)</param>
        /// <param name="deviceId">Id of the device</param>
        /// <param name="transportSettings">Prioritized list of transportTypes</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, string deviceId, ITransportSettings[] transportSettings)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException("connectionString");
            }

            if (deviceId == null)
            {
                throw new ArgumentNullException("deviceId");
            }

            if (DeviceIdParameterRegex.IsMatch(connectionString))
            {
                throw new ArgumentException("connectionString must not contain DeviceId keyvalue parameter", "connectionString");
            }

            return CreateFromConnectionString(connectionString + ";" + DeviceId + "=" + deviceId, transportSettings);
        }
#endif

        /// <summary>
        /// Explicitly open the DeviceClient instance.
        /// </summary>
        public AsyncTask OpenAsync()
        {
#if WINDOWS_UWP
            return impl.OpenAsync().AsTaskOrAsyncOp();
#else
            if (this.impl != null)
            {
                return this.impl.OpenAsync().AsTaskOrAsyncOp();
            }

            return this.EnsureOpenedAsync();
#endif
        }

        /// <summary>
        /// Close the DeviceClient instance
        /// </summary>
        /// <returns></returns>
        public AsyncTask CloseAsync()
        {
#if !WINDOWS_UWP
            if (this.impl != null)
            {
#endif
                return this.impl.CloseAsync().AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }

            return TaskHelpers.CompletedTask;
#endif
        }

        /// <summary>
        /// Receive a message from the device queue using the default timeout.
        /// </summary>
        /// <returns>The receive message or null if there was no message until the default timeout</returns>
        public AsyncTaskOfMessage ReceiveAsync()
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    return await this.impl.ReceiveAsync();
                });
            }
            else
            {
#endif
                return this.impl.ReceiveAsync().AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

        /// <summary>
        /// Receive a message from the device queue with the specified timeout
        /// </summary>
        /// <returns>The receive message or null if there was no message until the specified time has elapsed</returns>
        public AsyncTaskOfMessage ReceiveAsync(TimeSpan timeout)
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    return await this.impl.ReceiveAsync(timeout);
                });
            }
            else
            {
#endif
                return this.impl.ReceiveAsync().AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute()]
#endif
        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public AsyncTask CompleteAsync(string lockToken)
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    await this.impl.CompleteAsync(lockToken).AsTaskOrAsyncOp();
                });
            }
            else
            {
#endif
                return this.impl.CompleteAsync(lockToken).AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public AsyncTask CompleteAsync(Message message)
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    await this.impl.CompleteAsync(message).AsTaskOrAsyncOp();
                });
            }
            else
            {
#endif
                return this.impl.CompleteAsync(message).AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute()]
#endif
        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public AsyncTask AbandonAsync(string lockToken)
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    await this.impl.AbandonAsync(lockToken).AsTaskOrAsyncOp();
                });
            }
            else
            {
#endif
                return this.impl.AbandonAsync(lockToken).AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public AsyncTask AbandonAsync(Message message)
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    await this.impl.AbandonAsync(message).AsTaskOrAsyncOp();
                });
            }
            else
            {
#endif
                return this.impl.AbandonAsync(message).AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute()]
#endif
        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The previously received message</returns>
        public AsyncTask RejectAsync(string lockToken)
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    await this.impl.RejectAsync(lockToken).AsTaskOrAsyncOp();
                });
            }
            else
            {
#endif
                return this.impl.RejectAsync(lockToken).AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public AsyncTask RejectAsync(Message message)
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    await this.impl.RejectAsync(message).AsTaskOrAsyncOp();
                });
            }
            else
            {
#endif
                return this.impl.RejectAsync(message).AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

        /// <summary>
        /// Sends an event to device hub
        /// </summary>
        /// <returns>The message containing the event</returns>
        public AsyncTask SendEventAsync(Message message)
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    await this.impl.SendEventAsync(message);
                });
            }
            else
            {
#endif
                return this.impl.SendEventAsync(message).AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

        /// <summary>
        /// Sends a batch of events to device hub
        /// </summary>
        /// <returns>The task containing the event</returns>
        public AsyncTask SendEventBatchAsync(IEnumerable<Message> messages)
        {
#if !WINDOWS_UWP
            if (this.impl == null)
            {
                return AsyncTask.Run(async () =>
                {
                    await this.OpenAsync();

                    await this.impl.SendEventBatchAsync(messages).AsTaskOrAsyncOp();
                });
            }
            else
            {
#endif
                return this.impl.SendEventBatchAsync(messages).AsTaskOrAsyncOp();
#if !WINDOWS_UWP
            }
#endif
        }

#if !WINDOWS_UWP
        Task EnsureOpenedAsync()
        {
            bool needOpen = false;
            Task openTask;
            if (this.openTaskCompletionSource != null)
            {
                lock (this.ThisLock)
                {
                    if (this.openCalled)
                    {
                        if (this.openTaskCompletionSource == null)
                        {
                            // openTaskCompletionSource being null means open has finished completely
                            openTask = TaskHelpers.CompletedTask;
                        }
                        else
                        {
                            openTask = this.openTaskCompletionSource.Task;
                        }
                    }
                    else
                    {
                        // It's this call's job to kick off the open.
                        this.openCalled = true;
                        openTask = this.openTaskCompletionSource.Task;
                        needOpen = true;
                    }
                }
            }
            else
            {
                // Open has already fully completed.
                openTask = TaskHelpers.CompletedTask;
            }

            if (needOpen)
            {
                this.TryOpenPrioritizedTransportsAsync().ContinueWith(
                    t =>
                    {
                        var localOpenTaskCompletionSource = this.openTaskCompletionSource;
                        lock (this.ThisLock)
                        {
                            if (!t.IsFaulted && !t.IsCanceled)
                            {
                                // This lets future calls avoid the Open logic all together.
                                this.openTaskCompletionSource = null;
                            }
                            else
                            {
                                // OpenAsync was cancelled or threw an exception, next time retry.
                                this.openCalled = false;
                                this.openTaskCompletionSource = new TaskCompletionSource<object>(this);
                            }
                        }

                        // This completes anyone waiting for open to finish
                        TaskHelpers.MarshalTaskResults(t, localOpenTaskCompletionSource);
                    });
            }

            return openTask;
        }

        async AsyncTask TryOpenPrioritizedTransportsAsync()
        {
            Exception lastException = null;
            // Concrete Device Client creation was deferred. Use prioritized list of transports.
            foreach (var transportSetting in this.transportSettings)
            {
                DeviceClientHelper helper;
                try
                {
                    switch (transportSetting.GetTransportType())
                    {                    
                        case TransportType.Amqp_WebSocket_Only:
                        case TransportType.Amqp_Tcp_Only:
                            helper = new AmqpDeviceClient(this.iotHubConnectionString, transportSetting as AmqpTransportSettings);
                            break;
                        case TransportType.Http1:
                            helper = new HttpDeviceClient(this.iotHubConnectionString, transportSetting as Http1TransportSettings);
                            break;
                        case TransportType.Amqp:
                        default:
                            throw new InvalidOperationException("Unsupported Transport Setting {0}".FormatInvariant(transportSetting));
                    }

                    // Try to open a connection with this transport
                    await helper.OpenAsync();
                }
                catch (Exception exception)
                {
                    if (exception.IsFatal() || exception is UnauthorizedException || exception is InvalidOperationException)
                    {
                        throw;
                    }

                    lastException = exception;

                    // open connection failed. Move to next transport type
                    continue;
                }

                // Success - return this transport type
                this.impl = helper;
                this.TransportTypeInUse = transportSetting.GetTransportType();

                return;
            }

            if (lastException != null)
            {
                throw lastException;
            }

            throw new InvalidOperationException("Unsupported Transport Settings {0}".FormatInvariant(this.transportSettings.ToList()));
        }
#endif
    }
}