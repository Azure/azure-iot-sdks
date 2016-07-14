// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Tests.PCLTests
{
    using System;
    using System.Threading.Tasks;

    using Microsoft.Azure.Devices.Client.Extensions;
    using NUnit.Framework;

    public static class TestAssert
    {
        public static TException Throws<TException>(this Action action, string errorMessage = null) where TException : Exception
        {
            errorMessage = errorMessage?? "Failed";
            try
            {
                action();
            }
            catch (TException ex)
            {
                return ex;
            }
            catch (Exception ex)
            {
                throw new AssertionException(
                    "{0}. Expected:<{1}> Actual<{2}>".FormatInvariant(errorMessage, typeof(TException).ToString(), ex.GetType().ToString()), 
                    ex); 
            }

            throw new AssertionException("{0}. Expected {1} exception but no exception is thrown".FormatInvariant(errorMessage, typeof(TException).ToString()));
        }

        public static TException Throws<TException>(Func<Task> action, string errorMessage = null) where TException : Exception
        {
            return Throws<TException>(() => action().Wait(), errorMessage);
        }

        public static async Task<TException> ThrowsAsync<TException>(this Func<Task> action, string errorMessage = null) where TException : Exception
        {
            errorMessage = errorMessage ?? "Failed";
            try
            {
                await action();
            }
            catch (TException ex)
            {
                return ex;
            }
            catch (Exception ex)
            {
                throw new AssertionException(
                    "{0}. Expected:<{1}> Actual<{2}>".FormatInvariant(errorMessage, typeof(TException).ToString(), ex.GetType().ToString()),
                    ex);
            }

            throw new AssertionException("{0}. Expected {1} exception but no exception is thrown".FormatInvariant(errorMessage, typeof(TException).ToString()));
        }
    }
}