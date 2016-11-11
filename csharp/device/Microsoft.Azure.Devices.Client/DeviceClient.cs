// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Common;
    using System;
    using System.Linq;
    using System.Collections.Generic;
    using System.Text.RegularExpressions;
    using System.Threading;
    using Microsoft.Azure.Devices.Client.Extensions;
    using Microsoft.Azure.Devices.Client.Transport;
#if !WINDOWS_UWP && !PCL
    using Microsoft.Azure.Devices.Client.Transport.Mqtt;
#endif

    // C# using aliases cannot name an unbound generic type declaration without supplying type arguments
    // Therefore, define a separate alias for each type argument
#if WINDOWS_UWP
    using AsyncTask = Windows.Foundation.IAsyncAction;
    using AsyncTaskOfMessage = Windows.Foundation.IAsyncOperation<Message>;
#else
    using AsyncTask = System.Threading.Tasks.Task;
    using AsyncTaskOfMessage = System.Threading.Tasks.Task<Message>;
#endif

    public delegate string MethodCallback(string payload, out MethodStatusType status);

    /*
     * Class Diagramm and Chain of Responsibility in Device Client 
                             +--------------------+
                             | <<interface>>      |
                             | IDelegatingHandler |
                             |  * Open            |
                             |  * Close           |
                             |  * SendEvent       |
                             |  * SendEvents      |
                             |  * Receive         |
                             |  * Complete        |
                             |  * Abandon         |
                             |  * Reject          |
                             +-------+------------+
                                     |
                                     |implements
                                     |
                                     |
                             +-------+-------+
                             |  <<abstract>> |     
                             |  Default      |
     +---+inherits----------->  Delegating   <------inherits-----------------+
     |                       |  Handler      |                               |
     |           +--inherits->               <--inherits----+                |
     |           |           +-------^-------+              |                |
     |           |                   |inherits              |                |
     |           |                   |                      |                |
+------------+       +---+---------+      +--+----------+       +---+--------+       +--------------+
|            |       |             |      |             |       |            |       | <<abstract>> |
| GateKeeper |  use  | Retry       | use  |  Error      |  use  | Routing    |  use  | Transport    |
| Delegating +-------> Delegating  +------>  Delegating +-------> Delegating +-------> Delegating   |
| Handler    |       | Handler     |      |  Handler    |       | Handler    |       | Handler      |
|            |       |             |      |             |       |            |       |              |
| overrides: |       | overrides:  |      |  overrides  |       | overrides: |       | overrides:   |
|  Open      |       |  Open       |      |   Open      |       |  Open      |       |  Receive     |
|  Close     |       |  SendEvent  |      |   SendEvent |       |            |       |              |
|            |       |  SendEvents |      |   SendEvents|       +------------+       +--^--^---^----+
+------------+       |  Receive    |      |   Receive   |                               |  |   |
                     |  Reject     |      |   Reject    |                               |  |   |
                     |  Abandon    |      |   Abandon   |                               |  |   |
                     |  Complete   |      |   Complete  |                               |  |   |
                     |             |      |             |                               |  |   |
                     +-------------+      +-------------+     +-------------+-+inherits-+  |   +---inherits-+-------------+
                                                              |             |              |                |             |
                                                              | AMQP        |              inherits         | HTTP        |
                                                              | Transport   |              |                | Transport   |
                                                              | Handler     |          +---+---------+      | Handler     |
                                                              |             |          |             |      |             |
                                                              | overrides:  |          | MQTT        |      | overrides:  |
                                                              |  everything |          | Transport   |      |  everything |
                                                              |             |          | Handler     |      |             |
                                                              +-------------+          |             |      +-------------+
                                                                                       | overrides:  |
                                                                                       |  everything |
                                                                                       |             |
                                                                                       +-------------+
*/

    /// <summary>
    /// Contains methods that a device can use to send messages to and receive from the service.
    /// </summary>
    public sealed class DeviceClient
#if !WINDOWS_UWP && !PCL
        : IDisposable
