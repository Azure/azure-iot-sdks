﻿// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//
// This file is NOT generated
//
namespace Microsoft.Azure.Devices.Client.Common
{
    using System;
    using System.Threading.Tasks;
    using Windows.Foundation;
    using System.Linq;
    using Windows.ApplicationModel.Resources.Core;

    class ResourceManagerImpl
    {
        private readonly ResourceMap stringResourceMap;
        private readonly ResourceContext resourceContext;
        public ResourceManagerImpl()
        {
            stringResourceMap = Windows.ApplicationModel.Resources.Core.ResourceManager.Current.MainResourceMap.GetSubtree("Microsoft.Azure.Devices.Client/Resources");
            resourceContext = Windows.ApplicationModel.Resources.Core.ResourceContext.GetForCurrentView();
        }

        public string GetString(string name, System.Globalization.CultureInfo culture)
        {
            var value = stringResourceMap.GetValue(name, resourceContext).ValueAsString;
            return ValidateNotEmpty(value);
        }
        private static string ValidateNotEmpty(string value)
        {
            if (string.IsNullOrEmpty(value))
            {
                throw new NotImplementedException("Resource string not found. Did you add the string to Resources.resw?");
            }
            return value;
        }
    }

    class Resources
    {
        private static readonly ResourceManagerImpl ResourceManager = new ResourceManagerImpl();
        private static global::System.Globalization.CultureInfo resourceCulture = null; // unused

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
        ///   Looks up a localized string similar to {0} If you contact a support representative please include this correlation identifier: {1}, timestamp: {2:u}, errorcode: DG{3}..
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
    namespace Api
    {
        internal class ApiResources
        {
            private static readonly ResourceManagerImpl ResourceManager = new ResourceManagerImpl();
            private static global::System.Globalization.CultureInfo resourceCulture = null; // unused

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
        }
    }

    }
