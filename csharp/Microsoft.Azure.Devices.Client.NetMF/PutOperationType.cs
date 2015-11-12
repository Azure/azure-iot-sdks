// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    enum PutOperationType
    {
        /// <summary>
        /// The PUT operation will create a new entity
        /// </summary>
        CreateEntity,

        /// <summary>
        /// The PUT operation will update (replace) an existing entity as per ETag rules
        /// </summary>
        UpdateEntity,

        /// <summary>
        /// The PUT operation will update (replace) an existing entity ignoring ETag rules
        /// </summary>
        ForceUpdateEntity
    }
}
