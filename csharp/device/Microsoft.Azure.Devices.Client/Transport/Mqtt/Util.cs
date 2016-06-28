// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;
    using System.Globalization;
    using System.IO;
    using System.Threading.Tasks;
    using DotNetty.Buffers;
    using DotNetty.Codecs.Mqtt.Packets;
    using DotNetty.Transport.Channels;
    using Microsoft.Azure.Devices.Client.Common;
    using Microsoft.Azure.Devices.Client.Extensions;

    static class Util
    {
        static class PacketIdGenerator
        {
            static ushort current = (ushort)new Random((int)DateTime.UtcNow.ToFileTimeUtc()).Next(0, ushort.MaxValue);

            public static int Next()
            {
                unchecked
                {
                    return current++;
                }
            }
        }

        static class IotHubWirePropertyNames
        {
            public const string AbsoluteExpiryTime = "$.exp";
            public const string CorrelationId = "$.cid";
            public const string MessageId = "$.mid";
            public const string To = "$.to";
            public const string UserId = "$.uid";
        }

        static class MessageSystemPropertyNames
        {
            public const string MessageId = "message-id";

            public const string To = "to";

            public const string ExpiryTimeUtc = "absolute-expiry-time";

            public const string CorrelationId = "correlation-id";

            public const string UserId = "user-id";

            public const string Operation = "iothub-operation";

            public const string Ack = "iothub-ack";
        }

        static readonly Dictionary<string, string> ToSystemPropertiesMap = new Dictionary<string, string>
        {
            {IotHubWirePropertyNames.AbsoluteExpiryTime, MessageSystemPropertyNames.ExpiryTimeUtc},
            {IotHubWirePropertyNames.CorrelationId, MessageSystemPropertyNames.CorrelationId},
            {IotHubWirePropertyNames.MessageId, MessageSystemPropertyNames.MessageId},
            {IotHubWirePropertyNames.To, MessageSystemPropertyNames.To},
            {IotHubWirePropertyNames.UserId, MessageSystemPropertyNames.UserId},
            {MessageSystemPropertyNames.Operation, MessageSystemPropertyNames.Operation},
            {MessageSystemPropertyNames.Ack, MessageSystemPropertyNames.Ack}
        };

        static readonly Dictionary<string, string> FromSystemPropertiesMap = new Dictionary<string, string>
        {
            {MessageSystemPropertyNames.ExpiryTimeUtc, IotHubWirePropertyNames.AbsoluteExpiryTime},
            {MessageSystemPropertyNames.CorrelationId, IotHubWirePropertyNames.CorrelationId},
            {MessageSystemPropertyNames.MessageId, IotHubWirePropertyNames.MessageId},
            {MessageSystemPropertyNames.To, IotHubWirePropertyNames.To},
            {MessageSystemPropertyNames.UserId, IotHubWirePropertyNames.UserId},
            {MessageSystemPropertyNames.Operation, MessageSystemPropertyNames.Operation},
            {MessageSystemPropertyNames.Ack, MessageSystemPropertyNames.Ack}
        };

        const char SegmentSeparatorChar = '/';
        const char SingleSegmentWildcardChar = '+';
        const char MultiSegmentWildcardChar = '#';
        static readonly char[] WildcardChars = { MultiSegmentWildcardChar, SingleSegmentWildcardChar };
        const string IotHubTrueString = "true";
        const string SegmentSeparator = "/";
        const int MaxPayloadSize = 0x3ffff;

        public static bool CheckTopicFilterMatch(string topicName, string topicFilter)
        {
            int topicFilterIndex = 0;
            int topicNameIndex = 0;
            while (topicNameIndex < topicName.Length && topicFilterIndex < topicFilter.Length)
            {
                int wildcardIndex = topicFilter.IndexOfAny(WildcardChars, topicFilterIndex);
                if (wildcardIndex == -1)
                {
                    int matchLength = Math.Max(topicFilter.Length - topicFilterIndex, topicName.Length - topicNameIndex);
                    return string.Compare(topicFilter, topicFilterIndex, topicName, topicNameIndex, matchLength, StringComparison.Ordinal) == 0;
                }
                else
                {
                    if (topicFilter[wildcardIndex] == MultiSegmentWildcardChar)
                    {
                        if (wildcardIndex == 0) // special case -- any topic name would match
                        {
                            return true;
                        }
                        else
                        {
                            int matchLength = wildcardIndex - topicFilterIndex - 1;
                            if (string.Compare(topicFilter, topicFilterIndex, topicName, topicNameIndex, matchLength, StringComparison.Ordinal) == 0
                                && (topicName.Length == topicNameIndex + matchLength || (topicName.Length > topicNameIndex + matchLength && topicName[topicNameIndex + matchLength] == SegmentSeparatorChar)))
                            {
                                // paths match up till wildcard and either it is parent topic in hierarchy (one level above # specified) or any child topic under a matching parent topic
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }
                    else
                    {
                        // single segment wildcard
                        int matchLength = wildcardIndex - topicFilterIndex;
                        if (matchLength > 0 && string.Compare(topicFilter, topicFilterIndex, topicName, topicNameIndex, matchLength, StringComparison.Ordinal) != 0)
                        {
                            return false;
                        }
                        topicNameIndex = topicName.IndexOf(SegmentSeparatorChar, topicNameIndex + matchLength);
                        topicFilterIndex = wildcardIndex + 1;
                        if (topicNameIndex == -1)
                        {
                            // there's no more segments following matched one
                            return topicFilterIndex == topicFilter.Length;
                        }
                    }
                }
            }

            return topicFilterIndex == topicFilter.Length && topicNameIndex == topicName.Length;
        }

        public static QualityOfService DeriveQos(Message message, MqttTransportSettings config)
        {
            QualityOfService qos;
            string qosValue;
            if (message.Properties.TryGetValue(config.QoSPropertyName, out qosValue))
            {
                int qosAsInt;
                if (int.TryParse(qosValue, out qosAsInt))
                {
                    qos = (QualityOfService)qosAsInt;
                    if (qos > QualityOfService.ExactlyOnce)
                    {
                        qos = config.PublishToServerQoS;
                    }
                }
                else
                {
                    qos = config.PublishToServerQoS;
                }
            }
            else
            {
                qos = config.PublishToServerQoS;
            }
            return qos;
        }

        public static Message CompleteMessageFromPacket(Message message, PublishPacket packet, MqttTransportSettings mqttTransportSettings)
        {
            message.MessageId = Guid.NewGuid().ToString("N");
            if (packet.RetainRequested)
            {
                message.Properties[mqttTransportSettings.RetainPropertyName] = IotHubTrueString;
            }
            if (packet.Duplicate)
            {
                message.Properties[mqttTransportSettings.DupPropertyName] = IotHubTrueString;
            }

            return message;
        }

        public static async Task<PublishPacket> ComposePublishPacketAsync(IChannelHandlerContext context, Message message, QualityOfService qos, string topicName)
        {
            var packet = new PublishPacket(qos, false, false);
            packet.TopicName = PopulateMessagePropertiesFromMessage(topicName, message);
            if (qos > QualityOfService.AtMostOnce)
            {
                int packetId = GetNextPacketId();
                switch (qos)
                {
                    case QualityOfService.AtLeastOnce:
                        packetId &= 0x7FFF; // clear 15th bit
                        break;
                    case QualityOfService.ExactlyOnce:
                        packetId |= 0x8000; // set 15th bit
                        break;
                    default:
                        throw new ArgumentOutOfRangeException(nameof(qos), qos, null);
                }
                packet.PacketId = packetId;
            }
            using (Stream payloadStream = message.GetBodyStream())
            {
                long streamLength = payloadStream.Length;
                if (streamLength > MaxPayloadSize)
                {
                    throw new InvalidOperationException($"Message size ({streamLength} bytes) is too big to process. Maximum allowed payload size is {MaxPayloadSize}");
                }

                int length = (int)streamLength;
                IByteBuffer buffer = context.Channel.Allocator.Buffer(length, length);
                await buffer.WriteBytesAsync(payloadStream, length);
                Contract.Assert(buffer.ReadableBytes == length);

                packet.Payload = buffer;
            }
            return packet;
        }

        public static async Task WriteMessageAsync(IChannelHandlerContext context, object message, Func<IChannelHandlerContext, Exception, bool> exceptionHandler)
        {
            try
            {
                await context.WriteAndFlushAsync(message);
            }
            catch (Exception ex)
            {
                if (!exceptionHandler(context, ex))
                {
                    throw;
                }
            }
        }

        public static void PopulateMessagePropertiesFromPacket(Message message, PublishPacket publish)
        {
            message.LockToken = publish.QualityOfService == QualityOfService.AtLeastOnce ? publish.PacketId.ToString() : null;
            Dictionary<string, string> properties = UrlEncodedDictionarySerializer.Deserialize(publish.TopicName, publish.TopicName.NthIndexOf('/', 0, 4) + 1);
            foreach (KeyValuePair<string, string> property in properties)
            {
                string propertyName;
                if (ToSystemPropertiesMap.TryGetValue(property.Key, out propertyName))
                {
                    message.SystemProperties[propertyName] = ConvertToSystemProperty(property);
                }
                else
                {
                    message.Properties[property.Key] = property.Value;
                }
            }
        }

        static string PopulateMessagePropertiesFromMessage(string topicName, Message message)
        {
            var systemProperties = new Dictionary<string, string>();
            foreach (KeyValuePair<string, object> property in message.SystemProperties)
            {
                string propertyName;
                if (FromSystemPropertiesMap.TryGetValue(property.Key, out propertyName))
                {
                    systemProperties[propertyName] = ConvertFromSystemProperties(property.Value);
                }
            }
            string properties = UrlEncodedDictionarySerializer.Serialize(new ReadOnlyMergeDictionary<string, string>(systemProperties, message.Properties));

            return topicName.EndsWith(SegmentSeparator, StringComparison.Ordinal) ? topicName + properties + SegmentSeparator : topicName + SegmentSeparator + properties;
        }

        static string ConvertFromSystemProperties(object systemProperty)
        {
            if (systemProperty is string)
            {
                return (string)systemProperty;
            }
            if (systemProperty is DateTime)
            {
                return ((DateTime)systemProperty).ToString("o", CultureInfo.InvariantCulture);
            }
            return systemProperty?.ToString();
        }

        static object ConvertToSystemProperty(KeyValuePair<string, string> property)
        {
            if (string.IsNullOrEmpty(property.Value))
            {
                return property.Value;
            }
            if (property.Key == IotHubWirePropertyNames.AbsoluteExpiryTime)
            {
                return DateTime.ParseExact(property.Value, "o", CultureInfo.InvariantCulture);
            }
            if (property.Key == MessageSystemPropertyNames.Ack)
            {
                return (DeliveryAcknowledgement)Enum.Parse(typeof(DeliveryAcknowledgement), property.Value, true);
            }
            return property.Value;
        }

        public static int GetNextPacketId()
        {
            return PacketIdGenerator.Next();
        }
    }
}