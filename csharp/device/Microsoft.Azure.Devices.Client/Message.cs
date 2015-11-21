// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.IO;
    using System.Threading;
#if WINDOWS_UWP && !NETMF
    using Windows.Storage.Streams;
#elif !NETMF
    using Microsoft.Azure.Amqp;
#endif

#if NETMF
    using System.Collections;
#else
    using Microsoft.Azure.Devices.Client.Common.Api;
    using System.Collections.Generic;
#endif
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;

    // TODO: can we use DateTimeOffset for WinRT as well? With conversion to local time?
#if WINDOWS_UWP
    using DateTimeT = System.DateTimeOffset;
#else
    using DateTimeT = System.DateTime;
#endif

    /// <summary>
    /// The data structure represent the message that is used for interacting with IotHub.
    /// </summary>
    public sealed class Message :
#if !WINDOWS_UWP && !NETMF
        IDisposable, IReadOnlyIndicator
#elif NETMF
        IDisposable
#elif WINDOWS_UWP
        IReadOnlyIndicator
#endif
    {
        readonly object messageLock = new object();
#if NETMF
        Stream bodyStream;
#else
        volatile Stream bodyStream;
#endif
        bool disposed;
        bool ownsBodyStream;
        int getBodyCalled;
#if NETMF
        int sizeInBytesCalled;
#else
        long sizeInBytesCalled;
#endif

#if !WINDOWS_UWP && !NETMF
        AmqpMessage serializedAmqpMessage;
#endif

        /// <summary>
        /// Default constructor with no body data
        /// </summary>
        public Message()
        {
#if NETMF
            this.Properties = new Hashtable();
            this.SystemProperties = new Hashtable();
            //this.serializedAmqpMessage = null;
#else
            this.Properties = new ReadOnlyDictionary45<string, string>(new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase), this);
            this.SystemProperties = new ReadOnlyDictionary45<string, object>(new Dictionary<string, object>(StringComparer.OrdinalIgnoreCase), this);
            this.InitializeWithStream(Stream.Null, true);
#if !WINDOWS_UWP
            this.serializedAmqpMessage = null;
#endif
#endif
        }

        /// <summary>
        /// Constructor which uses the argument stream as the body stream.
        /// </summary>
        /// <param name="stream">a stream which will be used as body stream.</param>
        /// <remarks>User is expected to own the disposing of the stream when using this constructor.</remarks>
        // UWP cannot expose a method with System.IO.Stream in signature. TODO: consider adding an IRandomAccessStream overload
#if WINDOWS_UWP
        private
#else
        public 
#endif
            Message(Stream stream)
            : this()
        {
            if (stream != null)
            {
                this.InitializeWithStream(stream, false);
            }
        }

        /// <summary>
        /// Constructor which uses the input byte array as the body
        /// </summary>
        /// <param name="byteArray">a byte array which will be used to
        /// form the body stream</param>
        /// <remarks>user should treat the input byte array as immutable when
        /// sending the message.</remarks>
#if NETMF
        public Message(byte[] byteArray)
            : this(new MemoryStream(byteArray))
#else
        public Message([System.Runtime.InteropServices.WindowsRuntime.ReadOnlyArrayAttribute] byte[] byteArray)
            : this(new MemoryStream(byteArray))
#endif
        {
            // reset the owning of the steams
            this.ownsBodyStream = true;
        }

#if !WINDOWS_UWP && !NETMF
        /// <summary>
        /// This constructor is only used in the receive path from Amqp path, 
        /// or in Cloning from a Message that has serialized.
        /// </summary>
        /// <param name="amqpMessage"></param>
        internal Message(AmqpMessage amqpMessage)
            : this()
        {
            if (amqpMessage == null)
            {
                throw Fx.Exception.ArgumentNull("amqpMessage");
            }

            MessageConverter.UpdateMessageHeaderAndProperties(amqpMessage, this);
            Stream stream = amqpMessage.BodyStream;
            this.InitializeWithStream(stream, true);
        }