#endif
    {
        const string DeviceId = "DeviceId";
        const string DeviceIdParameterPattern = @"(^\s*?|.*;\s*?)" + DeviceId + @"\s*?=.*";
#if !PCL
        IotHubConnectionString iotHubConnectionString = null;
        const RegexOptions RegexOptions = System.Text.RegularExpressions.RegexOptions.Compiled | System.Text.RegularExpressions.RegexOptions.IgnoreCase;
#else
        const RegexOptions RegexOptions = System.Text.RegularExpressions.RegexOptions.IgnoreCase;
#endif
        static readonly Regex DeviceIdParameterRegex = new Regex(DeviceIdParameterPattern, RegexOptions);

        internal IDelegatingHandler InnerHandler { get; set; }

        /// <summary>
        /// Stores the timeout used in the operation retries.
        /// </summary>
        /* Codes_SRS_DEVICECLIENT_28_002: [This property shall be defaulted to 240000 (4 minutes).] */
        public uint OperationTimeoutInMilliseconds { get; set; } = 4 * 60 * 1000;

        /// <summary>
        /// Stores the retry strategy used in the operation retries.
        /// </summary>
        public RetryPolicyType RetryPolicy { get; set; }

        /// <summary>
        /// Stores Methods supported by the client device and their associated delegate.
        /// </summary>
        Dictionary<string, MethodCallback> deviceMethods;

#if !PCL
        DeviceClient(IotHubConnectionString iotHubConnectionString, ITransportSettings[] transportSettings)
        {
            this.iotHubConnectionString = iotHubConnectionString;

#if !WINDOWS_UWP
            var innerHandler = new RetryDelegatingHandler(
                new ErrorDelegatingHandler(
                    () => new RoutingDelegatingHandler(this.CreateTransportHandler, iotHubConnectionString, transportSettings)));
#else
            // UWP does not support retry yet. We need to make the underlying Message stream accessible internally on UWP
            // to be sure that either the stream has not been read or it is seekable to safely retry operation
            var innerHandler = new ErrorDelegatingHandler(
                () => new RoutingDelegatingHandler(this.CreateTransportHandler, iotHubConnectionString, transportSettings));
#endif
            this.InnerHandler = new GateKeeperDelegatingHandler(innerHandler);
        }

        DefaultDelegatingHandler CreateTransportHandler(IotHubConnectionString iotHubConnectionString, ITransportSettings transportSetting)
        {
            switch (transportSetting.GetTransportType())
            {
                case TransportType.Amqp_WebSocket_Only:
                case TransportType.Amqp_Tcp_Only:
                    return new AmqpTransportHandler(iotHubConnectionString, transportSetting as AmqpTransportSettings);
                case TransportType.Http1:
                    return new HttpTransportHandler(iotHubConnectionString, transportSetting as Http1TransportSettings);
#if !WINDOWS_UWP && !NETMF
                case TransportType.Mqtt_WebSocket_Only:
                case TransportType.Mqtt_Tcp_Only:
                    return new MqttTransportHandler(iotHubConnectionString, transportSetting as MqttTransportSettings);
#endif
                default:
                    throw new InvalidOperationException("Unsupported Transport Setting {0}".FormatInvariant(transportSetting));
            }
        }

#else
        DeviceClient(IotHubConnectionString iotHubConnectionString)
        {
            this.InnerHandler = new GateKeeperDelegatingHandler(
                new ErrorDelegatingHandler(() => new HttpTransportHandler(iotHubConnectionString)));
        }
#endif

        /// <summary>
        /// Create an Amqp DeviceClient from individual parameters
        /// </summary>
        /// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        /// <param name="authenticationMethod">The authentication method that is used</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient Create(string hostname, IAuthenticationMethod authenticationMethod)
        {
#if WINDOWS_UWP || PCL
            return Create(hostname, authenticationMethod, TransportType.Http1);
#else
            return Create(hostname, authenticationMethod, TransportType.Amqp);
#endif
        }

#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute()]
#endif
        /// <summary>
        /// Create a DeviceClient from individual parameters
        /// </summary>
        /// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        /// <param name="authenticationMethod">The authentication method that is used</param>
        /// <param name="transportType">The transportType used (Http1 or Amqp)</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient Create(string hostname, IAuthenticationMethod authenticationMethod, TransportType transportType)
        {
            if (hostname == null)
            {
                throw new ArgumentNullException(nameof(hostname));
            }

            if (authenticationMethod == null)
            {
                throw new ArgumentNullException(nameof(authenticationMethod));
            }

            var connectionStringBuilder = IotHubConnectionStringBuilder.Create(hostname, authenticationMethod);

#if !WINDOWS_UWP && !PCL && !NETMF
            if (authenticationMethod is DeviceAuthenticationWithX509Certificate)
            {
                if (connectionStringBuilder.Certificate == null)
                {
                    throw new ArgumentException("certificate must be present in DeviceAuthenticationWithX509Certificate");
                }

                return CreateFromConnectionString(connectionStringBuilder.ToString(), PopulateCertificateInTransportSettings(connectionStringBuilder, transportType));
            }
#endif
            return CreateFromConnectionString(connectionStringBuilder.ToString(), transportType);
        }

#if !PCL
        /// <summary>
        /// Create a DeviceClient from individual parameters
        /// </summary>
        /// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        /// <param name="authenticationMethod">The authentication method that is used</param>
        /// <param name="transportSettings">Prioritized list of transportTypes and their settings</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient Create(string hostname, IAuthenticationMethod authenticationMethod, [System.Runtime.InteropServices.WindowsRuntime.ReadOnlyArrayAttribute] ITransportSettings[] transportSettings)
        {
            if (hostname == null)
            {
                throw new ArgumentNullException("hostname");
            }

            if (authenticationMethod == null)
            {
                throw new ArgumentNullException("authenticationMethod");
            }

            var connectionStringBuilder = IotHubConnectionStringBuilder.Create(hostname, authenticationMethod);
#if !WINDOWS_UWP && !PCL && !NETMF
            if (authenticationMethod is DeviceAuthenticationWithX509Certificate)
            {
                if (connectionStringBuilder.Certificate == null)
                {
                    throw new ArgumentException("certificate must be present in DeviceAuthenticationWithX509Certificate");
                }

                return CreateFromConnectionString(connectionStringBuilder.ToString(), PopulateCertificateInTransportSettings(connectionStringBuilder, transportSettings));
            }
#endif
            return CreateFromConnectionString(connectionStringBuilder.ToString(), transportSettings);
        }
#endif

        /// <summary>
        /// Create a DeviceClient using Amqp transport from the specified connection string
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (including DeviceId)</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString)
        {
#if WINDOWS_UWP || PCL
            return CreateFromConnectionString(connectionString, TransportType.Http1);
#else
            return CreateFromConnectionString(connectionString, TransportType.Amqp);
#endif
        }

        /// <summary>
        /// Create a DeviceClient using Amqp transport from the specified connection string
        /// (WinRT) Create a DeviceClient using Http transport from the specified connection string
        /// </summary>
        /// <param name="connectionString">IoT Hub-Scope Connection string for the IoT hub (without DeviceId)</param>
        /// <param name="deviceId">Id of the Device</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, string deviceId)
        {
#if WINDOWS_UWP || PCL
            return CreateFromConnectionString(connectionString, deviceId, TransportType.Http1);    
#else
            return CreateFromConnectionString(connectionString, deviceId, TransportType.Amqp);
#endif
        }

