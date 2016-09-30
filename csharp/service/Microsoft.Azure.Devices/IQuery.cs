// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Collections.Generic;
    using System.Threading.Tasks;

    /// <summary>
    ///     This wraps underlying paged query result access logic.
    ///     Use pattern:
    /// 
    ///     IQuery q = await registryManager.CreateQuery(sql, pagesize);
    ///     while (q.HasMoreResults) {
    ///         IEnumerable\<Twin\> result = r.GetNextAsTwinAsync();
    ///         // access individual device twin records
    ///     }
    /// </summary>
    public interface IQuery
    {
        /// <summary>
        ///     Indicate if more results can be fetched
        /// </summary>
        bool HasMoreResults { get; }

        Task<IEnumerable<Twin>> GetNextAsTwinAsync();

        Task<IEnumerable<DeviceJob>> GetNextAsDeviceJobAsync();

        Task<IEnumerable<JobResponse>> GetNextAsJobResponseAsync();

        Task<IEnumerable<string>> GetNextAsJsonAsync();
    }
}
