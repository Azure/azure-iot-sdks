// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Device = require('./device');

declare class Registry {
    constructor(transport: Registry.Transport);
    create(deviceInfo: { deviceId: string; }, done: Registry.ResponseCallback): void;
    update(deviceInfo: { deviceId: string; }, done: Registry.ResponseCallback): void;
    get(deviceId: string, done: Registry.ResponseCallback): void;
    list(done: (err: Error, devices?: Device[], response?: any) => void): void;
    delete(deviceId: string, done: Registry.ResponseCallback): void;
    importDevicesFromBlob(inputBlobContainerUri: string, outputBlobContainerUri: string, done: Registry.JobCallback): void;
    exportDevicesToBlob(outputBlobContainerUri: string, excludeKeys: boolean, done: Registry.JobCallback): void;
    listJobs(done: (err: Error, jobsList?: string[]) => void): void;
    getJob(jobId: string, done: Registry.JobCallback): void;
    cancelJob(jobId: string, done: Registry.JobCallback): void;

    static fromConnectionString(value: string, transportCtr?: Registry.TransportContructor): Registry;
    static fromSharedAccessSignature(value: string): Registry;

}

declare namespace Registry {
    interface TransportConfig {
        host: string;
        hubName: string;
        sharedAccessSignature: string;
    }
    interface JobStatus {
    }
    interface Transport {
        createDevice(path: string, deviceInfo: { deviceId: string }, done: ResponseCallback): void;
        updateDevice(path: string, deviceInfo: { deviceId: string }, done: ResponseCallback): void;
        listDevices(path: string, done: (err: Error, devices?: Device[], response?: any) => void): void;
        deleteDevice(path: string, done: ResponseCallback): void;
        importDevicesFromBlob(path: string, importRequest: {}, done: JobCallback): void;
        exportDevicesToBlob(path: string, exportRequest: {}, done: JobCallback): void;
        listJobs(path: string, done: (err: Error, jobsList?: string[]) => void): void;
        getJob(path: string, done: JobCallback): void;
        cancelJob(path: string, done: JobCallback): void;
    }

    type ResponseCallback = (err: Error, device: Device, response: any) => void;
    type JobCallback = (err: Error, jobStatus?: JobStatus) => void;
    type TransportContructor = new (config: TransportConfig) => Transport;
}

export = Registry;