#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute()]
#endif

        /// <summary>
        /// Create DeviceClient from the specified connection string using the specified transport type
        /// (PCL) Only Http transport is allowed
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (including DeviceId)</param>
        /// <param name="transportType">Specifies whether Amqp or Http transport is used</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, TransportType transportType)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException(nameof(connectionString));
            }

            switch (transportType)
            {
                case TransportType.Amqp:
#if PCL
                    throw new NotImplementedException("Amqp protocol is not supported");
#else
                    return CreateFromConnectionString(connectionString, new ITransportSettings[]
                    {
                        new AmqpTransportSettings(TransportType.Amqp_Tcp_Only),
                        new AmqpTransportSettings(TransportType.Amqp_WebSocket_Only)
                    });
#endif
                case TransportType.Mqtt:
#if WINDOWS_UWP || PCL
                    throw new NotImplementedException("Mqtt protocol is not supported");
#else
                    return CreateFromConnectionString(connectionString, new ITransportSettings[]
                    {
                        new MqttTransportSettings(TransportType.Mqtt_Tcp_Only),
                        new MqttTransportSettings(TransportType.Mqtt_WebSocket_Only)
                    });
#endif
                case TransportType.Amqp_WebSocket_Only:
                case TransportType.Amqp_Tcp_Only:
#if PCL
                    throw new NotImplementedException("Amqp protocol is not supported");
#else
                    return CreateFromConnectionString(connectionString, new ITransportSettings[] { new AmqpTransportSettings(transportType) });
#endif
                case TransportType.Mqtt_WebSocket_Only:
                case TransportType.Mqtt_Tcp_Only:
#if WINDOWS_UWP || PCL
                    throw new NotImplementedException("Mqtt protocol is not supported");
#else
                    return CreateFromConnectionString(connectionString, new ITransportSettings[] { new MqttTransportSettings(transportType) });
#endif
                case TransportType.Http1:
#if PCL
                    IotHubConnectionString iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
                    return new DeviceClient(iotHubConnectionString);
#else
                    return CreateFromConnectionString(connectionString, new ITransportSettings[] { new Http1TransportSettings() });
#endif
                default:
#if !PCL
                    throw new InvalidOperationException("Unsupported Transport Type {0}".FormatInvariant(transportType));
#else
                    throw new InvalidOperationException($"Unsupported Transport Type {transportType}");
#endif
            }
        }

        /// <summary>
        /// Create DeviceClient from the specified connection string using the specified transport type
        /// </summary>
        /// <param name="connectionString">IoT Hub-Scope Connection string for the IoT hub (without DeviceId)</param>
        /// <param name="deviceId">Id of the device</param>
        /// <param name="transportType">The transportType used (Http1 or Amqp)</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, string deviceId, TransportType transportType)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException(nameof(connectionString));
            }

            if (deviceId == null)
            {
                throw new ArgumentNullException(nameof(deviceId));
            }

            if (DeviceIdParameterRegex.IsMatch(connectionString))
            {
                throw new ArgumentException("Connection string must not contain DeviceId keyvalue parameter", nameof(connectionString));
            }

            return CreateFromConnectionString(connectionString + ";" + DeviceId + "=" + deviceId, transportType);
        }