#endif
        /// <summary>
        /// This constructor is only used on the Gateway http path so that 
        /// we can clean up the stream.
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="ownStream"></param>
        internal Message(Stream stream, bool ownStream)
            : this(stream)
        {
            this.ownsBodyStream = ownStream;
        }

        /// <summary>
        /// [Required for two way requests] Used to correlate two-way communication. 
        /// Format: A case-sensitive string ( up to 128 char long) of ASCII 7-bit alphanumeric chars 
        /// + {'-', ':', '/', '\', '.', '+', '%', '_', '#', '*', '?', '!', '(', ')', ',', '=', '@', ';', '$', '''}. 
        /// Non-alphanumeric characters are from URN RFC.
        /// </summary>
        public string MessageId
        {
            get
            {
#if NETMF
                return (this.GetSystemProperty(MessageSystemPropertyNames.MessageId)) as string;
#else
                return this.GetSystemProperty<string>(MessageSystemPropertyNames.MessageId);
#endif
            }

            set
            {
                this.SystemProperties[MessageSystemPropertyNames.MessageId] = value;
            }
        }

        /// <summary>
        /// [Required] Destination of the message
        /// </summary>
        public string To
        {
            get
            {
#if NETMF
                return (string)(this.GetSystemProperty(MessageSystemPropertyNames.To) ?? string.Empty);
#else
                return this.GetSystemProperty<string>(MessageSystemPropertyNames.To);
#endif
            }

            set
            {
                this.SystemProperties[MessageSystemPropertyNames.To] = value;
            }
        }

        /// <summary>
        /// [Optional] The time when this message is considered expired
        /// </summary>
        public DateTimeT ExpiryTimeUtc
        {
            get
            {
#if NETMF
                return (DateTime)(this.GetSystemProperty(MessageSystemPropertyNames.ExpiryTimeUtc) ?? DateTime.MinValue);
#else
                return this.GetSystemProperty<DateTimeT>(MessageSystemPropertyNames.ExpiryTimeUtc);
#endif
            }

            internal set
            {
                this.SystemProperties[MessageSystemPropertyNames.ExpiryTimeUtc] = value;
            }
        }

        /// <summary>
        /// Used in message responses and feedback
        /// </summary>
        public string CorrelationId
        {
            get
            {
#if NETMF
                return (string)(this.GetSystemProperty(MessageSystemPropertyNames.CorrelationId) ?? string.Empty);
#else
                return this.GetSystemProperty<string>(MessageSystemPropertyNames.CorrelationId);
#endif
            }

            set
            {
                this.SystemProperties[MessageSystemPropertyNames.CorrelationId] = value;
            }
        }

        /// <summary>
        /// Indicates whether consumption or expiration of the message should post data to the feedback queue
        /// </summary>
        DeliveryAcknowledgement Ack
        {
            get
            {
#if NETMF
                string deliveryAckAsString = (string)(this.GetSystemProperty(MessageSystemPropertyNames.Ack) ?? string.Empty);
#else
                string deliveryAckAsString = this.GetSystemProperty<string>(MessageSystemPropertyNames.Ack);
#endif
                if (!deliveryAckAsString.IsNullOrWhiteSpace())
                {
                    switch (deliveryAckAsString)
                    {
                        case "none":
                            return DeliveryAcknowledgement.None;
                        case "positive":
                            return DeliveryAcknowledgement.PositiveOnly;
                        case "negative":
                            return DeliveryAcknowledgement.NegativeOnly;
                        case "full":
                            return DeliveryAcknowledgement.Full;
                        default:
                            throw new IotHubException("Invalid Delivery Ack mode");
                    }
                }

                return DeliveryAcknowledgement.None;
            }
            set
            {
                string valueToSet = null;

                switch (value)
                {
                    case DeliveryAcknowledgement.None:
                        valueToSet = "none";
                        break;
                    case DeliveryAcknowledgement.PositiveOnly:
                        valueToSet = "positive";
                        break;
                    case DeliveryAcknowledgement.NegativeOnly:
                        valueToSet = "negative";
                        break;
                    case DeliveryAcknowledgement.Full:
                        valueToSet = "full";
                        break;
                    default:
                        throw new IotHubException("Invalid Delivery Ack mode");
                }

                this.SystemProperties[MessageSystemPropertyNames.Ack] = valueToSet;
            }
        }

        /// <summary>
        /// [Required] SequenceNumber of the received message
        /// </summary>
        public ulong SequenceNumber
        {
            get
            {
#if NETMF
                return (ulong)(this.GetSystemProperty(MessageSystemPropertyNames.SequenceNumber) ?? 0);
#else
                return this.GetSystemProperty<ulong>(MessageSystemPropertyNames.SequenceNumber);                
#endif
            }

            internal set
            {
                this.SystemProperties[MessageSystemPropertyNames.SequenceNumber] = value;
            }
        }

        /// <summary>
        /// [Required] LockToken of the received message
        /// </summary>
        public string LockToken
        {
            get
            {
#if NETMF
                return (string)(this.GetSystemProperty(MessageSystemPropertyNames.LockToken) ?? string.Empty);
#else
                return this.GetSystemProperty<string>(MessageSystemPropertyNames.LockToken);
#endif
            }

            internal set
            {
                this.SystemProperties[MessageSystemPropertyNames.LockToken] = value;
            }
        }

        /// <summary>
        /// Time when the message was received by the server
        /// </summary>
        public DateTimeT EnqueuedTimeUtc
        {
            get
            {
#if NETMF
                return (DateTime)(this.GetSystemProperty(MessageSystemPropertyNames.EnqueuedTime) ?? DateTime.MinValue);
#else
                return this.GetSystemProperty<DateTimeT>(MessageSystemPropertyNames.EnqueuedTime);
#endif
            }

            internal set
            {
                this.SystemProperties[MessageSystemPropertyNames.EnqueuedTime] = value;
            }
        }

        /// <summary>
        /// Time when the message was received by the server
        /// </summary>
        public uint DeliveryCount
        {
            get
            {
#if NETMF
                return (byte)(this.GetSystemProperty(MessageSystemPropertyNames.DeliveryCount) ?? 0);
#else
                return this.GetSystemProperty<byte>(MessageSystemPropertyNames.DeliveryCount);
#endif
            }

            internal set
            {
                this.SystemProperties[MessageSystemPropertyNames.DeliveryCount] = (byte)value;
            }
        }

        /// <summary>
        /// [Required in feedback messages] Used to specify the origin of messages generated by device hub. 
        /// Possible value: “{hub name}/”
        /// </summary>
        public string UserId
        {
            get
            {
#if NETMF
                return (string)(this.GetSystemProperty(MessageSystemPropertyNames.UserId) ?? string.Empty);
#else
                return this.GetSystemProperty<string>(MessageSystemPropertyNames.UserId);
#endif
            }

            set
            {
                this.SystemProperties[MessageSystemPropertyNames.UserId] = value;
            }
        }

        /// <summary>
        /// Gets the dictionary of user properties which are set when user send the data.
        /// </summary>
