// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;
    using System;
    using System.Collections;
    using System.IO;
    using System.Threading;
    using DateTimeT = System.DateTime;

    /// <summary>
    /// The data structure represent the message that is used for interacting with IotHub.
    /// </summary>
    public sealed class Message : IDisposable
    {
        object messageLock = new object();
        Stream bodyStream;
        bool disposed;
        bool ownsBodyStream;
        int getBodyCalled;
        //AmqpMessage serializedAmqpMessage;

        /// <summary>
        /// Default constructor with no body data
        /// </summary>
        public Message()
        {
            this.Properties = new Hashtable();
            this.SystemProperties = new Hashtable();
            //this.InitializeWithStream(Stream.Null, true);
            //this.serializedAmqpMessage = null;
        }

        /// <summary>
        /// Constructor which uses the argument stream as the body stream.
        /// </summary>
        /// <param name="stream">a stream which will be used as body stream.</param>
        /// <remarks>User is expected to own the disposing of the stream when using this constructor.</remarks>
        // UWP cannot expose a method with System.IO.Stream in signature. TODO: consider adding an IRandomAccessStream overload

        public Message(Stream stream)
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
        public Message(byte[] byteArray)
            : this(new MemoryStream(byteArray))
        {
            // reset the owning of the steams
            this.ownsBodyStream = true;
        }

        ///// <summary>
        ///// This constructor is only used in the receive path from Amqp path, 
        ///// or in Cloning from a Message that has serialized.
        ///// </summary>
        ///// <param name="amqpMessage"></param>
        //internal Message(AmqpMessage amqpMessage)
        //    : this()
        //{
        //    if (amqpMessage == null)
        //    {
        //        throw Fx.Exception.ArgumentNull("amqpMessage");
        //    }

        //    MessageConverter.UpdateMessageHeaderAndProperties(amqpMessage, this);
        //    Stream stream = amqpMessage.BodyStream;
        //    this.InitializeWithStream(stream, true);
        //}

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
                return (string)(this.GetSystemProperty(MessageSystemPropertyNames.MessageId) ?? string.Empty);
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
                return (string)(this.GetSystemProperty(MessageSystemPropertyNames.To) ?? string.Empty);
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
                return (DateTime)(this.GetSystemProperty(MessageSystemPropertyNames.ExpiryTimeUtc) ?? DateTime.MinValue);
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
                return (string)(this.GetSystemProperty(MessageSystemPropertyNames.CorrelationId) ?? string.Empty);
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
                string deliveryAckAsString = (string)(this.GetSystemProperty(MessageSystemPropertyNames.Ack) ?? string.Empty);

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
                return (ulong)(this.GetSystemProperty(MessageSystemPropertyNames.SequenceNumber) ?? 0);
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
                return (string)(this.GetSystemProperty(MessageSystemPropertyNames.LockToken) ?? string.Empty);
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
                return (DateTime)(this.GetSystemProperty(MessageSystemPropertyNames.EnqueuedTime) ?? DateTime.MinValue);
            }

            internal set
            {
                this.SystemProperties[MessageSystemPropertyNames.EnqueuedTime] = value;
            }
        }

        /// <summary>
        /// Number of times the message was delivered
        /// </summary>
        public uint DeliveryCount
        {
            get
            {
                return (byte)(this.GetSystemProperty(MessageSystemPropertyNames.DeliveryCount) ?? 0);
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
                return (string)(this.GetSystemProperty(MessageSystemPropertyNames.UserId) ?? string.Empty);
            }

            set
            {
                this.SystemProperties[MessageSystemPropertyNames.UserId] = value;
            }
        }

        /// <summary>
        /// Gets the dictionary of user properties which are set when user send the data.
        /// </summary>
        public Hashtable Properties { get; private set; }

        /// <summary>
        /// Gets the dictionary of system properties which are managed internally.
        /// </summary>
        internal Hashtable SystemProperties { get; private set; }

        //bool IReadOnlyIndicator.IsReadOnly
        //{
        //    get
        //    {
        //        return Interlocked.Read(ref this.sizeInBytesCalled) == 1;
        //    }
        //}

        public Stream BodyStream
        {
            get
            {
                return this.bodyStream;
            }
        }

        //internal AmqpMessage SerializedAmqpMessage
        //{
        //    get
        //    {
        //        lock (this.messageLock)
        //        {
        //            return this.serializedAmqpMessage;
        //        }
        //    }
        //}

        /// <summary>
        /// Gets or sets the deliveryTag which is used for server side checkpointing.
        /// </summary>
        //internal ArraySegment<byte> DeliveryTag { get; set; }

        /// <summary>
        /// Makes a clone of the current event data instance.
        /// </summary>
        /// <returns></returns>
        /// <exception cref="ObjectDisposedException">throws if the event data has already been disposed.</exception>
        public Message Clone()
        {
            this.ThrowIfDisposed();
            //if (this.serializedAmqpMessage != null)
            //{
            //    return new Message(this.serializedAmqpMessage);
            //}

            var message = new Message();
            if (this.bodyStream != null)
            {
                // The new Message always owns the cloned stream.
                message = new Message(CloneStream(this.bodyStream))
                {
                    ownsBodyStream = true
                };
            }

            foreach (var systemProperty in this.SystemProperties.Keys)
            {
                message.SystemProperties[systemProperty] = this.SystemProperties[systemProperty];
            }

            foreach (var property in this.Properties.Keys)
            {
                message.Properties.Add(property, this.Properties[property]);
            }

            return message;
        }

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

            return null;
        }

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

            //BufferListStream listStream;
            //if ((listStream = this.bodyStream as BufferListStream) != null)
            //{
            //    // We can trust Amqp bufferListStream.Length;
            //    byte[] bytes = new byte[listStream.Length];
            //    listStream.Read(bytes, 0, bytes.Length);
            //    return bytes;
            //}

            // This is just fail safe code in case we are not using the Amqp protocol.
            return ReadFullStream(this.bodyStream);
        }

        //internal AmqpMessage ToAmqpMessage(bool setBodyCalled = true)
        //{
        //    this.ThrowIfDisposed();
        //    if (this.serializedAmqpMessage == null)
        //    {
        //        lock (this.messageLock)
        //        {
        //            if (this.serializedAmqpMessage == null)
        //            {
        //                // Interlocked exchange two variable does allow for a small period 
        //                // where one is set while the other is not. Not sure if it is worth
        //                // correct this gap. The intention of setting this two variable is
        //                // so that GetBody should not be called and all Properties are
        //                // readonly because the amqpMessage has been serialized.
        //                if (setBodyCalled)
        //                {
        //                    this.SetGetBodyCalled();
        //                }

        //                this.SetSizeInBytesCalled();
        //                this.serializedAmqpMessage = this.bodyStream == null
        //                    ? AmqpMessage.Create()
        //                    : AmqpMessage.Create(this.bodyStream, false);
        //                this.serializedAmqpMessage = this.PopulateAmqpMessageForSend(this.serializedAmqpMessage);
        //            }
        //        }
        //    }

        //    return this.serializedAmqpMessage;
        //}

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
                //throw Fx.Exception.AsError(new InvalidOperationException(ApiResources.MessageBodyConsumed));
            }
        }

        void SetSizeInBytesCalled()
        {
            //Interlocked.Exchange(ref this.sizeInBytesCalled, 1);
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
            inputStream.Position = 0;
            byte[] buffer = new byte[inputStream.Length];

            inputStream.Read(buffer, 0, (int)inputStream.Length);

            return buffer;
        }

        static Stream CloneStream(Stream originalStream)
        {
            if (originalStream != null)
            {
                MemoryStream memoryStream;
                if ((memoryStream = originalStream as MemoryStream) != null)
                {
                    // Note: memoryStream.GetBuffer() doesn't work
                    //return new MemoryStream(memoryStream.ToArray(), 0, (int)memoryStream.Length, false, true);
                }

                ICloneable cloneable;

                if ((cloneable = originalStream as ICloneable) != null)
                {
                    return (Stream)cloneable.Clone();
                }

                if (originalStream.Length == 0)
                {
                    // This can happen in Stream.Null
                    return null;
                }

                //throw Fx.AssertAndThrow("Does not support cloning of Stream Type: " + originalStream.GetType());
            }

            return null;
        }


        //AmqpMessage PopulateAmqpMessageForSend(AmqpMessage message)
        //{
        //    MessageConverter.UpdateAmqpMessageHeadersAndProperties(message, this);
        //    return message;
        //}

        object GetSystemProperty(string key)
        {
            // .NetMF doesn't have generics so we can only look for the key and return the object inside. The caller will have to figure out how to handle it
            return this.SystemProperties[key];
        }

        void ThrowIfDisposed()
        {
            if (this.disposed)
            {
                //throw Fx.Exception.ObjectDisposed(ApiResources.MessageDisposed);
            }
        }

        void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                if (disposing)
                {
                    //if (this.serializedAmqpMessage != null)
                    //{
                    //    // in the receive scenario, this.bodyStream is a reference
                    //    // to serializedAmqpMessage.BodyStream, and we assume disposing
                    //    // the amqpMessage will dispose the body stream so we don't
                    //    // need to dispose bodyStream twice.
                    //    this.serializedAmqpMessage.Dispose();
                    //    this.bodyStream = null;
                    //}
                    //else
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