#if !PCL
        /// <summary>
        /// Create DeviceClient from the specified connection string using a prioritized list of transports
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (with DeviceId)</param>
        /// <param name="transportSettings">Prioritized list of transports and their settings</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, [System.Runtime.InteropServices.WindowsRuntime.ReadOnlyArrayAttribute] ITransportSettings[] transportSettings)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException(nameof(connectionString));
            }

            if (transportSettings == null)
            {
                throw new ArgumentNullException(nameof(transportSettings));
            }

            if (transportSettings.Length == 0)
            {
                throw new ArgumentOutOfRangeException(nameof(connectionString), "Must specify at least one TransportSettings instance");
            }

            IotHubConnectionString iotHubConnectionString = IotHubConnectionString.Parse(connectionString);

            foreach (ITransportSettings transportSetting in transportSettings)
            {
                switch (transportSetting.GetTransportType())
                {
                    case TransportType.Amqp_WebSocket_Only:
                    case TransportType.Amqp_Tcp_Only:
                        if (!(transportSetting is AmqpTransportSettings))
                        {
                            throw new InvalidOperationException("Unknown implementation of ITransportSettings type");
                        }
                        break;
                    case TransportType.Http1:
                        if (!(transportSetting is Http1TransportSettings))
                        {
                            throw new InvalidOperationException("Unknown implementation of ITransportSettings type");
                        }
                        break;
#if !WINDOWS_UWP
                    case TransportType.Mqtt_WebSocket_Only:
                    case TransportType.Mqtt_Tcp_Only:
                        if (!(transportSetting is MqttTransportSettings))
                        {
                            throw new InvalidOperationException("Unknown implementation of ITransportSettings type");
                        }
                        break;
#endif
                    default:
                        throw new InvalidOperationException("Unsupported Transport Type {0}".FormatInvariant(transportSetting.GetTransportType()));
                }
            }

            // Defer concrete DeviceClient creation to OpenAsync
            return new DeviceClient(iotHubConnectionString, transportSettings);
        }

        /// <summary>
        /// Create DeviceClient from the specified connection string using the prioritized list of transports
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (without DeviceId)</param>
        /// <param name="deviceId">Id of the device</param>
        /// <param name="transportSettings">Prioritized list of transportTypes and their settings</param>
        /// <returns>DeviceClient</returns>
