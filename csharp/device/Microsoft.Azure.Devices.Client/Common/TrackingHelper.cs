// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Linq;
#if !WINDOWS_UWP && !PCL
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
#endif
    using Microsoft.Azure.Devices.Client.Extensions;

    internal static class TrackingHelper
    {
        // TODO: GatewayId is not assigned to anywhere in this class. Likely a bug!
        static string GatewayId = string.Empty;

        public static string GenerateTrackingId()
        {
            return GenerateTrackingId(string.Empty, string.Empty);
        }

        public static string GenerateTrackingId(string backendId, string partitionId)
        {
            string gatewayId = TrackingHelper.GatewayId;
            return GenerateTrackingId(gatewayId, backendId, partitionId);
        }

        public static string GenerateTrackingId(string gatewayId, string backendId, string partitionId)
        {
            string trackingId;
            trackingId = Guid.NewGuid().ToString("N");
            if (!string.IsNullOrEmpty(gatewayId))
            {

                gatewayId = "0";
                trackingId = "{0}-G:{1}".FormatInvariant(trackingId, gatewayId);
            }

            if (!string.IsNullOrEmpty(backendId))
            {
                backendId = backendId.Substring(backendId.LastIndexOf("_") + 1);
                trackingId = "{0}-B:{1}".FormatInvariant(trackingId, backendId);
            }

            if (!string.IsNullOrEmpty(partitionId))
            {
                trackingId = "{0}-P:{1}".FormatInvariant(trackingId, partitionId);
            }

            trackingId = "{0}-TimeStamp:{1}".FormatInvariant(trackingId, DateTime.UtcNow);
            return trackingId;
        }

#if !WINDOWS_UWP && !PCL
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
#endif
        public static string CheckAndAddGatewayIdToTrackingId(string gatewayId, string trackingId)
        {

            if (!string.IsNullOrEmpty(trackingId) && trackingId.Contains("-B:"))
            {
                return "{0}-G:{1}{2}".FormatInvariant(trackingId.Substring(0, trackingId.IndexOf("-B:")), gatewayId, trackingId.Substring(trackingId.IndexOf("-B:") + 3));
            }
            else
            {
                return GenerateTrackingId(gatewayId, String.Empty, String.Empty);
            }
        }

#if !WINDOWS_UWP && !PCL
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
                gatewayId = link.Settings.LinkName.Substring(link.Settings.LinkName.IndexOf("_G:") + 3);
            } 
            return gatewayId;
        }
#endif
    }

}