#if NETMF
        public Hashtable Properties { get; private set; }
#else
         public IDictionary<string, string> Properties { get; private set; }
#endif

        /// <summary>
        /// Gets the dictionary of system properties which are managed internally.
        /// </summary>
#if NETMF
        internal Hashtable SystemProperties { get; private set; }
#else
        internal IDictionary<string, object> SystemProperties { get; private set; }
#endif

#if !NETMF
        bool IReadOnlyIndicator.IsReadOnly
        {
            get
            {
                return Interlocked.Read(ref this.sizeInBytesCalled) == 1;
            }
        }
#endif

#if !WINDOWS_UWP
        public
#endif
        Stream BodyStream
        {
            get
            {
                return this.bodyStream;
            }
        }

#if !WINDOWS_UWP && !NETMF
        internal AmqpMessage SerializedAmqpMessage
        {
            get
            {
                lock (this.messageLock)
                {
                    return this.serializedAmqpMessage;
                }
            }
        }
#endif

#if !NETMF
        /// <summary>
        /// Gets or sets the deliveryTag which is used for server side checkpointing.
        /// </summary>
        internal ArraySegment<byte> DeliveryTag { get; set; }
#endif
        /// <summary>
        /// Makes a clone of the current event data instance.
        /// </summary>
        /// <returns></returns>
        /// <exception cref="ObjectDisposedException">throws if the event data has already been disposed.</exception>
        public Message Clone()
        {
            this.ThrowIfDisposed();
#if !WINDOWS_UWP && !NETMF
            if (this.serializedAmqpMessage != null)
            {
                return new Message(this.serializedAmqpMessage);
            }
#endif
            var message = new Message();
            if (this.bodyStream != null)
            {
                // The new Message always owns the cloned stream.
                message = new Message(CloneStream(this.bodyStream))
                {
                    ownsBodyStream = true
                };
            }

#if NETMF
            foreach (var systemProperty in this.SystemProperties.Keys)
            {
                message.SystemProperties[systemProperty] = this.SystemProperties[systemProperty];
            }

            foreach (var property in this.Properties.Keys)
            {
                message.Properties.Add(property, this.Properties[property]);
            }
#else
            foreach (var systemProperty in this.SystemProperties)
            {
                // MessageId would already be there.
                if (message.SystemProperties.ContainsKey(systemProperty.Key))
                {
                    message.SystemProperties[systemProperty.Key] = systemProperty.Value;
                }
                else
                {
                    message.SystemProperties.Add(systemProperty);
                }
            }

            foreach (var property in this.Properties)
            {
                message.Properties.Add(property);
            }
#endif

            return message;
        }