#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute]
#endif
        public static DeviceClient CreateFromConnectionString(string connectionString, string deviceId, [System.Runtime.InteropServices.WindowsRuntime.ReadOnlyArrayAttribute] ITransportSettings[] transportSettings)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException(nameof(connectionString));
            }

            if (deviceId == null)
            {
                throw new ArgumentNullException(nameof(deviceId));
            }

            if (DeviceIdParameterRegex.IsMatch(connectionString))
            {
                throw new ArgumentException("Connection string must not contain DeviceId keyvalue parameter", nameof(connectionString));
            }

            return CreateFromConnectionString(connectionString + ";" + DeviceId + "=" + deviceId, transportSettings);
        }
#endif

        private CancellationTokenSource GetOperationTimeoutCancellationTokenSource()
        {
            return new CancellationTokenSource(TimeSpan.FromMilliseconds(OperationTimeoutInMilliseconds));
        }

        /// <summary>
        /// Explicitly open the DeviceClient instance.
        /// </summary>

        public AsyncTask OpenAsync()
        {
            /* Codes_SRS_DEVICECLIENT_28_007: [ The async operation shall retry until time specified in OperationTimeoutInMilliseconds property expire or unrecoverable(authentication, quota exceed) error occurs.] */
            return ApplyTimeout(operationTimeoutCancellationToken => this.InnerHandler.OpenAsync(true, operationTimeoutCancellationToken));
        }

        /// <summary>
        /// Close the DeviceClient instance
        /// </summary>
        /// <returns></returns>
        public AsyncTask CloseAsync()
        {
            return this.InnerHandler.CloseAsync().AsTaskOrAsyncOp();
        }

        /// <summary>
        /// Receive a message from the device queue using the default timeout.
        /// </summary>
        /// <returns>The receive message or null if there was no message until the default timeout</returns>
        public AsyncTaskOfMessage ReceiveAsync()
        {
            /* Codes_SRS_DEVICECLIENT_28_011: [The async operation shall retry until time specified in OperationTimeoutInMilliseconds property expire or unrecoverable(authentication, quota exceed) error occurs.] */
            return ApplyTimeout(operationTimeoutCancellationToken => this.InnerHandler.ReceiveAsync(operationTimeoutCancellationToken));
        }

        /// <summary>
        /// Receive a message from the device queue with the specified timeout
        /// </summary>
        /// <returns>The receive message or null if there was no message until the specified time has elapsed</returns>
        public AsyncTaskOfMessage ReceiveAsync(TimeSpan timeout)
        {
            /* Codes_SRS_DEVICECLIENT_28_011: [The async operation shall retry until time specified in OperationTimeoutInMilliseconds property expire or unrecoverable(authentication, quota exceed) error occurs.] */
            return ApplyTimeout(operationTimeoutCancellationToken => this.InnerHandler.ReceiveAsync(timeout, operationTimeoutCancellationToken));
        }



