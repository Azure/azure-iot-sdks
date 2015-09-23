// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Extensions
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq;
    using System.Reflection;

    static class ExceptionExtensions
    {
        const string ExceptionIdentifierName = "ExceptionId";
        static MethodInfo prepForRemotingMethodInfo;
        
        public static bool IsFatal(this Exception exception)
        {
            return Fx.IsFatal(exception);
        }

        public static IEnumerable<Exception> Unwind(this Exception exception)
        {
            while (exception != null)
            {
                yield return exception;
                exception = exception.InnerException;
            }
        }

        public static IEnumerable<Exception> Unwind(this Exception exception, params Type[] targetTypes)
        {
            return exception.Unwind().Where(e => targetTypes.Any(t => t.IsInstanceOfType(e)));
        }

        public static IEnumerable<TException> Unwind<TException>(this Exception exception)
        {
            return exception.Unwind().OfType<TException>();
        }

        public static Exception PrepareForRethrow(this Exception exception)
        {
            Fx.Assert(exception != null, "The specified Exception is null.");

            if (!ShouldPrepareForRethrow(exception))
            {
                return exception;
            }

            if (PartialTrustHelpers.UnsafeIsInFullTrust())
            {
                // Racing here is harmless
                if (ExceptionExtensions.prepForRemotingMethodInfo == null)
                {
#if WINDOWS_UWP // No Type.GetMethod in UWP
                    ExceptionExtensions.prepForRemotingMethodInfo = null;
#else
                    ExceptionExtensions.prepForRemotingMethodInfo =
                        typeof(Exception).GetMethod("PrepForRemoting", BindingFlags.Instance | BindingFlags.NonPublic, null, new Type[] { }, new ParameterModifier[] { });
#endif
                }
                if (ExceptionExtensions.prepForRemotingMethodInfo != null)
                {
                    // PrepForRemoting is not thread-safe. When the same exception instance is thrown by multiple threads
                    // the remote stack trace string may not format correctly. However, We don't lock this to protect us from it given
                    // it is discouraged to throw the same exception instance from multiple threads and the side impact is ignorable.
                    prepForRemotingMethodInfo.Invoke(exception, new object[] { });
                }
            }

            return exception;
        }

        public static Exception DisablePrepareForRethrow(this Exception exception)
        {
            exception.Data[AsyncResult.DisablePrepareForRethrow] = string.Empty;
            return exception;
        }

        public static string ToStringSlim(this Exception exception)
        {
            // exception.Data is empty collection by default.
            if (exception.Data != null && exception.Data.Contains(ExceptionIdentifierName))
            {
                return string.Format(CultureInfo.InvariantCulture,
                    "ExceptionId: {0}-{1}: {2}",
                    exception.Data[ExceptionIdentifierName],
                    exception.GetType(),
                    exception.Message);
            }
            else if (exception.Data != null)
            {
                string exceptionIdentifier = Guid.NewGuid().ToString();
                exception.Data[ExceptionIdentifierName] = exceptionIdentifier;

                return string.Format(CultureInfo.InvariantCulture,
                    "ExceptionId: {0}-{1}",
                    exceptionIdentifier,
                    exception.ToString());
            }

            // In case Data collection in the exception is nullified.
            return exception.ToString();
        }

        public static string GetReferenceCode(this Exception exception)
        {
            if (exception.Data != null && exception.Data.Contains(ExceptionIdentifierName))
            {
                return (string)exception.Data[ExceptionIdentifierName];
            }
            else
            {
                return null;
            }
        }

        static bool ShouldPrepareForRethrow(Exception exception)
        {
            while (exception != null)
            {
                if (exception.Data != null && exception.Data.Contains(AsyncResult.DisablePrepareForRethrow))
                {
                    return false;
                }

                exception = exception.InnerException;
            }

            return true;
        }
    }
}
