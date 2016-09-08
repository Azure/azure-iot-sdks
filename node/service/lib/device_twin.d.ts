// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Registry = require('./registry');

declare class DeviceTwin {
    constructor(device: string|{ deviceId: string; }, registryClient: Registry);
    get(done: Registry.ResponseCallback): void;
    tags: DeviceTwin.DeviceTwinTags;
}

declare namespace DeviceTwin {
    interface DeviceTwinTags {
      replace(newTags: any, force: boolean, done: Registry.ResponseCallback): void;
    }
}

export = DeviceTwin;
