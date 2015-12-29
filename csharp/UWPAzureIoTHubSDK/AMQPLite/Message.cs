//  ------------------------------------------------------------------------------------
//  Copyright (c) Microsoft Corporation
//  All rights reserved. 
//  
//  Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this 
//  file except in compliance with the License. You may obtain a copy of the License at 
//  http://www.apache.org/licenses/LICENSE-2.0  
//  
//  THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR 
//  CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR 
//  NON-INFRINGEMENT. 
// 
//  See the Apache Version 2.0 License for specific language governing permissions and 
//  limitations under the License.
//  ------------------------------------------------------------------------------------

namespace Amqp
{
    using Amqp.Framing;
    using Amqp.Types;
    using System;

    /// <summary>
    /// The Message class represents an AMQP message.
    /// </summary>
    public class Message
    {
        /// <summary>
        /// The header section.
        /// </summary>
        public Header Header;

        /// <summary>
        /// The delivery annotation section.
        /// </summary>
        public DeliveryAnnotations DeliveryAnnotations;

        /// <summary>
        /// The message annotation section.
        /// </summary>
        public MessageAnnotations MessageAnnotations;

        /// <summary>
        /// The properties section.
        /// </summary>
        public Properties Properties;

        /// <summary>
        /// The application properties section.
        /// </summary>
        public ApplicationProperties ApplicationProperties;

        /// <summary>
        /// The body section. The library supports one section only.
        /// </summary>
        public RestrictedDescribed BodySection;

        /// <summary>
        /// The footer section.
        /// </summary>
        public Footer Footer;

        /// <summary>
        /// Initializes an empty message.
        /// </summary>
        public Message()
        {
        }

        /// <summary>
        /// Initializes a message with an AmqpValue body. The body must be a defined AMQP type.
        /// </summary>
        /// <param name="body">the object stored in the AmqpValue section.</param>
        public Message(object body)
        {
            this.BodySection = new AmqpValue() { Value = body };
        }

        /// <summary>
        /// Gets the object from the body. The returned value depends on the type of the body section.
        /// Use the BodySection field if the entire section is needed.
        /// </summary>
        public object Body
        {
            get
            {
                if (this.BodySection == null)
                {
                    return null;
                }
                else if (this.BodySection.Descriptor.Code == Codec.AmqpValue.Code)
                {
                    return ((AmqpValue)this.BodySection).Value;
                }
                else if (this.BodySection.Descriptor.Code == Codec.Data.Code)
                {
                    return ((Data)this.BodySection).Binary;
                }
                else if (this.BodySection.Descriptor.Code == Codec.AmqpSequence.Code)
                {
                    return ((AmqpSequence)this.BodySection).List;
                }
                else
                {
                    throw new AmqpException(ErrorCode.DecodeError, "The body section is invalid.");
                }
            }
        }

        /// <summary>
        /// Gets the delivery tag associated with the message.
        /// </summary>
        public byte[] DeliveryTag
        {
            get
            {
                return this.Delivery != null ? this.Delivery.Tag : null;
            }
        }

        internal Delivery Delivery
        {
            get;
            set;
        }

        /// <summary>
        /// Encodes the message into a buffer.
        /// </summary>
        /// <returns>The buffer.</returns>
        public ByteBuffer Encode()
        {
            ByteBuffer buffer = new ByteBuffer(128, true);
            EncodeIfNotNull(this.Header, buffer);
            EncodeIfNotNull(this.DeliveryAnnotations, buffer);
            EncodeIfNotNull(this.MessageAnnotations, buffer);
            EncodeIfNotNull(this.Properties, buffer);
            EncodeIfNotNull(this.ApplicationProperties, buffer);
            EncodeIfNotNull(this.BodySection, buffer);
            EncodeIfNotNull(this.Footer, buffer);
            return buffer;
        }

        /// <summary>
        /// Decodes a message from a buffer and advance the buffer read cursor.
        /// </summary>
        /// <param name="buffer">The buffer.</param>
        /// <returns></returns>
        public static Message Decode(ByteBuffer buffer)
        {
            Message message = new Message();

            while (buffer.Length > 0)
            {
                var described = (RestrictedDescribed)Encoder.ReadObject(buffer);
                if (described.Descriptor.Code == Codec.Header.Code)
                {
                    message.Header = (Header)described;
                }
                else if (described.Descriptor.Code == Codec.DeliveryAnnotations.Code)
                {
                    message.DeliveryAnnotations = (DeliveryAnnotations)described;
                }
                else if (described.Descriptor.Code == Codec.MessageAnnotations.Code)
                {
                    message.MessageAnnotations = (MessageAnnotations)described;
                }
                else if (described.Descriptor.Code == Codec.Properties.Code)
                {
                    message.Properties = (Properties)described;
                }
                else if (described.Descriptor.Code == Codec.ApplicationProperties.Code)
                {
                    message.ApplicationProperties = (ApplicationProperties)described;
                }
                else if (described.Descriptor.Code == Codec.AmqpValue.Code ||
                    described.Descriptor.Code == Codec.Data.Code ||
                    described.Descriptor.Code == Codec.AmqpSequence.Code)
                {
                    message.BodySection = described;
                }
                else if (described.Descriptor.Code == Codec.Footer.Code)
                {
                    message.Footer = (Footer)described;
                }
                else
                {
                    throw new AmqpException(ErrorCode.FramingError,
                        Fx.Format(SRAmqp.AmqpUnknownDescriptor, described.Descriptor));
                }
            }

            return message;
        }

        static void EncodeIfNotNull(RestrictedDescribed section, ByteBuffer buffer)
        {
            if (section != null)
            {
                section.Encode(buffer);
            }
        }
    }
}