#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute()]
#endif

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public AsyncTask CompleteAsync(string lockToken)
        {
            if (string.IsNullOrEmpty(lockToken))
            {
                throw Fx.Exception.ArgumentNull("lockToken");
            }

            /* Codes_SRS_DEVICECLIENT_28_013: [The async operation shall retry until time specified in OperationTimeoutInMilliseconds property expire or unrecoverable error(authentication, quota exceed) occurs.] */
            return ApplyTimeout(operationTimeoutCancellationToken => this.InnerHandler.CompleteAsync(lockToken, operationTimeoutCancellationToken));
        }

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public AsyncTask CompleteAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }
            /* Codes_SRS_DEVICECLIENT_28_015: [The async operation shall retry until time specified in OperationTimeoutInMilliseconds property expire or unrecoverable error(authentication, quota exceed) occurs.] */
            return this.CompleteAsync(message.LockToken);
        }

#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute()]
#endif

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public AsyncTask AbandonAsync(string lockToken)
        {
            if (string.IsNullOrEmpty(lockToken))
            {
                throw Fx.Exception.ArgumentNull("lockToken");
            }
            /* Codes_SRS_DEVICECLIENT_28_015: [The async operation shall retry until time specified in OperationTimeoutInMilliseconds property expire or unrecoverable error(authentication, quota exceed) occurs.] */
            return ApplyTimeout(operationTimeoutCancellationToken => this.InnerHandler.AbandonAsync(lockToken, operationTimeoutCancellationToken));
        }

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public AsyncTask AbandonAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }

            return this.AbandonAsync(message.LockToken);
        }

#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverloadAttribute()]
#endif

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The previously received message</returns>
        public AsyncTask RejectAsync(string lockToken)
        {
            if (string.IsNullOrEmpty(lockToken))
            {
                throw Fx.Exception.ArgumentNull("lockToken");
            }

            return ApplyTimeout(operationTimeoutCancellationToken => this.InnerHandler.RejectAsync(lockToken, operationTimeoutCancellationToken));
        }

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public AsyncTask RejectAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }
            /* Codes_SRS_DEVICECLIENT_28_017: [The async operation shall retry until time specified in OperationTimeoutInMilliseconds property expire or unrecoverable error(authentication, quota exceed) occurs.] */
            return ApplyTimeout(operationTimeoutCancellationToken => this.InnerHandler.RejectAsync(message.LockToken, operationTimeoutCancellationToken));
        }

        /// <summary>
        /// Sends an event to device hub
        /// </summary>
        /// <returns>The message containing the event</returns>
        public AsyncTask SendEventAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }
            /* Codes_SRS_DEVICECLIENT_28_019: [The async operation shall retry until time specified in OperationTimeoutInMilliseconds property expire or unrecoverable error(authentication or quota exceed) occurs.] */
            return ApplyTimeout(operationTimeoutCancellationToken => this.InnerHandler.SendEventAsync(message, operationTimeoutCancellationToken));
        }

        /// <summary>
        /// Sends a batch of events to device hub
        /// </summary>
        /// <returns>The task containing the event</returns>
        public AsyncTask SendEventBatchAsync(IEnumerable<Message> messages)
        {
            if (messages == null)
            {
                throw Fx.Exception.ArgumentNull("messages");
            }
            /* Codes_SRS_DEVICECLIENT_28_019: [The async operation shall retry until time specified in OperationTimeoutInMilliseconds property expire or unrecoverable error(authentication or quota exceed) occurs.] */
            return ApplyTimeout(operationTimeoutCancellationToken => this.InnerHandler.SendEventAsync(messages, operationTimeoutCancellationToken));
        }

        private AsyncTask ApplyTimeout(Func<CancellationToken, System.Threading.Tasks.Task> operation)
        {
            if (OperationTimeoutInMilliseconds == 0)
            {
                return operation(CancellationToken.None)
                    .WithTimeout(TimeSpan.MaxValue, () => Resources.OperationTimeoutExpired, CancellationToken.None)
                    .AsTaskOrAsyncOp();
            }

            CancellationTokenSource operationTimeoutCancellationTokenSource = GetOperationTimeoutCancellationTokenSource();

            var result = operation(operationTimeoutCancellationTokenSource.Token)
                .WithTimeout(TimeSpan.FromMilliseconds(OperationTimeoutInMilliseconds), () => Resources.OperationTimeoutExpired, operationTimeoutCancellationTokenSource.Token);
            result.ContinueWith(t =>
                {
                    operationTimeoutCancellationTokenSource.Dispose();
                });
            return result.AsTaskOrAsyncOp();
        }

        private AsyncTaskOfMessage ApplyTimeout(Func<CancellationToken, System.Threading.Tasks.Task<Message>> operation)
        {
            if (OperationTimeoutInMilliseconds == 0)
            {
                return operation(CancellationToken.None)
                    .WithTimeout(TimeSpan.MaxValue, () => Resources.OperationTimeoutExpired, CancellationToken.None)
                    .AsTaskOrAsyncOp();
            }
            
            CancellationTokenSource operationTimeoutCancellationTokenSource = GetOperationTimeoutCancellationTokenSource();

            var result = operation(operationTimeoutCancellationTokenSource.Token)
                .WithTimeout(TimeSpan.FromMilliseconds(OperationTimeoutInMilliseconds), () => Resources.OperationTimeoutExpired, operationTimeoutCancellationTokenSource.Token);
            result.ContinueWith(t =>
                {
                    operationTimeoutCancellationTokenSource.Dispose();
                    return t.Result;
                });
            return result.AsTaskOrAsyncOp();
        }


