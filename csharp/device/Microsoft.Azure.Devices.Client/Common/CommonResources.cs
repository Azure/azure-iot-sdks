// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    sealed class CommonResources
#if !PCL
        : Microsoft.Azure.Devices.Client.Common.Resources
#endif
    {
        internal static string GetString(string value, params object[] args)
        {
            if (args != null && args.Length > 0)
            {
                for (int i = 0; i < args.Length; i++)
                {
                    string text = args[i] as string;
                    if (text != null && text.Length > 1024)
                    {
                        args[i] = text.Substring(0, 1021) + "...";
                    }
                }

#if !WINDOWS_UWP && !PCL
                return string.Format(CommonResources.Culture, value, args);
#else
                return string.Format(value, args);
#endif
            }

            return value;
        }

#if PCL
        public static string AsyncCallbackThrewException = "An AsyncCallback threw an exception.";
        public static string AsyncResultCompletedTwice = "The IAsyncResult implementation '{0}' tried to complete a single operation multiple times. This could be caused by an incorrect application of IAsyncResult implementation or other extensibility code, such as an IAsyncResult that returns incorrect CompletedSynchronously values, or invokes the AsyncCallback multiple times.";
        public static string InvalidNullAsyncResult = "A null value was returned from an asynchronous 'Begin' method or passed to an AsyncCallback. Asynchronous 'Begin' implementations must return a non-null IAsyncResult and pass the same IAsyncResult object as the parameter to the AsyncCallback.";
        public static string InvalidAsyncResultImplementation = "An incorrect implementation of the IAsyncResult interface may be returning incorrect values from the CompletedSynchronously property or calling the AsyncCallback more than once. The type {0} could be the incorrect implementation.";
        public static string InvalidAsyncResultImplementationGeneric = "An incorrect implementation of the IAsyncResult interface may be returning incorrect values from the CompletedSynchronously property or calling the AsyncCallback more than once.";
        public static string InvalidAsyncResult = "The AsyncResult object used to end this operation was not the object that was returned when the operation was initiated.";
        public static string AsyncResultAlreadyEnded = "End cannot be called twice on the same AsyncResult.";
        public static string ShipAssertExceptionMessage = "An unexpected failure occurred. Applications should not attempt to handle this error. For diagnostic purposes, this English message is associated with the failure: {0}.";
        public static string BufferIsNotRightSizeForBufferManager = "This buffer cannot be returned to the buffer manager because it is the wrong size.";
        public static string InvalidSemaphoreExit = "Object synchronization method was called from an unsynchronized block of code.";
        public static string TimeoutMustBeNonNegative = "Argument {0} must be a non-negative timeout value. The provided value was {1}.";
        public static string TimeoutMustBePositive = "Argument {0} must be a positive timeout value. The provided value was {1}.";
#endif
    }
}
