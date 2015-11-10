// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Linq;

    using Microsoft.Azure.Amqp;    
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Devices.Common.Client;
    using Microsoft.Azure.Devices.Common.Exceptions;

    //TrackingId format is Guid[-G:<GatewayId>][-B:<BackendId>][-P:<PartitionId>]-TimeStamp:<Timestamp>

    public static class TrackingHelper
    {
        public static string GatewayId;
        const string GatewayPrefix = "-G:";
        const string BackendPrefix = "-B:";        
        const string PartitionPrefix = "-P:";
        const string TimeStampPrefix = "-TimeStamp:";

        public static string GenerateTrackingId()
        {
            return GenerateTrackingId(String.Empty, String.Empty);
        }

        public static string GenerateTrackingId(string backendId, string partitionId)
        {
            string gatewayId = TrackingHelper.GatewayId;
            return GenerateTrackingId(gatewayId, backendId, partitionId);
        }

        public static string GenerateTrackingId(string gatewayId, string backendId, string partitionId)
        {
            string trackingId = Guid.NewGuid().ToString("N");

            if (!string.IsNullOrEmpty(gatewayId))
            {
                if (gatewayId.Contains('.'))
                {
                    gatewayId = gatewayId.Substring(gatewayId.LastIndexOf(".", StringComparison.OrdinalIgnoreCase) + 1);
                }
                else
                {
                    gatewayId = "0";
                }

                trackingId = "{0}{1}{2}".FormatInvariant(trackingId, GatewayPrefix, gatewayId);
            }

            if (!string.IsNullOrEmpty(backendId))
            {
                if (backendId.Contains("."))
                {
                    backendId = backendId.Substring(backendId.LastIndexOf(".", StringComparison.OrdinalIgnoreCase) + 1);
                }

                trackingId = "{0}{1}{2}".FormatInvariant(trackingId, BackendPrefix, backendId);
            }

            if (!string.IsNullOrEmpty(partitionId))
            {
                trackingId = "{0}{1}{2}".FormatInvariant(trackingId, PartitionPrefix, partitionId);
            }

            trackingId = "{0}{1}{2}".FormatInvariant(trackingId, TimeStampPrefix, DateTime.UtcNow);
            return trackingId;
        }

        public static string GenerateTrackingId(this AmqpException exception)
        {
            return exception.GenerateTrackingId(TrackingHelper.GatewayId, string.Empty, string.Empty);
        }

        public static string GenerateTrackingId(this AmqpException exception, string backendId, string partitionId)
        {
            return exception.GenerateTrackingId(TrackingHelper.GatewayId, backendId, partitionId);
        }

        public static string GenerateTrackingId(this AmqpException exception,string gatewayId, string backendId, string partitionId)
        {
            if (exception.Error.Info == null)
            {
                exception.Error.Info = new Fields();
            }

            string trackingId;
            if (!exception.Error.Info.Any() || !exception.Error.Info.TryGetValue(IotHubAmqpProperty.TrackingId, out trackingId))
            {
                trackingId = GenerateTrackingId(gatewayId, backendId, partitionId);
                exception.Error.Info.Add(IotHubAmqpProperty.TrackingId, trackingId);
            }
            return trackingId;
        }

        public static void SetErrorCode(this AmqpException exception)
        {
            if (exception.Error.Info == null)
            {
                exception.Error.Info = new Fields();
            }

            ErrorCode errorCode;
            if (!exception.Error.Info.TryGetValue(IotHubAmqpProperty.TrackingId, out errorCode))
            {
                exception.Error.Info.Add(IotHubAmqpProperty.ErrorCode, GetErrorCodeFromAmqpError(exception.Error));
            }
        }

        public static string CheckAndAddGatewayIdToTrackingId(string trackingId)
        {

            if (!String.IsNullOrEmpty(trackingId) && !trackingId.Contains(GatewayPrefix) && trackingId.Contains(BackendPrefix) && TrackingHelper.GatewayId != null)
            {
                var indexOfBackend = trackingId.IndexOf(BackendPrefix, StringComparison.Ordinal);
                return "{0}{3}{1}{2}".FormatInvariant(trackingId.Substring(0, indexOfBackend), TrackingHelper.GatewayId, trackingId.Substring(indexOfBackend), GatewayPrefix);
            }
            else
            {
                return GenerateTrackingId(TrackingHelper.GatewayId, String.Empty, String.Empty);
            }

        }

        public static string GetTrackingId(this AmqpException amqpException)
        {
            Error errorObj = amqpException.Error;
            string trackingId = null;
            if (errorObj.Info != null)
            {
                errorObj.Info.TryGetValue(IotHubAmqpProperty.TrackingId, out trackingId);
            }
            return trackingId;
        }

        public static string GetGatewayId(this AmqpLink link)
        {
            string gatewayId = null;
            if(link.Settings.LinkName.Contains("_G:"))
            {
                gatewayId = link.Settings.LinkName.Substring(link.Settings.LinkName.IndexOf("_G:", StringComparison.Ordinal) + 3);
            } 
            return gatewayId;
        }

        public static ErrorCode GetErrorCodeFromAmqpError(Error ex)
        {
            if (ex.Condition.Equals(AmqpErrorCode.NotFound))
            {
                return ErrorCode.DeviceNotFound;
            }
            if (ex.Condition.Equals(IotHubAmqpErrorCode.MessageLockLostError))
            {
                return ErrorCode.DeviceMessageLockLost;
            }
            if (ex.Condition.Equals(IotHubAmqpErrorCode.IotHubSuspended))
            {
                return ErrorCode.IotHubSuspended;
            }
            if (ex.Condition.Equals(IotHubAmqpErrorCode.IotHubNotFoundError))
            {
                return ErrorCode.IotHubNotFound;
            }
            if (ex.Condition.Equals(IotHubAmqpErrorCode.PreconditionFailed))
            {
                return ErrorCode.PreconditionFailed;
            }
            if (ex.Condition.Equals(AmqpErrorCode.MessageSizeExceeded))
            {
                return ErrorCode.MessageTooLarge;
            }
            if (ex.Condition.Equals(AmqpErrorCode.ResourceLimitExceeded))
            {
                return ErrorCode.DeviceMaximumQueueDepthExceeded;
            }
            if (ex.Condition.Equals(AmqpErrorCode.UnauthorizedAccess))
            {
                return ErrorCode.IotHubUnauthorizedAccess;
            }

            return ErrorCode.InvalidErrorCode;
        }
    }
}
