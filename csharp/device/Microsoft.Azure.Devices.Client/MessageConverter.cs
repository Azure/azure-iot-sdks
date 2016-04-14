// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.IO;
    using System.Runtime.Serialization;
    using System.Text;

#if !PCL

    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Encoding;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Devices.Client.Common.Api;

    static class MessageConverter
    {
        public const string LockTokenName = "x-opt-lock-token";
        public const string SequenceNumberName = "x-opt-sequence-number";
        public const string TimeSpanName = AmqpConstants.Vendor + ":timespan";
        public const string UriName = AmqpConstants.Vendor + ":uri";
        public const string DateTimeOffsetName = AmqpConstants.Vendor + ":datetime-offset";

        /// <summary>
        /// Copies the properties from the amqp message to the Message instance.
        /// </summary>
        public static void UpdateMessageHeaderAndProperties(AmqpMessage amqpMessage, Message data)
        {
            Fx.AssertAndThrow(amqpMessage.DeliveryTag != null, "AmqpMessage should always contain delivery tag.");
            data.DeliveryTag = amqpMessage.DeliveryTag;

            SectionFlag sections = amqpMessage.Sections;
            if ((sections & SectionFlag.Properties) != 0)
            {
                // Extract only the Properties that we support
                data.MessageId = amqpMessage.Properties.MessageId != null ? amqpMessage.Properties.MessageId.ToString() : null;
                data.To = amqpMessage.Properties.To != null ? amqpMessage.Properties.To.ToString() : null;

                if (amqpMessage.Properties.AbsoluteExpiryTime.HasValue)
                {
                    data.ExpiryTimeUtc = amqpMessage.Properties.AbsoluteExpiryTime.Value;
                }

                data.CorrelationId = amqpMessage.Properties.CorrelationId != null ? amqpMessage.Properties.CorrelationId.ToString() : null;
                data.UserId = amqpMessage.Properties.UserId.Array != null ? Encoding.UTF8.GetString(amqpMessage.Properties.UserId.Array) : null;
            }

            if ((sections & SectionFlag.MessageAnnotations) != 0)
            {
                string lockToken;
                if (amqpMessage.MessageAnnotations.Map.TryGetValue(LockTokenName, out lockToken))
                {
                    data.LockToken = lockToken;
                }

                ulong sequenceNumber;
                if (amqpMessage.MessageAnnotations.Map.TryGetValue(SequenceNumberName, out sequenceNumber))
                {
                    data.SequenceNumber = sequenceNumber;
                }

                DateTime enqueuedTime;
                if (amqpMessage.MessageAnnotations.Map.TryGetValue(MessageSystemPropertyNames.EnqueuedTime, out enqueuedTime))
                {
                    data.EnqueuedTimeUtc = enqueuedTime;
                }

                byte deliveryCount;
                if (amqpMessage.MessageAnnotations.Map.TryGetValue(MessageSystemPropertyNames.DeliveryCount, out deliveryCount))
                {
                    data.DeliveryCount = deliveryCount;
                }
            }

            if ((sections & SectionFlag.ApplicationProperties) != 0)
            {
                foreach (KeyValuePair<MapKey, object> pair in amqpMessage.ApplicationProperties.Map)
                {
                    object netObject = null;
                    if (TryGetNetObjectFromAmqpObject(pair.Value, MappingType.ApplicationProperty, out netObject))
                    {
                        var stringObject = netObject as string;
                        
                        if (stringObject != null)
                        {
                            switch (pair.Key.ToString())
                            {
                                case MessageSystemPropertyNames.Operation:
                                    data.SystemProperties[pair.Key.ToString()] = stringObject;
                                    break;
                                default:
                                    data.Properties[pair.Key.ToString()] = stringObject;
                                    break;
                            }
                        }
                        else
                        {
                            // TODO: RDBug 4093369 Handling of non-string property values in Amqp messages
                            // Drop non-string properties and log an error
                            Fx.Exception.TraceHandled(new InvalidDataException("IotHub does not accept non-string Amqp properties"), "MessageConverter.UpdateMessageHeaderAndProperties");
                        }                        
                    }
                }
            }
        }

        /// <summary>
        /// Copies the Message instance's properties to the AmqpMessage instance.
        /// </summary>
        public static void UpdateAmqpMessageHeadersAndProperties(AmqpMessage amqpMessage, Message data, bool copyUserProperties = true)
        {
            amqpMessage.Properties.MessageId = data.MessageId;

            if (data.To != null)
            {
                amqpMessage.Properties.To = data.To;
            }


#if WINDOWS_UWP
            if (!data.ExpiryTimeUtc.Equals(default(DateTimeOffset)))
            {
                amqpMessage.Properties.AbsoluteExpiryTime = data.ExpiryTimeUtc.DateTime;
            }
#else
            if (!data.ExpiryTimeUtc.Equals(default(DateTime)))
            {
                amqpMessage.Properties.AbsoluteExpiryTime = data.ExpiryTimeUtc;
            }
#endif
            if (data.CorrelationId != null)
            {
                amqpMessage.Properties.CorrelationId = data.CorrelationId;
            }

            if (data.UserId != null)
            {
                amqpMessage.Properties.UserId = new ArraySegment<byte>(Encoding.UTF8.GetBytes(data.UserId));
            }

            if (amqpMessage.ApplicationProperties == null)
            {
                amqpMessage.ApplicationProperties = new ApplicationProperties();
            }

            object deliveryAckSetting;
            if (data.SystemProperties.TryGetValue(MessageSystemPropertyNames.Ack, out deliveryAckSetting))
            {
                amqpMessage.ApplicationProperties.Map["iothub-ack"] = (string)deliveryAckSetting;
            }

            if (copyUserProperties && data.Properties.Count > 0)
            {
                foreach (var pair in data.Properties)
                {
                    object amqpObject;
                    if (TryGetAmqpObjectFromNetObject(pair.Value, MappingType.ApplicationProperty, out amqpObject))
                    {
                        amqpMessage.ApplicationProperties.Map[pair.Key] = amqpObject;
                    }
                }
            }
        }
        
        public static bool TryGetAmqpObjectFromNetObject(object netObject, MappingType mappingType, out object amqpObject)
        {
            amqpObject = null;
            if (netObject == null)
            {
                return false;
            }

            switch (SerializationUtilities.GetTypeId(netObject))
            {
                case PropertyValueType.Byte:
                case PropertyValueType.SByte:
                case PropertyValueType.Int16:
                case PropertyValueType.Int32:
                case PropertyValueType.Int64:
                case PropertyValueType.UInt16:
                case PropertyValueType.UInt32:
                case PropertyValueType.UInt64:
                case PropertyValueType.Single:
                case PropertyValueType.Double:
                case PropertyValueType.Boolean:
                case PropertyValueType.Decimal:
                case PropertyValueType.Char:
                case PropertyValueType.Guid:
                case PropertyValueType.DateTime:
                case PropertyValueType.String:
                    amqpObject = netObject;
                    break;
                case PropertyValueType.Stream:
                    if (mappingType == MappingType.ApplicationProperty)
                    {
                        amqpObject = ReadStream((Stream)netObject);
                    }
                    break;
                case PropertyValueType.Uri:
                    amqpObject = new DescribedType((AmqpSymbol)UriName, ((Uri)netObject).AbsoluteUri);
                    break;
                case PropertyValueType.DateTimeOffset:
                    amqpObject = new DescribedType((AmqpSymbol)DateTimeOffsetName, ((DateTimeOffset)netObject).UtcTicks);
                    break;
                case PropertyValueType.TimeSpan:
                    amqpObject = new DescribedType((AmqpSymbol)TimeSpanName, ((TimeSpan)netObject).Ticks);
                    break;
                case PropertyValueType.Unknown:
                    if (netObject is Stream)
                    {
                        if (mappingType == MappingType.ApplicationProperty)
                        {
                            amqpObject = ReadStream((Stream)netObject);
                        }
                    }
                    else if (mappingType == MappingType.ApplicationProperty)
                    {
                        throw FxTrace.Exception.AsError(new SerializationException(IotHubApiResources.GetString(ApiResources.FailedToSerializeUnsupportedType, netObject.GetType().FullName)));
                    }
                    else if (netObject is byte[])
                    {
                        amqpObject = new ArraySegment<byte>((byte[])netObject);
                    }
                    else if (netObject is IList)
                    {
                        // Array is also an IList
                        amqpObject = netObject;
                    }
                    else if (netObject is IDictionary)
                    {
                        amqpObject = new AmqpMap((IDictionary)netObject);
                    }
                    break;
                default:
                    break;
            }

            return amqpObject != null;
        }

        public static bool TryGetNetObjectFromAmqpObject(object amqpObject, MappingType mappingType, out object netObject)
        {
            netObject = null;
            if (amqpObject == null)
            {
                return false;
            }

            switch (SerializationUtilities.GetTypeId(amqpObject))
            {
                case PropertyValueType.Byte:
                case PropertyValueType.SByte:
                case PropertyValueType.Int16:
                case PropertyValueType.Int32:
                case PropertyValueType.Int64:
                case PropertyValueType.UInt16:
                case PropertyValueType.UInt32:
                case PropertyValueType.UInt64:
                case PropertyValueType.Single:
                case PropertyValueType.Double:
                case PropertyValueType.Boolean:
                case PropertyValueType.Decimal:
                case PropertyValueType.Char:
                case PropertyValueType.Guid:
                case PropertyValueType.DateTime:
                case PropertyValueType.String:
                    netObject = amqpObject;
                    break;
                case PropertyValueType.Unknown:
                    if (amqpObject is AmqpSymbol)
                    {
                        netObject = ((AmqpSymbol)amqpObject).Value;
                    }
                    else if (amqpObject is ArraySegment<byte>)
                    {
                        ArraySegment<byte> binValue = (ArraySegment<byte>)amqpObject;
                        if (binValue.Count == binValue.Array.Length)
                        {
                            netObject = binValue.Array;
                        }
                        else
                        {
                            byte[] buffer = new byte[binValue.Count];
                            Buffer.BlockCopy(binValue.Array, binValue.Offset, buffer, 0, binValue.Count);
                            netObject = buffer;
                        }
                    }
                    else if (amqpObject is DescribedType)
                    {
                        DescribedType describedType = (DescribedType)amqpObject;
                        if (describedType.Descriptor is AmqpSymbol)
                        {
                            AmqpSymbol symbol = (AmqpSymbol)describedType.Descriptor;
                            if (symbol.Equals((AmqpSymbol)UriName))
                            {
                                netObject = new Uri((string)describedType.Value);
                            }
                            else if (symbol.Equals((AmqpSymbol)TimeSpanName))
                            {
                                netObject = new TimeSpan((long)describedType.Value);
                            }
                            else if (symbol.Equals((AmqpSymbol)DateTimeOffsetName))
                            {
                                netObject = new DateTimeOffset(new DateTime((long)describedType.Value, DateTimeKind.Utc));
                            }
                        }
                    }
                    else if (mappingType == MappingType.ApplicationProperty)
                    {
                        throw FxTrace.Exception.AsError(new SerializationException(IotHubApiResources.GetString(ApiResources.FailedToSerializeUnsupportedType, amqpObject.GetType().FullName)));
                    }
                    else if (amqpObject is AmqpMap)
                    {
                        AmqpMap map = (AmqpMap)amqpObject;
                        Dictionary<string, object> dictionary = new Dictionary<string, object>();
                        foreach (var pair in map)
                        {
                            dictionary.Add(pair.Key.ToString(), pair.Value);
                        }

                        netObject = dictionary;
                    }
                    else
                    {
                        netObject = amqpObject;
                    }
                    break;
                default:
                    break;
            }

            return netObject != null;
        }

        public static ArraySegment<byte> ReadStream(Stream stream)
        {
            MemoryStream memoryStream = new MemoryStream();
            int bytesRead;
            byte[] readBuffer = new byte[512];
            while ((bytesRead = stream.Read(readBuffer, 0, readBuffer.Length)) > 0)
            {
                memoryStream.Write(readBuffer, 0, bytesRead);
            }

#if WINDOWS_UWP
            // UWP doesn't have GetBuffer. ToArray creates a copy -- make sure perf impact is acceptable
            return new ArraySegment<byte>(memoryStream.ToArray(), 0, (int)memoryStream.Length);
#else
            return new ArraySegment<byte>(memoryStream.GetBuffer(), 0, (int)memoryStream.Length);
#endif
        }
    }
#endif
        }
