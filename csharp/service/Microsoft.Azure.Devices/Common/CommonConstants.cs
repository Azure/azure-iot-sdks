// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
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
        public const ulong AmqpMaxMessageSize = 256 * 1024 * 4; // Temporarily accept messages upto 1Mb in size. Reduce to 256 kb after fixing client behavior

        public const int HttpsPort = 443;
        public const int AmqpsPort = 5671;

        // IotHub WindowsFabric Constants
        public const int WindowsFabricRetryLimit = 20;
        public const int WindowsFabricRetryWaitInMilliseconds = 3000;
        public const int WindowsFabricClientConnectionPort = 19000;

        // AzureStorage Constants
        public const int AzureStorageRetryLimit = 3;
        public const int AzureStorageRetryWaitInMilliseconds = 3000;

        public const string IotHubApplicationName = "fabric:/microsoft.azure.devices.container";
        public const string IotHubApplicationTypeName = "microsoft.azure.devices.container";
        public const string IotHubServiceTypeName = "microsoft.azure.devices.container.service";

        public const string ThrottlingApplicationName = "fabric:/microsoft.azure.devices.throttling";
        public const string ThrottlingApplicationTypeName = "microsoft.azure.devices.throttling";
        public const string ThrottlingServiceTypeName = "microsoft.azure.devices.throttling.service";

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
        public const string DeviceBoundPathCompleteTemplate = DeviceBoundPathTemplate + "/{1}";
        public const string DeviceBoundPathAbandonTemplate = DeviceBoundPathCompleteTemplate + "/abandon";
        public const string DeviceBoundPathRejectTemplate = DeviceBoundPathCompleteTemplate + "?reject";

        // IotHub constants
        public const int IotHubDefaultMaxAuthorizationRules = 16;

        // IotHub provisioning terminal states (CSM/ARM)
        public const string ProvisioningStateSucceed = "Succeeded";
        public const string ProvisioningStateFailed = "Failed";
        public const string ProvisioningStateCanceled = "Canceled";

        // IotHub provisioning initial state
        public const string ProvisioingStateAccepted = "Accepted";

        public const string DeviceToCloudOperation = "d2c";
        public const string CloudToDeviceOperation = "c2d";

        public const string ApiVersionQueryParameterName = "api-version";
        public const string WildCardEtag = "*";

        // Service configurable parameters
        public const string PartitionCount = "PartitionCount";
        public const string TargetReplicaSetSize = "TargetReplicaSetSize";
        public const string MinReplicaSetSize = "MinReplicaSetSize";
        public const string SkuMaxUnitOverride = "SkuMaxUnitOverride";

        public const string ContentTypeHeaderName = "Content-Type";
        public const string ContentEncodingHeaderName = "Content-Encoding";
        public const string BatchedMessageContentType = "application/vnd.microsoft.iothub.json";
        public const string BatchedFeedbackContentType = "application/vnd.microsoft.iothub.feedback.json";
        public const string FileNotificationContentType = "application/vnd.microsoft.iothub.filenotification.json";

        public const string IotHubSystemStoreServiceName = "iothub-systemstore";
        public const string IotHubStatsCacheKey = "_cache_key_for_iothub_stats";
        public const string SystemStoreStatsPartitionKey = "__system_store_iothub_stats_partition_key__";
        public const string SystemStoreScaleUnitConfigPartitionKey = "__system_store_scaleunit_config_partition_key__";
        public const string SystemStoreScaleUnitConfigRowKey = "__system_store_scaleunit_config_row_key__";
        public const string ScaleUnitSettingsKey = "_cache_key_for_scaleunit_settings";
        public const int SystemStoreMaxRetries = 5;
        public const string ConnectedDeviceCountProperty = "ConnectedDeviceCount";
        public const string EnabledDeviceCountProperty = "EnabledDeviceCount";
        public const string DisabledDeviceCountProperty = "DisabledDeviceCount";
        public const string TotalDeviceCountProperty = "TotalDeviceCount";
        public const long MaxStringLengthForAzureTableColumn = 23 * 1024;
        public const long MaxDeviceUpdateStatsCount = 500;
        public const string ListenKeyName = "listen";
        public const string DefaultEventHubEndpointName = "events";
        public const string ExportDevicesBlobName = "devices.txt";
        public const string ImportDevicesOutputBlobName = "importErrors.log";
        public static readonly TimeSpan SystemStoreMaximumExecutionTime = TimeSpan.FromSeconds(60);
        public static readonly TimeSpan SystemStoreDeltaBackOff = TimeSpan.FromSeconds(5);

        public const double RpDatabaseResourceLowThreshold = 70.0;
        public const double RpDatabaseResourceHighThreshold = 90.0;
        public const double IotHubResourcePoolsLowThreshold = 40.0;
        public const double IotHubResourcePoolsCriticallyLowThreshold = 10.0;
        public static readonly TimeSpan ProcessThreadCheckInterval = TimeSpan.FromMinutes(1);
        public const int MaxThreadsCountThreshold = 700;

        // Custom HTTP headers
        public const string IotHubErrorCode = "IotHubErrorCode";

        //Service Analytics related
        public static class ServiceAnalytics
        {
            public const string SendEventStatName = "SendEvent";
            public const string EnqueueMessageStatName = "EnqueueMessage";
        }

        // Quota Related
        public static class Quota
        {
            public const string IotHubQuotaMetricsCacheKeyPrefix = "_cache_key_for_iothub_quota_metrics";
            public const string IotHubQuotaMetricsPartitionKeyPrefix = "__system_store_iothub_quota_partition_key__";
            public const double DefaultMessageCountQuotaChunkSizeInKBytes = 64;
        }

        // Usage Related
        public static class Usage
        {
            public const long DefaultUsageMetricFlushThreshold = 100;
            public const long SendEventUsageMetricFlushThreshold = 1000;
            public const long EnqueueMessageUsageMetricFlushThreshold = 500;

            public static readonly TimeSpan QuotaUsageMetricFlushInterval = TimeSpan.FromMinutes(1);
            public static readonly TimeSpan UsageMetricDefaultFlushInterval = TimeSpan.FromMinutes(3);
        }

        public static class CommonServiceConfiguration
        {
            public const string GlobalEncryptionCertificateThumbprint = "IotHub.EncryptionCertificateThumbprint";
            public const string GlobalPrimaryEncryptionKey = "Global.PrimaryEncryptionKey";
            public const string GlobalSecondaryEncryptionKey = "Global.SecondaryEncryptionKey";
        }
        
        // Billing related
        public static class Billing
        {
            public const int UsageRecordPartitionCount = 16;
            public const string FreeSkuMeterId = "a435c5fc-1ab7-4be4-8cf4-223a670cae26";
            public const string Standard1SkuMeterId = "2885a6b2-5013-4fa9-8f3b-fc3ead8d136d";
            public const string Standard2SkuMeterId = "a36d229a-595c-44ad-a66c-1287be4bf433"; 
            public const int EventHubThroughputUnitRatioStandard1 = 1000;
            public const int EventHubThroughputUnitRatioStandard2 = 30;
        }

        public static class CloudToDevice
        {
            public const int DefaultMaxDeliveryCount = 10;
            public const int MaximumMaxDeliveryCount = 100;
            public const int MinimumMaxDeliveryCount = 1;

            public static readonly TimeSpan DefaultTtl = TimeSpan.FromHours(1);
            public static readonly TimeSpan MaximumDefaultTtl = TimeSpan.FromDays(2);
            public static readonly TimeSpan MinimumDefaultTtl = TimeSpan.FromMinutes(1);
        }

        public static class Feedback
        {
            public const string FeedbackQueueInternalOwnerKeyName = "owner";

            public static readonly TimeSpan DefaultLockDuration = TimeSpan.FromMinutes(1);
            public static readonly TimeSpan MaximumLockDuration = TimeSpan.FromMinutes(5);
            public static readonly TimeSpan MinimumLockDuration = TimeSpan.FromSeconds(5);

            public static readonly TimeSpan DefaultTtl = TimeSpan.FromHours(1);
            public static readonly TimeSpan MaximumTtl = TimeSpan.FromDays(2);
            public static readonly TimeSpan MinimumTtl = TimeSpan.FromMinutes(1);

            public const int DefaultMaxDeliveryCount = 10;
            public const int MaximumMaxDeliveryCount = 100;
            public const int MinimumMaxDeliveryCount = 1;

            public const int FreeSkuQueuePartitionSizeInMegabytes = 1024;
            public const int StandardSkuQueuePartitionSizeInMegabytes = 5120;
        }
        
        public static class EventHub
        {
            public const string ScaleUnitSendKeyNamePrefix = "scaleunitsend";
            public const string OwnerKeyNamePrefix = "owner";

            public const int MinPartitionCount = 2;
            public const int MaxPartitionCount = 32;
            public const int DefaultStandardPartitionCount = 4;
            public const int MinRetentionTimeInDays = 1;
            public const int MaxRetentionTimeInDays = 7;
            public const int MaxMessageSize = 255 * 1024;
        }
    }
}
