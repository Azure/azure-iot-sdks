// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//
// This file is NOT generated
//
namespace Microsoft.Azure.Devices
{
    using System;
    using Windows.ApplicationModel.Resources.Core;

    class ResourceManagerImpl
    {
        private readonly ResourceMap stringResourceMap;
        private readonly ResourceContext resourceContext;
        public ResourceManagerImpl(string subtree)
        {
            /* Discover resources in the image
            foreach (var m in ResourceManager.Current.MainResourceMap)
            {
                System.Diagnostics.Debug.WriteLine(m);
            }
            */
            stringResourceMap = ResourceManager.Current.MainResourceMap.GetSubtree(subtree);
            resourceContext = ResourceContext.GetForCurrentView();
        }

        public string GetString(string name, System.Globalization.CultureInfo culture)
        {
            var value = stringResourceMap.GetValue(name, resourceContext);
            if (value != null)
            {
                string str = value.ValueAsString;
                if (!string.IsNullOrEmpty(str))
                {
                    return str;
                }
            }
            throw new NotImplementedException("Resource string not found. Did you add the string to Resources.resw?");
        }
    }

    /// <summary>
    ///   A strongly-typed resource class, for looking up localized strings, etc.
    /// </summary>
    [global::System.CodeDom.Compiler.GeneratedCodeAttribute("System.Resources.Tools.StronglyTypedResourceBuilder", "4.0.0.0")]
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute()]
    [global::System.Runtime.CompilerServices.CompilerGeneratedAttribute()]
    internal class ApiResources
    {
        private static global::System.Resources.ResourceManager resourceMan;

        private static global::System.Globalization.CultureInfo resourceCulture;

        [global::System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
        internal ApiResources()
        {
        }

        private static readonly ResourceManagerImpl ResourceManager = new ResourceManagerImpl("Microsoft.Azure.Devices.Uwp/ApiResources");

        /// <summary>
        ///   Overrides the current thread's CurrentUICulture property for all
        ///   resource lookups using this strongly typed resource class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Globalization.CultureInfo Culture
        {
            get
            {
                return resourceCulture;
            }
            set
            {
                resourceCulture = value;
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The value of this argument must be non-negative..
        /// </summary>
        internal static string ArgumentMustBeNonNegative
        {
            get
            {
                return ResourceManager.GetString("ArgumentMustBeNonNegative", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The value of this argument must be positive..
        /// </summary>
        internal static string ArgumentMustBePositive
        {
            get
            {
                return ResourceManager.GetString("ArgumentMustBePositive", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The connection string is not well formed..
        /// </summary>
        internal static string ConnectionStringIsNotWellFormed
        {
            get
            {
                return ResourceManager.GetString("ConnectionStringIsNotWellFormed", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Device {0} cannot specify both symmetric keys and thumbprints..
        /// </summary>
        internal static string DeviceAuthenticationInvalid
        {
            get
            {
                return ResourceManager.GetString("DeviceAuthenticationInvalid", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The device identifier {0} is invalid..
        /// </summary>
        internal static string DeviceIdInvalid
        {
            get
            {
                return ResourceManager.GetString("DeviceIdInvalid", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The id of the device was not set..
        /// </summary>
        internal static string DeviceIdNotSet
        {
            get
            {
                return ResourceManager.GetString("DeviceIdNotSet", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Either both primary and secondary keys must be specified or neither one to auto generate on service side..
        /// </summary>
        internal static string DeviceKeysInvalid
        {
            get
            {
                return ResourceManager.GetString("DeviceKeysInvalid", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The ETag should be set while deleting the device..
        /// </summary>
        internal static string ETagNotSetWhileDeletingDevice
        {
            get
            {
                return ResourceManager.GetString("ETagNotSetWhileDeletingDevice", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The ETag should be set while updating the device..
        /// </summary>
        internal static string ETagNotSetWhileUpdatingDevice
        {
            get
            {
                return ResourceManager.GetString("ETagNotSetWhileUpdatingDevice", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The ETag should not be set while registering the device..
        /// </summary>
        internal static string ETagSetWhileRegisteringDevice
        {
            get
            {
                return ResourceManager.GetString("ETagSetWhileRegisteringDevice", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Serialization operation failed due to unsupported type {0}..
        /// </summary>
        internal static string FailedToSerializeUnsupportedType
        {
            get
            {
                return ResourceManager.GetString("FailedToSerializeUnsupportedType", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The HostName is null..
        /// </summary>
        internal static string HostNameIsNull
        {
            get
            {
                return ResourceManager.GetString("HostNameIsNull", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The endpoint in the connection string is invalid..
        /// </summary>
        internal static string InvalidConnectionStringEndpoint
        {
            get
            {
                return ResourceManager.GetString("InvalidConnectionStringEndpoint", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The password is not valid..
        /// </summary>
        internal static string InvalidPassword
        {
            get
            {
                return ResourceManager.GetString("InvalidPassword", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The connection string has an invalid value for property: {0}.
        /// </summary>
        internal static string InvalidPropertyInConnectionString
        {
            get
            {
                return ResourceManager.GetString("InvalidPropertyInConnectionString", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The User is not valid..
        /// </summary>
        internal static string InvalidUser
        {
            get
            {
                return ResourceManager.GetString("InvalidUser", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The message body cannot be read multiple times. To reuse it store the value after reading..
        /// </summary>
        internal static string MessageBodyConsumed
        {
            get
            {
                return ResourceManager.GetString("MessageBodyConsumed", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to This messaging entity has already been closed, aborted, or disposed..
        /// </summary>
        internal static string MessageDisposed
        {
            get
            {
                return ResourceManager.GetString("MessageDisposed", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The connection string is missing the property: {0}.
        /// </summary>
        internal static string MissingPropertyInConnectionString
        {
            get
            {
                return ResourceManager.GetString("MissingPropertyInConnectionString", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The specified offset exceeds the buffer size ({0} bytes)..
        /// </summary>
        internal static string OffsetExceedsBufferSize
        {
            get
            {
                return ResourceManager.GetString("OffsetExceedsBufferSize", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The parameter {0} cannot be null, empty or whitespace..
        /// </summary>
        internal static string ParameterCannotBeNullOrWhitespace
        {
            get
            {
                return ResourceManager.GetString("ParameterCannotBeNullOrWhitespace", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The {0} property is not a valid Uri.
        /// </summary>
        internal static string PropertyIsNotValidUri
        {
            get
            {
                return ResourceManager.GetString("PropertyIsNotValidUri", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The RegistryManager instance was already closed..
        /// </summary>
        internal static string RegistryManagerInstanceAlreadyClosed
        {
            get
            {
                return ResourceManager.GetString("RegistryManagerInstanceAlreadyClosed", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The specified size exceeds the remaining buffer space ({0} bytes)..
        /// </summary>
        internal static string SizeExceedsRemainingBufferSpace
        {
            get
            {
                return ResourceManager.GetString("SizeExceedsRemainingBufferSpace", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to &apos;{0}&apos; is not a valid X.509 thumbprint.
        /// </summary>
        internal static string StringIsNotThumbprint
        {
            get
            {
                return ResourceManager.GetString("StringIsNotThumbprint", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The parameter {0} cannot be null or empty..
        /// </summary>
        internal static string ParameterCannotBeNullOrEmpty
        {
            get
            {
                return ResourceManager.GetString("ParameterCannotBeNullOrEmpty", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The JobClient instance was already closed..
        /// </summary>
        internal static string JobClientInstanceAlreadyClosed
        {
            get
            {
                return ResourceManager.GetString("JobClientInstanceAlreadyClosed", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to null or empty deviceIds.
        /// </summary>
        internal static string DeviceJobParametersNullOrEmptyDeviceList
        {
            get
            {
                return ResourceManager.GetString("DeviceJobParametersNullOrEmptyDeviceList", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to null or empty deviceId entries specified.
        /// </summary>
        internal static string DeviceJobParametersNullOrEmptyDeviceListEntries
        {
            get
            {
                return ResourceManager.GetString("DeviceJobParametersNullOrEmptyDeviceListEntries", resourceCulture);
            }
        }
    }
}

namespace Microsoft.Azure.Devices.Common
{
    /// <summary>
    ///   A strongly-typed resource class, for looking up localized strings, etc.
    /// </summary>
    internal class Resources
    {
        private static global::System.Resources.ResourceManager resourceMan;

        private static global::System.Globalization.CultureInfo resourceCulture;

        [global::System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
        internal Resources()
        {
        }

        private static readonly ResourceManagerImpl ResourceManager = new ResourceManagerImpl("Microsoft.Azure.Devices.Uwp/Resources");

        /// <summary>
        ///   Overrides the current thread's CurrentUICulture property for all
        ///   resource lookups using this strongly typed resource class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Globalization.CultureInfo Culture
        {
            get
            {
                return resourceCulture;
            }
            set
            {
                resourceCulture = value;
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The ActionItem was scheduled for execution but has not completed yet..
        /// </summary>
        internal static string ActionItemIsAlreadyScheduled
        {
            get
            {
                return ResourceManager.GetString("ActionItemIsAlreadyScheduled", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The argument {0} is null or empty..
        /// </summary>
        internal static string ArgumentNullOrEmpty
        {
            get
            {
                return ResourceManager.GetString("ArgumentNullOrEmpty", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The argument {0} is null or white space..
        /// </summary>
        internal static string ArgumentNullOrWhiteSpace
        {
            get
            {
                return ResourceManager.GetString("ArgumentNullOrWhiteSpace", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The value supplied must be between {0} and {1}..
        /// </summary>
        internal static string ArgumentOutOfRange
        {
            get
            {
                return ResourceManager.GetString("ArgumentOutOfRange", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to An AsyncCallback threw an exception..
        /// </summary>
        internal static string AsyncCallbackThrewException
        {
            get
            {
                return ResourceManager.GetString("AsyncCallbackThrewException", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to End cannot be called twice on the same AsyncResult..
        /// </summary>
        internal static string AsyncResultAlreadyEnded
        {
            get
            {
                return ResourceManager.GetString("AsyncResultAlreadyEnded", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The IAsyncResult implementation &apos;{0}&apos; tried to complete a single operation multiple times. This could be caused by an incorrect application of IAsyncResult implementation or other extensibility code, such as an IAsyncResult that returns incorrect CompletedSynchronously values, or invokes the AsyncCallback multiple times..
        /// </summary>
        internal static string AsyncResultCompletedTwice
        {
            get
            {
                return ResourceManager.GetString("AsyncResultCompletedTwice", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to An exception was thrown from a TransactionScope used to flow a transaction into an asynchronous operation..
        /// </summary>
        internal static string AsyncTransactionException
        {
            get
            {
                return ResourceManager.GetString("AsyncTransactionException", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Failed to allocate a managed memory buffer of {0} bytes. The amount of available memory may be low..
        /// </summary>
        internal static string BufferAllocationFailed
        {
            get
            {
                return ResourceManager.GetString("BufferAllocationFailed", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The buffer has already been reclaimed..
        /// </summary>
        internal static string BufferAlreadyReclaimed
        {
            get
            {
                return ResourceManager.GetString("BufferAlreadyReclaimed", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to This buffer cannot be returned to the buffer manager because it is the wrong size..
        /// </summary>
        internal static string BufferIsNotRightSizeForBufferManager
        {
            get
            {
                return ResourceManager.GetString("BufferIsNotRightSizeForBufferManager", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Can&apos;t create session when the connection is closing..
        /// </summary>
        internal static string CreateSessionOnClosingConnection
        {
            get
            {
                return ResourceManager.GetString("CreateSessionOnClosingConnection", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Credit listener is already registered..
        /// </summary>
        internal static string CreditListenerAlreadyRegistered
        {
            get
            {
                return ResourceManager.GetString("CreditListenerAlreadyRegistered", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to A device with ID &apos;{0}&apos; is already registered..
        /// </summary>
        internal static string DeviceAlreadyExists
        {
            get
            {
                return ResourceManager.GetString("DeviceAlreadyExists", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Import error for device ID &apos;{0}&apos;: {1}.
        /// </summary>
        internal static string DeviceImportError
        {
            get
            {
                return ResourceManager.GetString("DeviceImportError", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The primary and secondary keys must be between {0} and {1} bytes in length..
        /// </summary>
        internal static string DeviceKeyLengthInvalid
        {
            get
            {
                return ResourceManager.GetString("DeviceKeyLengthInvalid", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to &quot;Device {0} at IotHub {1} not registered&quot;.
        /// </summary>
        internal static string DeviceNotFound
        {
            get
            {
                return ResourceManager.GetString("DeviceNotFound", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to A device with ID &apos;{0}&apos; is not registered..
        /// </summary>
        internal static string DeviceNotRegistered
        {
            get
            {
                return ResourceManager.GetString("DeviceNotRegistered", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Total number of devices on IotHub &apos;{0}&apos; exceeded the allocated quota. Max allowed device count : &apos;{1}&apos;, current device count : &apos;{2}&apos;. Consider increasing the units for this hub to increase the quota..
        /// </summary>
        internal static string DeviceQuotaExceededExceptionMessage
        {
            get
            {
                return ResourceManager.GetString("DeviceQuotaExceededExceptionMessage", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Disposition listener is already registered..
        /// </summary>
        internal static string DispositionListenerAlreadyRegistered
        {
            get
            {
                return ResourceManager.GetString("DispositionListenerAlreadyRegistered", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Not supported when the disposition listener is set..
        /// </summary>
        internal static string DispositionListenerSetNotSupported
        {
            get
            {
                return ResourceManager.GetString("DispositionListenerSetNotSupported", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Failed to convert to a char..
        /// </summary>
        internal static string ErrorConvertingToChar
        {
            get
            {
                return ResourceManager.GetString("ErrorConvertingToChar", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to ETag mismatch for device ID &apos;{0}&apos;.
        /// </summary>
        internal static string ETagMismatch
        {
            get
            {
                return ResourceManager.GetString("ETagMismatch", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to An unrecoverable error occurred. For diagnostic purposes, this English message is associated with the failure: &apos;{0}&apos;..
        /// </summary>
        internal static string FailFastMessage
        {
            get
            {
                return ResourceManager.GetString("FailFastMessage", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Internal server error..
        /// </summary>
        internal static string InternalServerError
        {
            get
            {
                return ResourceManager.GetString("InternalServerError", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The AsyncResult object used to end this operation was not the object that was returned when the operation was initiated..
        /// </summary>
        internal static string InvalidAsyncResult
        {
            get
            {
                return ResourceManager.GetString("InvalidAsyncResult", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to An incorrect implementation of the IAsyncResult interface may be returning incorrect values from the CompletedSynchronously property or calling the AsyncCallback more than once. The type {0} could be the incorrect implementation..
        /// </summary>
        internal static string InvalidAsyncResultImplementation
        {
            get
            {
                return ResourceManager.GetString("InvalidAsyncResultImplementation", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to An incorrect implementation of the IAsyncResult interface may be returning incorrect values from the CompletedSynchronously property or calling the AsyncCallback more than once..
        /// </summary>
        internal static string InvalidAsyncResultImplementationGeneric
        {
            get
            {
                return ResourceManager.GetString("InvalidAsyncResultImplementationGeneric", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The buffer size is invalid..
        /// </summary>
        internal static string InvalidBufferSize
        {
            get
            {
                return ResourceManager.GetString("InvalidBufferSize", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Invalid device format in line: {0}.
        /// </summary>
        internal static string InvalidDeviceLine
        {
            get
            {
                return ResourceManager.GetString("InvalidDeviceLine", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Invalid frame size:{0}, maximum frame size:{1}..
        /// </summary>
        internal static string InvalidFrameSize
        {
            get
            {
                return ResourceManager.GetString("InvalidFrameSize", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to A null value was returned from an asynchronous &apos;Begin&apos; method or passed to an AsyncCallback. Asynchronous &apos;Begin&apos; implementations must return a non-null IAsyncResult and pass the same IAsyncResult object as the parameter to the AsyncCallback..
        /// </summary>
        internal static string InvalidNullAsyncResult
        {
            get
            {
                return ResourceManager.GetString("InvalidNullAsyncResult", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Object synchronization method was called from an unsynchronized block of code..
        /// </summary>
        internal static string InvalidSemaphoreExit
        {
            get
            {
                return ResourceManager.GetString("InvalidSemaphoreExit", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to IOThreadTimer cannot accept Timespan.MaxValue..
        /// </summary>
        internal static string IOThreadTimerCannotAcceptMaxTimeSpan
        {
            get
            {
                return ResourceManager.GetString("IOThreadTimerCannotAcceptMaxTimeSpan", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to IotHub &apos;{0}&apos; is suspended.
        /// </summary>
        internal static string IotHubSuspendedException
        {
            get
            {
                return ResourceManager.GetString("IotHubSuspendedException", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Message listener is already registered..
        /// </summary>
        internal static string MessageListenerAlreadyRegistered
        {
            get
            {
                return ResourceManager.GetString("MessageListenerAlreadyRegistered", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Total number of messages on IotHub &apos;{0}&apos; exceeded the allocated quota. Max allowed message count : &apos;{1}&apos;, current message count : &apos;{2}&apos;. Send and Receive operations are blocked for this hub until the next UTC day. Consider increasing the units for this hub to increase the quota..
        /// </summary>
        internal static string MessageQuotaExceededExceptionMessage
        {
            get
            {
                return ResourceManager.GetString("MessageQuotaExceededExceptionMessage", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Unsupported type {0} for array..
        /// </summary>
        internal static string NotSupportFrameCode
        {
            get
            {
                return ResourceManager.GetString("NotSupportFrameCode", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to This object is read-only and cannot be modified..
        /// </summary>
        internal static string ObjectIsReadOnly
        {
            get
            {
                return ResourceManager.GetString("ObjectIsReadOnly", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The operation timed out..
        /// </summary>
        internal static string OperationTimedOut
        {
            get
            {
                return ResourceManager.GetString("OperationTimedOut", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Service certificate must be set..
        /// </summary>
        internal static string ServerCertificateNotSet
        {
            get
            {
                return ResourceManager.GetString("ServerCertificateNotSet", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to An unexpected failure occurred. Applications should not attempt to handle this error. For diagnostic purposes, this English message is associated with the failure: {0}..
        /// </summary>
        internal static string ShipAssertExceptionMessage
        {
            get
            {
                return ResourceManager.GetString("ShipAssertExceptionMessage", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to &apos;{0}&apos; is not a valid Base64 encoded string..
        /// </summary>
        internal static string StringIsNotBase64
        {
            get
            {
                return ResourceManager.GetString("StringIsNotBase64", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to TargetHost must be set..
        /// </summary>
        internal static string TargetHostNotSet
        {
            get
            {
                return ResourceManager.GetString("TargetHostNotSet", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Argument {0} must be a non-negative timeout value. The provided value was {1}..
        /// </summary>
        internal static string TimeoutMustBeNonNegative
        {
            get
            {
                return ResourceManager.GetString("TimeoutMustBeNonNegative", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to Argument {0} must be a positive timeout value. The provided value was {1}..
        /// </summary>
        internal static string TimeoutMustBePositive
        {
            get
            {
                return ResourceManager.GetString("TimeoutMustBePositive", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to An error occurred..
        /// </summary>
        internal static string UnspecifiedError
        {
            get
            {
                return ResourceManager.GetString("UnspecifiedError", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to {0} If you contact a support representative please include this correlation identifier: {1}, timestamp: {2:u}, errorcode: IH{3}..
        /// </summary>
        internal static string UserErrorFormat
        {
            get
            {
                return ResourceManager.GetString("UserErrorFormat", resourceCulture);
            }
        }

        /// <summary>
        ///   Looks up a localized string similar to The value of this argument must be non-negative..
        /// </summary>
        internal static string ValueMustBeNonNegative
        {
            get
            {
                return ResourceManager.GetString("ValueMustBeNonNegative", resourceCulture);
            }
        }
    }
}