#if !WINDOWS_UWP && !PCL
        /// <summary>
        /// Uploads a stream to a block blob in a storage account associated with the IoTHub for that device.
        /// If the blob already exists, it will be overwritten.
        /// </summary>
        /// <param name="blobName"></param>
        /// <param name="source"></param>
        /// <returns>AsncTask</returns>
        public AsyncTask UploadToBlobAsync(String blobName, System.IO.Stream source)
        {
            if (String.IsNullOrEmpty(blobName))
            {
                throw Fx.Exception.ArgumentNull("blobName");
            }
            if (source == null)
            {
                throw Fx.Exception.ArgumentNull("source");
            }
            if (blobName.Length > 1024)
            {
                throw Fx.Exception.Argument("blobName", "Length cannot exceed 1024 characters");
            }
            if (blobName.Split('/').Count() > 254)
            {
                throw Fx.Exception.Argument("blobName", "Path segment count cannot exceed 254");
            }

            var httpTransport = new HttpTransportHandler(iotHubConnectionString);
            return httpTransport.UploadToBlobAsync(blobName, source);
        }
#endif

        /// <summary>
        /// This method will initialize the transport layer internal Method handling processing. The
        /// user must call this method at least once before method calls will be enabled.
        /// </summary>
        public AsyncTask EnableMethodsAsync()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Registers a new delgate for the named method. If a delegate is already associated with
        /// the named method, it will be replaced with the new delegate.
        /// <param name="methodName"></param>
        /// <param name="methodDelegate"></param>
        /// </summary>
        public void SetMethodDelegate(string methodName, MethodCallback methodDelegate)
        {
            /* codes_SRS_DEVICECLIENT_10_001: [ The SetMethodDelegate shall lazy-initialize the deviceMethods property. ]*/
            if (this.deviceMethods == null)
            {
                this.deviceMethods = new Dictionary<string, MethodCallback>();
            }

            /* codes_SRS_DEVICECLIENT_10_002: [** If the given methodName already has an associated delegate, the existing delegate shall be removed. ]*/
            /* codes_SRS_DEVICECLIENT_10_003: [** The given delegate will only be added if it is not null. ]*/
            if (methodDelegate == null)
            {
                this.deviceMethods.Remove(methodName);
            }
            else
            {
                this.deviceMethods[methodName] = methodDelegate;
            }

            /* codes_SRS_DEVICECLIENT_10_004: [** The deviceMethods property shall be deleted if the last delegate has been removed. ]*/
            if (this.deviceMethods.Count == 0)
            {
                this.deviceMethods = null;
            }
        }

        internal void OnMethodCalled(Method method)
        {
            if (this.deviceMethods.ContainsKey(method.Name))
            {
                /* codes_SRS_DEVICECLIENT_10_011: [** The OnMethodCalled shall invoke the specified delegate. ]*/
                MethodStatusType status = MethodStatusType.NotImplemented;
                method.Result = this.deviceMethods[method.Name](method.Payload, out status);
                method.Status = status;
            }
            else
            {
                /* codes_SRS_DEVICECLIENT_10_012: [ If the given method does not have a delegate, the respose shall be set to Unsupported. ]*/
                method.Result = null;
                method.Status = MethodStatusType.NotSupported;
            }

            // ArgumentNullException is percolated to the caller.
        }

        public void Dispose()
        {
            this.InnerHandler?.Dispose();
        }

