// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;
    using Newtonsoft.Json.Linq;

    /// <summary>
    ///     Query on device twins, device twin aggregates and device jobs
    /// </summary>
    class Query : IQuery
    {
        private string continuationToken = string.Empty;
        private bool newQuery = true;
        private readonly Func<string, Task<QueryResult>> queryTaskFunc;

        /// <summary>
        ///     internal ctor
        /// </summary>
        /// <param name="pageSize"></param>
        /// <param name="queryTaskFunc"></param>
        internal Query(Func<string, Task<QueryResult>> queryTaskFunc)
        {
            this.queryTaskFunc = queryTaskFunc;
        }

        /// <summary>
        ///     return true before any next calls or when a continuation token is present
        /// </summary>
        public bool HasMoreResults => newQuery || !string.IsNullOrEmpty(this.continuationToken);

        /// <summary>
        ///     fetch the next paged result as twins
        /// </summary>
        /// <returns></returns>
        public async Task<IEnumerable<Twin>> GetNextAsTwinAsync()
        {
            return this.HasMoreResults
                ? await GetAndCastNextResultAsync<Twin>(QueryResultType.Twin)
                : new List<Twin>();
        }

        /// <summary>
        ///     fetch the next paged result as device jobs
        /// </summary>
        /// <returns></returns>
        public async Task<IEnumerable<DeviceJob>> GetNextAsDeviceJobAsync()
        {
            return this.HasMoreResults
                ? await GetAndCastNextResultAsync<DeviceJob>(QueryResultType.DeviceJob)
                : new List<DeviceJob>();
        }

        /// <summary>
        ///     fetch the next paged result as job responses
        /// </summary>
        /// <returns></returns>
        public async Task<IEnumerable<JobResponse>> GetNextAsJobResponseAsync()
        {
            return this.HasMoreResults
                ? await GetAndCastNextResultAsync<JobResponse>(QueryResultType.JobResponse)
                : new List<JobResponse>();
        }

        /// <summary>
        ///     fetch the next paged result as Json strings
        /// </summary>
        /// <returns></returns>
        public async Task<IEnumerable<string>> GetNextAsJsonAsync()
        {
            if (!this.HasMoreResults)
            {
                return new List<string>();
            }

            QueryResult r = await GetNextAsync();
            return r.Items.Select(o => o.ToString());
        }

        async Task<IEnumerable<T>> GetAndCastNextResultAsync<T>(QueryResultType type)
        {
            QueryResult r = await GetNextAsync();
            return CastResultContent<T>(r, type);
        }

        static IEnumerable<T> CastResultContent<T>(QueryResult result, QueryResultType expected)
        {
            if (result.Type != expected)
            {
                throw new InvalidCastException($"result type is {result.Type}");
            }

            // TODO: optimize this 2nd parse from JObject to target object type T
            return result.Items.Select(o => ((JObject) o).ToObject<T>());
        }

        async Task<QueryResult> GetNextAsync()
        {
            newQuery = false;
            QueryResult result = await queryTaskFunc(continuationToken);
            continuationToken = result.ContinuationToken;
            return result;
        }
    }
}
