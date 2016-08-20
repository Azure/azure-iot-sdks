// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Registry = require('./registry');
import Device = require('./device');

declare class Http implements Registry.Transport {
    constructor(config: Registry.TransportConfig);
    createDevice(path: string, deviceInfo: { deviceId: string }, done: Registry.ResponseCallback): void;
    updateDevice(path: string, deviceInfo: { deviceId: string }, done: Registry.ResponseCallback): void;
    listDevices(path: string, done: (err: Error, devices?: Device[], response?: any) => void): void;
    deleteDevice(path: string, done: Registry.ResponseCallback): void;
    importDevicesFromBlob(path: string, importRequest: {}, done: Registry.JobCallback): void;
    exportDevicesToBlob(path: string, exportRequest: {}, done: Registry.JobCallback): void;
    listJobs(path: string, done: (err: Error, jobsList?: string[]) => void): void;
    getJob(path: string, done: Registry.JobCallback): void;
    cancelJob(path: string, done: Registry.JobCallback): void;
}

export = Http;
