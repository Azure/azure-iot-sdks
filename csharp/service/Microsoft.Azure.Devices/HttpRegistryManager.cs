// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net;
    using System.Net.Http;
    using System.Text.RegularExpressions;
    using System.Threading;
    using System.Threading.Tasks;

    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Exceptions;

    class HttpRegistryManager : RegistryManager, IDisposable
    {
        const string AdminUriFormat = "/$admin/{0}?{1}";
        const string RequestUriFormat = "/devices/{0}?{1}";
        const string JobsUriFormat = "/jobs{0}?{1}";
        const string StatisticsUriFormat = "/statistics/devices?" + ClientApiVersionHelper.ApiVersionQueryString;
        const string DevicesRequestUriFormat = "/devices/?top={0}&{1}";

        static readonly Regex DeviceIdRegex = new Regex(@"^[A-Za-z0-9\-:.+%_#*?!(),=@;$']{1,128}$", RegexOptions.Compiled | RegexOptions.ECMAScript | RegexOptions.IgnoreCase);
        static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromSeconds(100);
        
        IHttpClientHelper httpClientHelper;
        readonly string iotHubName;

        internal HttpRegistryManager(IotHubConnectionString connectionString)
        {
            this.iotHubName = connectionString.IotHubName;
            this.httpClientHelper = new HttpClientHelper(
                connectionString.HttpsEndpoint,
                connectionString,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                DefaultOperationTimeout,
                client => { });
        }

        // internal test helper
        internal HttpRegistryManager(IHttpClientHelper httpClientHelper, string iotHubName)
        {
            if (httpClientHelper == null)
            {
                throw new ArgumentNullException("httpClientHelper");
            }

            this.iotHubName = iotHubName;
            this.httpClientHelper = httpClientHelper;
        }

        public override Task OpenAsync()
        {
            return TaskHelpers.CompletedTask;
        }

        public override Task CloseAsync()
        {
            if (this.httpClientHelper != null)
            {
                this.httpClientHelper.Dispose();
                this.httpClientHelper = null;
            }

            return TaskHelpers.CompletedTask;
        }

        public override Task<Device> AddDeviceAsync(Device device)
        {
            return this.AddDeviceAsync(device, CancellationToken.None);
        }

        public override Task<Device> AddDeviceAsync(Device device, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            ValidateDeviceId(device);

            if (!string.IsNullOrEmpty(device.ETag))
            {
                throw new ArgumentException(ApiResources.ETagSetWhileRegisteringDevice);
            }

            ValidateDeviceAuthentication(device.Authentication, device.Id);

            // auto generate keys if not specified
            if (device.Authentication == null)
            {
                device.Authentication = new AuthenticationMechanism();
            }

            return this.httpClientHelper.PutAsync(GetRequestUri(device.Id), device, PutOperationType.CreateEntity, null, cancellationToken);
        }

        public override Task<string[]> AddDevicesAsync(IEnumerable<Device> devices)
        {
            return this.AddDevicesAsync(devices, CancellationToken.None);
        }

        public override Task<string[]> AddDevicesAsync(IEnumerable<Device> devices, CancellationToken cancellationToken)
        {
            if (devices == null)
            {
                throw new ArgumentNullException("devices");
            }

            if (!devices.Any())
            {
                throw new ArgumentException("devices");
            }

            var exportImportDeviceList = new List<ExportImportDevice>(devices.Count());
            foreach (var device in devices)
            {
                ValidateDeviceId(device);

                if (!string.IsNullOrEmpty(device.ETag))
                {
                    throw new ArgumentException(ApiResources.ETagSetWhileRegisteringDevice);
                }

                var exportImportDevice = new ExportImportDevice(device, ImportMode.Create);
                exportImportDeviceList.Add(exportImportDevice);
            }

            return this.BulkDeviceOperationsAsync(exportImportDeviceList, cancellationToken);
        }

        public override Task<Device> UpdateDeviceAsync(Device device)
        {
            return this.UpdateDeviceAsync(device, CancellationToken.None);
        }

        public override Task<Device> UpdateDeviceAsync(Device device, bool forceUpdate)
        {
            return this.UpdateDeviceAsync(device, forceUpdate, CancellationToken.None);
        }

        public override Task<Device> UpdateDeviceAsync(Device device, CancellationToken cancellationToken)
        {
            return this.UpdateDeviceAsync(device, false, cancellationToken);
        }

        public override Task<Device> UpdateDeviceAsync(Device device, bool forceUpdate, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            ValidateDeviceId(device);

            if (string.IsNullOrWhiteSpace(device.ETag) && !forceUpdate)
            {
                throw new ArgumentException(ApiResources.ETagNotSetWhileUpdatingDevice);
            }

            ValidateDeviceAuthentication(device.Authentication, device.Id);

            // auto generate keys if not specified
            if (device.Authentication == null)
            {
                device.Authentication = new AuthenticationMechanism();
            }

            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.PreconditionFailed, async (responseMessage) => new PreconditionFailedException(await ExceptionHandlingHelper.GetExceptionMessageAsync(responseMessage)));
            errorMappingOverrides.Add(HttpStatusCode.NotFound, async responseMessage =>
            {
                var responseContent = await ExceptionHandlingHelper.GetExceptionMessageAsync(responseMessage);
                return (Exception) new DeviceNotFoundException(responseContent, (Exception) null);
            });

            PutOperationType operationType = forceUpdate ? PutOperationType.ForceUpdateEntity : PutOperationType.UpdateEntity;

            return this.httpClientHelper.PutAsync(GetRequestUri(device.Id), device, operationType, errorMappingOverrides, cancellationToken);
        }

        public override Task<string[]> UpdateDevicesAsync(IEnumerable<Device> devices)
        {
            return this.UpdateDevicesAsync(devices, false, CancellationToken.None);
        }

        public override Task<string[]> UpdateDevicesAsync(IEnumerable<Device> devices, bool forceUpdate, CancellationToken cancellationToken)
        {
            if (devices == null)
            {
                throw new ArgumentNullException("devices");
            }

            if (!devices.Any())
            {
                throw new ArgumentException("devices");
            }

            var exportImportDeviceList = new List<ExportImportDevice>(devices.Count());
            foreach (var device in devices)
            {
                ValidateDeviceId(device);

                if (string.IsNullOrWhiteSpace(device.ETag) && !forceUpdate)
                {
                    throw new ArgumentException(ApiResources.ETagNotSetWhileUpdatingDevice);
                }

                var exportImportDevice = new ExportImportDevice(device, forceUpdate ? ImportMode.Update : ImportMode.UpdateIfMatchETag);
                exportImportDeviceList.Add(exportImportDevice);
            }

            return this.BulkDeviceOperationsAsync(exportImportDeviceList, cancellationToken);
        }

        public override Task RemoveDeviceAsync(string deviceId)
        {
            return this.RemoveDeviceAsync(deviceId, CancellationToken.None);
        }
        
        public override Task RemoveDeviceAsync(string deviceId, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            if (string.IsNullOrWhiteSpace(deviceId))
            {
                throw new ArgumentException(IotHubApiResources.GetString(ApiResources.ParameterCannotBeNullOrWhitespace, "deviceId"));
            }

            // use wildcard etag
            var eTag = new ETagHolder { ETag = "*" };
            return this.RemoveDeviceAsync(deviceId, eTag, cancellationToken);
        }

        public override Task RemoveDeviceAsync(Device device)
        {
            return this.RemoveDeviceAsync(device, CancellationToken.None);
        }

        public override Task RemoveDeviceAsync(Device device, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            ValidateDeviceId(device);

            if (string.IsNullOrWhiteSpace(device.ETag))
            {
                throw new ArgumentException(ApiResources.ETagNotSetWhileDeletingDevice);
            }

            return this.RemoveDeviceAsync(device.Id, device, cancellationToken);
        }

        public override Task<string[]> RemoveDevicesAsync(IEnumerable<Device> devices)
        {
            return this.RemoveDevicesAsync(devices, false, CancellationToken.None);
        }

        public override Task<string[]> RemoveDevicesAsync(IEnumerable<Device> devices, bool forceDelete, CancellationToken cancellationToken)
        {
            if (devices == null)
            {
                throw new ArgumentNullException("devices");
            }

            if (!devices.Any())
            {
                throw new ArgumentException("devices");
            }

            var exportImportDeviceList = new List<ExportImportDevice>(devices.Count());
            foreach (var device in devices)
            {
                ValidateDeviceId(device);

                if (string.IsNullOrWhiteSpace(device.ETag) && !forceDelete)
                {
                    throw new ArgumentException(ApiResources.ETagNotSetWhileDeletingDevice);
                }

                var exportImportDevice = new ExportImportDevice(device, forceDelete ? ImportMode.Delete : ImportMode.DeleteIfMatchETag);
                exportImportDeviceList.Add(exportImportDevice);
            }

            return this.BulkDeviceOperationsAsync(exportImportDeviceList, cancellationToken);
        }

        public override Task<RegistryStatistics> GetRegistryStatisticsAsync()
        {
            return this.GetRegistryStatisticsAsync(CancellationToken.None);
        }

        public override Task<RegistryStatistics> GetRegistryStatisticsAsync(CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();
            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.NotFound, responseMessage => Task.FromResult((Exception)new IotHubNotFoundException(this.iotHubName)));
            return this.httpClientHelper.GetAsync<RegistryStatistics>(GetStatisticsUri(), errorMappingOverrides, null, cancellationToken);
        }

        public override Task<Device> GetDeviceAsync(string deviceId)
        {
            return this.GetDeviceAsync(deviceId, CancellationToken.None);
        }

        public override Task<Device> GetDeviceAsync(string deviceId, CancellationToken cancellationToken)
        {
            if (string.IsNullOrWhiteSpace(deviceId))
            {
                throw new ArgumentException(IotHubApiResources.GetString(ApiResources.ParameterCannotBeNullOrWhitespace, "deviceId"));
            }

            this.EnsureInstanceNotClosed();
            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.NotFound, async responseMessage =>
            {
                return (Exception)new DeviceNotFoundException(await ExceptionHandlingHelper.GetExceptionMessageAsync(responseMessage));
            });
            return this.httpClientHelper.GetAsync<Device>(GetRequestUri(deviceId), errorMappingOverrides, null, false, cancellationToken);
        }

        public override Task<IEnumerable<Device>> GetDevicesAsync(int maxCount)
        {
            return this.GetDevicesAsync(maxCount, CancellationToken.None);
        }

        public override Task<IEnumerable<Device>> GetDevicesAsync(int maxCount, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            return this.httpClientHelper.GetAsync<IEnumerable<Device>>(
                GetDevicesRequestUri(maxCount),
                null,
                null,
                cancellationToken);
        }
        
        /// <inheritdoc/>
        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Dispose resources
        /// </summary>
        /// <param name="disposing">
        /// Governs disposable of managed and native resources
        /// </param>
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (this.httpClientHelper != null)
                {
                    this.httpClientHelper.Dispose();
                    this.httpClientHelper = null;
                }
            }
        }

        Task<string[]> BulkDeviceOperationsAsync(IEnumerable<ExportImportDevice> devices, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            if (devices == null)
            {
                throw new ArgumentNullException("devices");
            }

            foreach (var device in devices)
            {
                ValidateDeviceAuthentication(device.Authentication, device.Id);

                // auto generate keys if not specified
                if (device.Authentication == null)
                {
                    device.Authentication = new AuthenticationMechanism();
                }
            }

            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.RequestEntityTooLarge, async (responseMessage) => new TooManyDevicesException(await ExceptionHandlingHelper.GetExceptionMessageAsync(responseMessage)));
            errorMappingOverrides.Add(HttpStatusCode.BadRequest, async (responseMessage) => new ArgumentException(await ExceptionHandlingHelper.GetExceptionMessageAsync(responseMessage)));

            return this.httpClientHelper.PostAsync<IEnumerable<ExportImportDevice>, string[]>(GetRequestUri(string.Empty), devices, errorMappingOverrides, null, cancellationToken);
        }

        internal override Task ExportRegistryAsync(string storageAccountConnectionString, string containerName)
        {
            return this.ExportRegistryAsync(storageAccountConnectionString, containerName, CancellationToken.None);
        }

        internal override Task ExportRegistryAsync(string storageAccountConnectionString, string containerName, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();
            
            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.NotFound, responseMessage => Task.FromResult((Exception)new IotHubNotFoundException(this.iotHubName)));

            return this.httpClientHelper.PostAsync<ExportImportRequest>(
                GetAdminUri("exportRegistry"),
                new ExportImportRequest
                {
                    ContainerName = containerName,
                    StorageConnectionString = storageAccountConnectionString
                },
                errorMappingOverrides,
                null,
                cancellationToken);
        }

        internal override Task ImportRegistryAsync(string storageAccountConnectionString, string containerName)
        {
            return this.ImportRegistryAsync(storageAccountConnectionString, containerName, CancellationToken.None);
        }

        internal override Task ImportRegistryAsync(string storageAccountConnectionString, string containerName, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.NotFound, responseMessage => Task.FromResult((Exception)new IotHubNotFoundException(this.iotHubName)));

            return this.httpClientHelper.PostAsync<ExportImportRequest>(
                GetAdminUri("importRegistry"),
                new ExportImportRequest
                {
                    ContainerName = containerName,
                    StorageConnectionString = storageAccountConnectionString
                },
                errorMappingOverrides,
                null,
                cancellationToken);
        }

        public override Task<JobProperties> GetJobAsync(string jobId)
        {
            return this.GetJobAsync(jobId, CancellationToken.None);
        }

        public override Task<IEnumerable<JobProperties>> GetJobsAsync()
        {
            return this.GetJobsAsync(CancellationToken.None);
        }

        public override Task CancelJobAsync(string jobId)
        {
            return this.CancelJobAsync(jobId, CancellationToken.None);
        }

        public override Task<JobProperties> ExportDevicesAsync(string exportBlobContainerUri, bool excludeKeys)
        {
            return this.ExportDevicesAsync(exportBlobContainerUri, excludeKeys, CancellationToken.None);
        }

        public override Task<JobProperties> ExportDevicesAsync(string exportBlobContainerUri, bool excludeKeys, CancellationToken ct)
        {
            var jobProperties = new JobProperties()
            {
                Type = JobType.ExportDevices,
                OutputBlobContainerUri = exportBlobContainerUri,
                ExcludeKeysInExport = excludeKeys
            };

            return this.CreateJobAsync(jobProperties, ct);
        }

        public override Task<JobProperties> ImportDevicesAsync(string importBlobContainerUri, string outputBlobContainerUri)
        {
            return this.ImportDevicesAsync(importBlobContainerUri, outputBlobContainerUri, CancellationToken.None);
        }

        public override Task<JobProperties> ImportDevicesAsync(string importBlobContainerUri, string outputBlobContainerUri, CancellationToken ct)
        {
            var jobProperties = new JobProperties()
            {
                Type = JobType.ImportDevices,
                InputBlobContainerUri = importBlobContainerUri,
                OutputBlobContainerUri = outputBlobContainerUri
            };

            return this.CreateJobAsync(jobProperties, ct);
        }
        
        Task<JobProperties> CreateJobAsync(JobProperties jobProperties, CancellationToken ct)
        {
            this.EnsureInstanceNotClosed();

            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>()
            {
                { HttpStatusCode.Forbidden, responseMessage => Task.FromResult((Exception) new JobQuotaExceededException())}
            };

            return this.httpClientHelper.PostAsync<JobProperties, JobProperties>(
                GetJobUri("/create"),
                jobProperties,
                errorMappingOverrides,
                null,
                ct);
        }

        public override Task<JobProperties> GetJobAsync(string jobId, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();
            
            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.NotFound, responseMessage => Task.FromResult((Exception)new JobNotFoundException(jobId)));

            return this.httpClientHelper.GetAsync<JobProperties>(
                GetJobUri("/{0}".FormatInvariant(jobId)),
                errorMappingOverrides,
                null,
                cancellationToken);
        }

        public override Task<IEnumerable<JobProperties>> GetJobsAsync(CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            return this.httpClientHelper.GetAsync<IEnumerable<JobProperties>>(
                GetJobUri(string.Empty),
                null,
                null,
                cancellationToken);
        }

        public override Task CancelJobAsync(string jobId, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.NotFound, responseMessage => Task.FromResult((Exception)new JobNotFoundException(jobId)));

            IETagHolder jobETag = new ETagHolder()
            {
                ETag = jobId
            };
            return this.httpClientHelper.DeleteAsync(
                GetJobUri("/{0}".FormatInvariant(jobId)),
                jobETag,
                errorMappingOverrides,
                null,
                cancellationToken);
        }
        
        Task RemoveDeviceAsync(string deviceId, IETagHolder eTagHolder, CancellationToken cancellationToken)
        {
            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.NotFound, async responseMessage =>
            {                
                var responseContent = await ExceptionHandlingHelper.GetExceptionMessageAsync(responseMessage);
                return (Exception) new DeviceNotFoundException(responseContent, (Exception) null);
            });
            errorMappingOverrides.Add(HttpStatusCode.PreconditionFailed, async (responseMessage) => new PreconditionFailedException(await ExceptionHandlingHelper.GetExceptionMessageAsync(responseMessage)));
            return this.httpClientHelper.DeleteAsync(GetRequestUri(deviceId), eTagHolder, errorMappingOverrides, null, cancellationToken);
        }

        static Uri GetRequestUri(string deviceId)
        {
            deviceId = WebUtility.UrlEncode(deviceId);
            return new Uri(RequestUriFormat.FormatInvariant(deviceId, ClientApiVersionHelper.ApiVersionQueryString), UriKind.Relative);
        }

        static Uri GetJobUri(string jobId)
        {
            return new Uri(JobsUriFormat.FormatInvariant(jobId, ClientApiVersionHelper.ApiVersionQueryString), UriKind.Relative);
        }

        static Uri GetDevicesRequestUri(int maxCount)
        {
            return new Uri(DevicesRequestUriFormat.FormatInvariant(maxCount, ClientApiVersionHelper.ApiVersionQueryString), UriKind.Relative);
        }

        static Uri GetAdminUri(string operation)
        {
            return new Uri(AdminUriFormat.FormatInvariant(operation, ClientApiVersionHelper.ApiVersionQueryString), UriKind.Relative);
        }

        static Uri GetStatisticsUri()
        {
            return new Uri(StatisticsUriFormat, UriKind.Relative);
        }

        static void ValidateDeviceId(Device device)
        {
            if (device == null)
            {
                throw new ArgumentNullException("device");
            }

            if (string.IsNullOrWhiteSpace(device.Id))
            {
                throw new ArgumentException("device.Id");
            }

            if (!DeviceIdRegex.IsMatch(device.Id))
            {
                throw new ArgumentException(ApiResources.DeviceIdInvalid.FormatInvariant(device.Id));
            }
        }

        public static bool ValidateDeviceAuthentication(AuthenticationMechanism authentication, string deviceId)
        {
            if (authentication != null)
            {
                // Both symmetric keys and X.509 cert thumbprints cannot be specified for the same device
                bool symmetricKeyIsSet = !authentication.SymmetricKey?.IsEmpty() ?? false;
                bool x509ThumbprintIsSet = !authentication.X509Thumbprint?.IsEmpty() ?? false;

                if (symmetricKeyIsSet && x509ThumbprintIsSet)
                {
                    throw new ArgumentException(ApiResources.DeviceAuthenticationInvalid.FormatInvariant(deviceId ?? string.Empty));
                }

                // Validate X.509 thumbprints or SymmetricKeys since we should not have both at the same time
                if (x509ThumbprintIsSet)
                {
                    return authentication.X509Thumbprint.IsValid(true);
                }
                else if (symmetricKeyIsSet)
                {
                    return authentication.SymmetricKey.IsValid(true);
                }
            }

            // return true even if authentication is null
            return true;
        }

        void EnsureInstanceNotClosed()
        {
            if (this.httpClientHelper == null)
            {
                throw new ObjectDisposedException("RegistryManager", ApiResources.RegistryManagerInstanceAlreadyClosed);
            }
        }
    }
}
