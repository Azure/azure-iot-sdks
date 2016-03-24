// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System.Collections.Concurrent;
    using System.Collections.Generic;

#if !WINDOWS_UWP
    sealed class IotHubConnectionCache
    {
        readonly ConcurrentDictionary<IotHubConnectionString, IotHubScopeConnectionPool> hubScopeConnectionPools;
        readonly ConcurrentDictionary<IotHubConnectionString, IotHubDeviceScopeConnectionPool> deviceScopeConnectionPools;

        public IotHubConnectionCache()
        {
            this.hubScopeConnectionPools = new ConcurrentDictionary<IotHubConnectionString, IotHubScopeConnectionPool>(new HubScopeConnectionPoolStringComparer());
            this.deviceScopeConnectionPools = new ConcurrentDictionary<IotHubConnectionString, IotHubDeviceScopeConnectionPool>(new DeviceScopeConnectionPoolStringComparer());
        }

        public IotHubConnection GetConnection(IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSettings)
        {
            IotHubConnection iotHubConnection;
            // Use connection caching for both hub-scope and device-scope connection strings
            if (connectionString.SharedAccessKeyName != null)
            {
                IotHubScopeConnectionPool connectionReferenceCounter;
                do
                {
                    connectionReferenceCounter = this.hubScopeConnectionPools.GetOrAdd(connectionString, k => new IotHubScopeConnectionPool(this, k, amqpTransportSettings));
                }
                while (!connectionReferenceCounter.TryAddRef());

                iotHubConnection = connectionReferenceCounter.Connection;
            }
            else if (amqpTransportSettings.AmqpConnectionPoolSettings.Pooling)
            {
                do
                {
                    IotHubDeviceScopeConnectionPool iotHubDeviceScopeConnectionPool =
                        this.deviceScopeConnectionPools.GetOrAdd(
                            connectionString,
                            k => new IotHubDeviceScopeConnectionPool(this, k, amqpTransportSettings)
                            );
                    iotHubConnection = iotHubDeviceScopeConnectionPool.GetConnection();
                }
                while (iotHubConnection == null);
            }
            else
            {
                // Connection pooling is turned off for device-scope connection strings
                iotHubConnection =  new IotHubMuxConnection(null, connectionString, amqpTransportSettings);
            }

            return iotHubConnection;
        }

        public bool RemoveHubScopeConnectionPool(IotHubConnectionString connectionString)
        {
            IotHubScopeConnectionPool iotHubScopeConnectionPool;
            if (this.hubScopeConnectionPools.TryRemove(connectionString, out iotHubScopeConnectionPool))
            {
                return true;
            }

            return false;
        }

        public bool RemoveDeviceScopeConnectionPool(IotHubConnectionString connectionString)
        {
            IotHubDeviceScopeConnectionPool iotHubDeviceScopeConnectionPool;
            if (this.deviceScopeConnectionPools.TryRemove(connectionString, out iotHubDeviceScopeConnectionPool))
            {
                return true;
            }

            return false;
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