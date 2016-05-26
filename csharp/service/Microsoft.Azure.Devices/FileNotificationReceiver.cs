// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// Contains methods that services can use to perform file notification receive operations.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public abstract class FileNotificationReceiver<T> : Receiver<T> where T : FileNotification
    {
    }
}
