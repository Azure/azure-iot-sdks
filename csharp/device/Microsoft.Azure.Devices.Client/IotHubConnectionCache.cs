// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.Threading;

#if !PCL
    class IotHubConnectionCache
    {
        readonly ConcurrentDictionary<IotHubConnectionString, IotHubScopeConnectionPool> hubScopeConnectionPools;
        readonly ConcurrentDictionary<IotHubConnectionString, IotHubDeviceScopeConnectionPool> deviceScopeConnectionPools;
        AmqpTransportSettings amqpTransportSettings;

        public IotHubConnectionCache()
        {
            this.hubScopeConnectionPools = new ConcurrentDictionary<IotHubConnectionString, IotHubScopeConnectionPool>(new HubScopeConnectionPoolStringComparer());
            this.deviceScopeConnectionPools = new ConcurrentDictionary<IotHubConnectionString, IotHubDeviceScopeConnectionPool>(new DeviceScopeConnectionPoolStringComparer());
        }

        // Making this virtual to allow Moq to override
        public virtual IotHubConnection GetConnection(IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSetting)
        {
            // Only the initial transportSetting is used, subsequent ones are ignored
            if (this.amqpTransportSettings == null)
            {
                Interlocked.CompareExchange(ref this.amqpTransportSettings, amqpTransportSetting, null);
            }
            else
            {
#if !WINDOWS_UWP
                // Client certificate is per device and must be overriden
                this.amqpTransportSettings.ClientCertificate = amqpTransportSetting.ClientCertificate;
#endif
            }

            IotHubConnection iotHubConnection;
            if (connectionString.SharedAccessKeyName != null || connectionString.SharedAccessSignature != null)
            {
                // Connections are not pooled when SAS signatures are used. However, we still use a connection pool object
                // Connections are not shared since the SAS signature will not match another connection string
                IotHubScopeConnectionPool iotHubScopeConnectionPool;
                do
                {
                    iotHubScopeConnectionPool = 
                        this.hubScopeConnectionPools.GetOrAdd(
                            connectionString, 
                            k => new IotHubScopeConnectionPool(this, k, this.amqpTransportSettings)
                            );
                }
                while (!iotHubScopeConnectionPool.TryAddRef());

                iotHubConnection = iotHubScopeConnectionPool.Connection;
            }
            else if (this.amqpTransportSettings.AmqpConnectionPoolSettings.Pooling)
            {
                // use connection pooling for device scope connection string
                do
                {
                    IotHubDeviceScopeConnectionPool iotHubDeviceScopeConnectionPool =
                        this.deviceScopeConnectionPools.GetOrAdd(
                            connectionString,
                            k => new IotHubDeviceScopeConnectionPool(this, k, this.amqpTransportSettings)
                            );
                    iotHubConnection = iotHubDeviceScopeConnectionPool.GetConnection(connectionString.DeviceId);
                }
                while (iotHubConnection == null);
            }
            else
            {
                // Connection pooling is turned off for device-scope connection strings
                iotHubConnection =  new IotHubSingleTokenConnection(null, connectionString, this.amqpTransportSettings);
            }

            return iotHubConnection;
        }

        // Making this virtual to allow Moq to override
        public virtual bool RemoveHubScopeConnectionPool(IotHubConnectionString connectionString)
        {
            IotHubScopeConnectionPool iotHubScopeConnectionPool;
            return this.hubScopeConnectionPools.TryRemove(connectionString, out iotHubScopeConnectionPool);
        }

        public bool RemoveDeviceScopeConnectionPool(IotHubConnectionString connectionString)
        {
            IotHubDeviceScopeConnectionPool iotHubDeviceScopeConnectionPool;
            return this.deviceScopeConnectionPools.TryRemove(connectionString, out iotHubDeviceScopeConnectionPool);
        }

        // A comparer used to see if two IotHubConnectionStrings can share a common AmqpConnection between them.
        class HubScopeConnectionPoolStringComparer : IEqualityComparer<IotHubConnectionString>
        {
            public bool Equals(IotHubConnectionString connectionString1, IotHubConnectionString connectionString2)
            {
                if (connectionString1 == null && connectionString2 == null)
                {
                    return true;
                }
                else if (connectionString1 == null || connectionString2 == null)
                {
                    return false;
                }

                // Hub-Scope connection strings must match fully
                return connectionString1.IotHubName == connectionString2.IotHubName &&
                    connectionString1.AmqpEndpoint == connectionString2.AmqpEndpoint &&
                    connectionString1.SharedAccessKey == connectionString2.SharedAccessKey &&
                    connectionString1.SharedAccessKeyName == connectionString2.SharedAccessKeyName &&
                    connectionString1.SharedAccessSignature == connectionString2.SharedAccessSignature;
            }

            public int GetHashCode(IotHubConnectionString connectionString)
            {
                if (connectionString == null)
                {
                    return 0;
                }

                return HashCode.CombineHashCodes(
                    HashCode.SafeGet(connectionString.IotHubName),
                    HashCode.SafeGet(connectionString.AmqpEndpoint),
                    HashCode.SafeGet(connectionString.SharedAccessKey),
                    HashCode.SafeGet(connectionString.SharedAccessKeyName),
                    HashCode.SafeGet(connectionString.SharedAccessSignature));
           }
        }

        class DeviceScopeConnectionPoolStringComparer : IEqualityComparer<IotHubConnectionString>
        {
            public bool Equals(IotHubConnectionString connectionString1, IotHubConnectionString connectionString2)
            {
                if (connectionString1 == null && connectionString2 == null)
                {
                    return true;
                }
                else if (connectionString1 == null || connectionString2 == null)
                {
                    return false;
                }

                // device-scope connection strings only need to match on IotHubName and Endpoint
                return connectionString1.IotHubName == connectionString2.IotHubName &&
                    connectionString1.AmqpEndpoint == connectionString2.AmqpEndpoint;
            }

            public int GetHashCode(IotHubConnectionString connectionString)
            {
                if (connectionString == null)
                {
                    return 0;
                }

                return HashCode.CombineHashCodes(
                    HashCode.SafeGet(connectionString.IotHubName),
                    HashCode.SafeGet(connectionString.AmqpEndpoint));
            }
        }
    }
#endif
}