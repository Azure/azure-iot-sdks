// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Http } from 'azure-iot-http-base';
import Query = require('./query');

declare class Registry {
    constructor(config: Registry.TransportConfig, httpRequestBuilder: Http);

    // CRUD operations
    create(deviceInfo: { deviceId: string; }, done: Registry.ResponseCallback): void;
    update(deviceInfo: { deviceId: string; }, done: Registry.ResponseCallback): void;
    get(deviceId: string, done: Registry.ResponseCallback): void;
    list(done: (err: Error, devices?: any[], response?: any) => void): void;
    delete(deviceId: string, done: Registry.ResponseCallback): void;

    // Bulk Import/Export Jobs
    importDevicesFromBlob(inputBlobContainerUri: string, outputBlobContainerUri: string, done: Registry.JobCallback): void;
    exportDevicesToBlob(outputBlobContainerUri: string, excludeKeys: boolean, done: Registry.JobCallback): void;
    listJobs(done: (err: Error, jobsList?: string[]) => void): void;
    getJob(jobId: string, done: Registry.JobCallback): void;
    cancelJob(jobId: string, done: Registry.JobCallback): void;

    // Device Twin
    getDeviceTwin(deviceId: string, done: Registry.ResponseCallback): void;
    updateDeviceTwin(deviceId: string, patch: any, etag: string, done: Registry.ResponseCallback): void;

    // Queries
    createQuery(sqlQuery: string, pageSize: number) : Query;
    executeQuery(query: Registry.QueryDescription, done: (err: Error, result: any, response: any) => void): void;

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
        sql: string;
        pageSize: number;
        continuationToken: string;
    }

    type ResponseCallback = (err: Error, device: any, response: any) => void;
    type JobCallback = (err: Error, jobStatus?: JobStatus) => void;
}

export = Registry;