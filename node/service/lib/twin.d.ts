// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Registry = require('./registry');

declare class Twin {
    constructor(device: string | { deviceId: string; }, registryClient: Registry);
    get(done: Registry.ResponseCallback): void;
    update(patch: any, done: Registry.ResponseCallback): void;
}

export = Twin;