#if !WINDOWS_UWP && !PCL
        static ITransportSettings[] PopulateCertificateInTransportSettings(IotHubConnectionStringBuilder connectionStringBuilder, TransportType transportType)
        {
            switch (transportType)
            {
                case TransportType.Amqp:
                    return new ITransportSettings[]
                    {
                        new AmqpTransportSettings(TransportType.Amqp_Tcp_Only)
                        {
                            ClientCertificate = connectionStringBuilder.Certificate
                        },
                        new AmqpTransportSettings(TransportType.Amqp_WebSocket_Only)
                        {
                            ClientCertificate = connectionStringBuilder.Certificate
                        }
                    };
                case TransportType.Amqp_Tcp_Only:
                    return new ITransportSettings[]
                    {
                        new AmqpTransportSettings(TransportType.Amqp_Tcp_Only)
                        {
                            ClientCertificate = connectionStringBuilder.Certificate
                        }
                    };
                case TransportType.Amqp_WebSocket_Only:
                    return new ITransportSettings[]
                    {
                        new AmqpTransportSettings(TransportType.Amqp_WebSocket_Only)
                        {
                            ClientCertificate = connectionStringBuilder.Certificate
                        }
                    };
                case TransportType.Http1:
                    return new ITransportSettings[]
                    {
                        new Http1TransportSettings()
                        {
                            ClientCertificate = connectionStringBuilder.Certificate
                        }
                    };
                case TransportType.Mqtt:
                    return new ITransportSettings[]
                    {
                        new MqttTransportSettings(TransportType.Mqtt_Tcp_Only) 
                        {
                            ClientCertificate = connectionStringBuilder.Certificate
                        },
                        new MqttTransportSettings(TransportType.Mqtt_WebSocket_Only)
                        {
                            ClientCertificate = connectionStringBuilder.Certificate
                        }
                    };
                case TransportType.Mqtt_Tcp_Only:
                    return new ITransportSettings[]
                    {
                        new MqttTransportSettings(TransportType.Mqtt_Tcp_Only)
                        {
                            ClientCertificate = connectionStringBuilder.Certificate
                        }
                    };
                case TransportType.Mqtt_WebSocket_Only:
                    return new ITransportSettings[]
                    {
                        new MqttTransportSettings(TransportType.Mqtt_WebSocket_Only)
                        {
                            ClientCertificate = connectionStringBuilder.Certificate
                        }
                    };
                default:
                    throw new InvalidOperationException("Unsupported Transport {0}".FormatInvariant(transportType));
            }
        }

        static ITransportSettings[] PopulateCertificateInTransportSettings(IotHubConnectionStringBuilder connectionStringBuilder, ITransportSettings[] transportSettings)
        {
            foreach (var transportSetting in  transportSettings)
            {
                switch (transportSetting.GetTransportType())
                {
                    case TransportType.Amqp_WebSocket_Only:
                    case TransportType.Amqp_Tcp_Only:
                        ((AmqpTransportSettings)transportSetting).ClientCertificate = connectionStringBuilder.Certificate;
                        break;
                    case TransportType.Http1:
                        ((Http1TransportSettings)transportSetting).ClientCertificate = connectionStringBuilder.Certificate;
                        break;
                    case TransportType.Mqtt_WebSocket_Only:
                    case TransportType.Mqtt_Tcp_Only:
                        ((MqttTransportSettings)transportSetting).ClientCertificate = connectionStringBuilder.Certificate;
                        break;
                    default:
                        throw new InvalidOperationException("Unsupported Transport {0}".FormatInvariant(transportSetting.GetTransportType()));
                }
            }

            return transportSettings;
        }
#endif
    }
}
