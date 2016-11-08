// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Query = require('./query');
import Device = require('./device');
import RestApiClient = require('./rest_api_client');

declare class Registry {
    constructor(config: Registry.TransportConfig, restApiClient: RestApiClient);

    // CRUD operations
    create(deviceInfo: { deviceId: string; }, done: Registry.DeviceCallback): void;
    update(deviceInfo: { deviceId: string; }, done: Registry.DeviceCallback): void;
    get(deviceId: string, done: Registry.DeviceCallback): void;
    list(done: (err: Error, devices?: Device[], response?: any) => void): void;
    delete(deviceId: string, done: Registry.ResponseCallback): void;

    // Bulk Import/Export Jobs
    importDevicesFromBlob(inputBlobContainerUri: string, outputBlobContainerUri: string, done: Registry.JobCallback): void;
    exportDevicesToBlob(outputBlobContainerUri: string, excludeKeys: boolean, done: Registry.JobCallback): void;
    listJobs(done: (err: Error, jobsList?: string[]) => void): void;
    getJob(jobId: string, done: Registry.JobCallback): void;
    cancelJob(jobId: string, done: Registry.JobCallback): void;

    // Twin
    getTwin(deviceId: string, done: Registry.ResponseCallback): void;
    updateTwin(deviceId: string, patch: any, etag: string, done: Registry.ResponseCallback): void;

    // Queries
    createQuery(sqlQuery: string, pageSize?: number): Query;

    // Factory methods
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

    interface QueryDescription {
        query: string;
    }

    type DeviceCallback = (err: Error, device: Device) => void;
    type ResponseCallback = (err: Error, device: any, response: any) => void;
    type JobCallback = (err: Error, jobStatus?: JobStatus) => void;
}

export = Registry;
