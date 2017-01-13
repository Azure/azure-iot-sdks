// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;

    static class CommonConstants
    {
        // TODO: move these to ConfigProvider
        public const string DeviceAudienceFormat = "{0}/devices/{1}";
        public const string MediaTypeForDeviceManagementApis = "application/json";
        public const string AmqpsScheme = "amqps";
        public const string AmqpScheme = "amqp";
        public const string AmqpDnsWSPrefix = "amqpws";
        
        // IotHub WindowsFabric Constants
        public const int WindowsFabricRetryLimit = 20;
        public const int WindowsFabricRetryWaitInMilliseconds = 3000;
        public const string IotHubApplicationName = "fabric:/microsoft.azure.devices.container";
        public const string IotHubApplicationTypeName = "microsoft.azure.devices.container";
        public const string IotHubServiceTypeName = "microsoft.azure.devices.container.service";
        public const string IotHubMetadataParentName = "iothub-metadata";

        public const string MicrosoftOwinContextPropertyName = "MS_OwinContext";

        // EventHub
        public const int EventHubEndpointPortNumber = 5671;
        public const string EventHubConnectionStringTemplate = "{0};PartitionCount={1}";

        // Namespace paths
        public const string ResourceProviderNamespace = "Microsoft.Devices";
        public const string ResourceProviderServiceResourceType = ResourceProviderNamespace + "/IotHubs";
        public const string ResourceProviderBasePathTemplate = "/subscriptions/{0}/resourceGroups/{1}/providers/" + ResourceProviderServiceResourceType + "/{2}";

        // Runtime Retry Constants
        public const int RuntimeRetryLimit = 3;
        public const int RuntimeRetryWaitInMilliseconds = 5000;

        // Device URI Templates
        public const string DeviceEventPathTemplate = "/devices/{0}/messages/events";
        public const string DeviceBoundPathTemplate = "/devices/{0}/messages/deviceBound";
        public const string BlobUploadStatusPathTemplate = "/devices/{0}/files/";
        public const string BlobUploadPathTemplate = "/devices/{0}/files";
        public const string DeviceBoundPathCompleteTemplate = DeviceBoundPathTemplate + "/{1}";
        public const string DeviceBoundPathAbandonTemplate = DeviceBoundPathCompleteTemplate + "/abandon";
        public const string DeviceBoundPathRejectTemplate = DeviceBoundPathCompleteTemplate + "?reject";

        // IotHub provisioning terminal states (CSM/ARM)
        public const string ProvisioningStateSucceed = "Succeeded";
        public const string ProvisioningStateFailed = "Failed";
        public const string ProvisioningStateCanceled = "Canceled";

        public const string DeviceToCloudOperation = "d2c";
        public const string CloudToDeviceOperation = "c2d";

        public const string ApiVersionQueryParameterName = "api-version";

        // Service configurable parameters
        public const string PartitionCount = "PartitionCount";
        public const string TargetReplicaSetSize = "TargetReplicaSetSize";
        public const string MinReplicaSetSize = "MinReplicaSetSize";

        public const string ContentTypeHeaderName = "Content-Type";
        public const string ContentEncodingHeaderName = "Content-Encoding";
        public const string BatchedMessageContentType = "application/vnd.microsoft.iothub.json";

        public const string IotHubServiceNamePrefix = "iothub.";
        public const string IotHubSystemStoreServiceName = "iothub-systemstore";
        public const string AdminUriFormat = "/$admin/{0}?{1}";
        public const string DefaultConfigurationKey = "_default_config_key";

        public static class CloudToDevice
        {
            public const int DefaultMaxDeliveryCount = 10;
            public const int MaximumMaxDeliveryCount = 100;
            public const int MinimumMaxDeliveryCount = 1;

#if NETMF
            public static readonly TimeSpan DefaultTtl = new TimeSpan(1, 0, 0);
            public static readonly TimeSpan MaximumDefaultTtl = new TimeSpan(2, 0, 0, 0);
            public static readonly TimeSpan MinimumDefaultTtl = new TimeSpan(0, 1, 0);
#else
            public static readonly TimeSpan DefaultTtl = TimeSpan.FromHours(1);
            public static readonly TimeSpan MaximumDefaultTtl = TimeSpan.FromDays(2);
            public static readonly TimeSpan MinimumDefaultTtl = TimeSpan.FromMinutes(1);
#endif
        }

        public static class Feedback
        {
#if NETMF
            public static readonly TimeSpan DefaultLockDuration = new TimeSpan(0, 1, 0);
            public static readonly TimeSpan MaximumLockDuration = new TimeSpan(0, 5, 0);
            public static readonly TimeSpan MinimumLockDuration = new TimeSpan(0, 0, 5);

            public static readonly TimeSpan DefaultTtl = new TimeSpan(1, 0, 0);
            public static readonly TimeSpan MaximumTtl = new TimeSpan(48, 0, 0);
            public static readonly TimeSpan MinimumTtl = new TimeSpan(0, 1, 0);
#else
            public static readonly TimeSpan DefaultLockDuration = TimeSpan.FromMinutes(1);
            public static readonly TimeSpan MaximumLockDuration = TimeSpan.FromMinutes(5);
            public static readonly TimeSpan MinimumLockDuration = TimeSpan.FromSeconds(5);

            public static readonly TimeSpan DefaultTtl = TimeSpan.FromHours(1);
            public static readonly TimeSpan MaximumTtl = TimeSpan.FromHours(48);
            public static readonly TimeSpan MinimumTtl = TimeSpan.FromMinutes(1);
#endif

            public const int DefaultMaxDeliveryCount = 10;
            public const int MaximumMaxDeliveryCount = 100;
            public const int MinimumMaxDeliveryCount = 1;

            public const int QueuePartitionSizeInMegabytes = 5120;
        }
    }
}