#if !WINDOWS_UWP
        /// <summary>
        /// Dispose the current event data instance
        /// </summary>
        public void Dispose()
        {
            this.Dispose(true);
        }

        /// <summary>
        /// Return the body stream of the current event data instance
        /// </summary>
        /// <returns></returns>
        /// <exception cref="InvalidOperationException">throws if the method has been called.</exception>
        /// <exception cref="ObjectDisposedException">throws if the event data has already been disposed.</exception>
        /// <remarks>This method can only be called once and afterwards method will throw <see cref="InvalidOperationException"/>.</remarks>
        public Stream GetBodyStream()
        {
            this.ThrowIfDisposed();
            this.SetGetBodyCalled();
            if (this.bodyStream != null)
            {
                return this.bodyStream;
            }

#if NETMF
            return null;
#else
            return Stream.Null;
#endif
        }
#endif

        /// <summary>
        /// This methods return the body stream as a byte array
        /// </summary>
        /// <returns></returns>
        /// <exception cref="InvalidOperationException">throws if the method has been called.</exception>
        /// <exception cref="ObjectDisposedException">throws if the event data has already been disposed.</exception>
        public byte[] GetBytes()
        {
            this.ThrowIfDisposed();
            this.SetGetBodyCalled();
            if (this.bodyStream == null)
            {
                return new byte[] { };
            }

#if !WINDOWS_UWP && !NETMF
            BufferListStream listStream;
            if ((listStream = this.bodyStream as BufferListStream) != null)
            {
                // We can trust Amqp bufferListStream.Length;
                byte[] bytes = new byte[listStream.Length];
                listStream.Read(bytes, 0, bytes.Length);
                return bytes;
            }
#endif

            // This is just fail safe code in case we are not using the Amqp protocol.
            return ReadFullStream(this.bodyStream);
        }

#if !WINDOWS_UWP && !NETMF
        internal AmqpMessage ToAmqpMessage(bool setBodyCalled = true)
        {
            this.ThrowIfDisposed();
            if (this.serializedAmqpMessage == null)
            {
                lock (this.messageLock)
                {
                    if (this.serializedAmqpMessage == null)
                    {
                        // Interlocked exchange two variable does allow for a small period 
                        // where one is set while the other is not. Not sure if it is worth
                        // correct this gap. The intention of setting this two variable is
                        // so that GetBody should not be called and all Properties are
                        // readonly because the amqpMessage has been serialized.
                        if (setBodyCalled)
                        {
                            this.SetGetBodyCalled();
                        }

                        this.SetSizeInBytesCalled();
                        this.serializedAmqpMessage = this.bodyStream == null
                            ? AmqpMessage.Create()
                            : AmqpMessage.Create(this.bodyStream, false);
                        this.serializedAmqpMessage = this.PopulateAmqpMessageForSend(this.serializedAmqpMessage);
                    }
                }
            }

            return this.serializedAmqpMessage;
        }
