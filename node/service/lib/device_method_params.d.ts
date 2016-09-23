// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

declare interface DeviceMethodParams {
    name: string;
    payload?: any;
    timeoutInSeconds?: number;
}

export = DeviceMethodParams;