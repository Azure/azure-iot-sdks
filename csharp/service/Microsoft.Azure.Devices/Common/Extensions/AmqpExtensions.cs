// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Extensions
{
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;

    /// <summary>
    /// Contains extension methods for amqp
    /// </summary>
    internal static class AmqpExtensions
    {
        /// <summary>
        /// Get the receiving link
        /// </summary>
        /// <param name="faultTolerantReceivingLink"></param>
        /// <returns></returns>
        internal static async Task<ReceivingAmqpLink> GetReceivingLinkAsync(this FaultTolerantAmqpObject<ReceivingAmqpLink> faultTolerantReceivingLink)
        {
            ReceivingAmqpLink receivingLink;
            if (!faultTolerantReceivingLink.TryGetOpenedObject(out receivingLink))
            {
                receivingLink = await faultTolerantReceivingLink.GetOrCreateAsync(IotHubConnection.DefaultOpenTimeout);
            }

            return receivingLink;
        }
    }
}