#endif
        // Test hook only
        internal void ResetGetBodyCalled()
        {
            Interlocked.Exchange(ref this.getBodyCalled, 0);
            if (this.bodyStream != null && this.bodyStream.CanSeek)
            {
                this.bodyStream.Seek(0, SeekOrigin.Begin);
            }
        }

        void SetGetBodyCalled()
        {
            if (1 == Interlocked.Exchange(ref this.getBodyCalled, 1))
            {
#if NETMF
                throw new InvalidOperationException();
#else
                throw Fx.Exception.AsError(new InvalidOperationException(ApiResources.MessageBodyConsumed));
#endif
            }
        }

        void SetSizeInBytesCalled()
        {
            Interlocked.Exchange(ref this.sizeInBytesCalled, 1);
        }

        void InitializeWithStream(Stream stream, bool ownsStream)
        {
            // This method should only be used in constructor because
            // this has no locking on the bodyStream.
            this.bodyStream = stream;
            this.ownsBodyStream = ownsStream;
        }

        static byte[] ReadFullStream(Stream inputStream)
        {
#if NETMF
            inputStream.Position = 0;
            byte[] buffer = new byte[inputStream.Length];

            inputStream.Read(buffer, 0, (int)inputStream.Length);

            return buffer;
#else
            using (var ms = new MemoryStream())
            {
                inputStream.CopyTo(ms);
                return ms.ToArray();
            }
#endif
        }

        static Stream CloneStream(Stream originalStream)
        {
            if (originalStream != null)
            {
                MemoryStream memoryStream;
                if ((memoryStream = originalStream as MemoryStream) != null)
                {
                    // Note: memoryStream.GetBuffer() doesn't work
#if !NETMF
                    return new MemoryStream(memoryStream.ToArray(), 0, (int)memoryStream.Length, false, true);
#endif
                }
#if !WINDOWS_UWP
                ICloneable cloneable;

                if ((cloneable = originalStream as ICloneable) != null)
                {
                    return (Stream)cloneable.Clone();
                }
#endif
                if (originalStream.Length == 0)
                {
                    // This can happen in Stream.Null
#if NETMF
                    return null;
                }

                throw new Exception("Does not support cloning of Stream Type: " + originalStream.GetType());
#else
                    return Stream.Null;
                }

                throw Fx.AssertAndThrow("Does not support cloning of Stream Type: " + originalStream.GetType());
#endif
            }

            return null;
        }

#if !WINDOWS_UWP && !NETMF
        AmqpMessage PopulateAmqpMessageForSend(AmqpMessage message)
        {
            MessageConverter.UpdateAmqpMessageHeadersAndProperties(message, this);
            return message;
        }
#endif

#if NETMF
        object GetSystemProperty(string key)
        {
            // .NetMF doesn't have generics so we have to resort to look for the key and return the object inside. The caller will have to figure out how to handle it
            return this.SystemProperties[key];
        }
#else
        T GetSystemProperty<T>(string key)
        {
            if (this.SystemProperties.ContainsKey(key))
            {
                return (T)this.SystemProperties[key];
            }

            return default(T);
        }
#endif

        void ThrowIfDisposed()
        {
            if (this.disposed)
            {
#if NETMF
                throw new Exception("Message disposed");
#else
                throw Fx.Exception.ObjectDisposed(ApiResources.MessageDisposed);
#endif
            }
        }

        void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                if (disposing)
                {
#if !WINDOWS_UWP && !NETMF
                    if (this.serializedAmqpMessage != null)
                    {
                        // in the receive scenario, this.bodyStream is a reference
                        // to serializedAmqpMessage.BodyStream, and we assume disposing
                        // the amqpMessage will dispose the body stream so we don't
                        // need to dispose bodyStream twice.
                        this.serializedAmqpMessage.Dispose();
                        this.bodyStream = null;
                    }
                    else
#endif
                    if (this.bodyStream != null && this.ownsBodyStream)
                    {
                        this.bodyStream.Dispose();
                        this.bodyStream = null;
                    }
                }

                this.disposed = true;
            }
        }
    }
}

