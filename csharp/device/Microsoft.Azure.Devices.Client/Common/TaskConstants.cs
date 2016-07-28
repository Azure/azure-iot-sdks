// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
namespace Microsoft.Azure.Devices.Client.Common
{
    using System.Threading.Tasks;

    static class TaskConstants
    {
        static readonly Task<bool> BooleanTrueTask = Task.FromResult(true);

        /// <summary>
        ///     A <see cref="Task" /> that has been completed.
        /// </summary>
        public static Task Completed
        {
            get { return BooleanTrueTask; }
        }
    }
}