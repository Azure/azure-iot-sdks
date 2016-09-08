// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Http } from 'azure-iot-http-base';

declare class Registry {
    constructor(config: Registry.TransportConfig, httpRequestBuilder: Http);
    create(deviceInfo: { deviceId: string; }, done: Registry.ResponseCallback): void;
    update(deviceInfo: { deviceId: string; }, done: Registry.ResponseCallback): void;
    get(deviceId: string, done: Registry.ResponseCallback): void;
    list(done: (err: Error, devices?: any[], response?: any) => void): void;
    delete(deviceId: string, done: Registry.ResponseCallback): void;
    importDevicesFromBlob(inputBlobContainerUri: string, outputBlobContainerUri: string, done: Registry.JobCallback): void;
    exportDevicesToBlob(outputBlobContainerUri: string, excludeKeys: boolean, done: Registry.JobCallback): void;
    listJobs(done: (err: Error, jobsList?: string[]) => void): void;
    getJob(jobId: string, done: Registry.JobCallback): void;
    cancelJob(jobId: string, done: Registry.JobCallback): void;
    getDeviceTwin(deviceId: string, done: Registry.ResponseCallback): void;

    static fromConnectionString(value: string): Registry;
    static fromSharedAccessSignature(value: string): Registry;

}

declare namespace Registry {
    interface TransportConfig {
        host: string;
        sharedAccessSignature: string;
    }

    interface JobStatus {
    }

    type ResponseCallback = (err: Error, device: any, response: any) => void;
    type JobCallback = (err: Error, jobStatus?: JobStatus) => void;
}

export = Registry;