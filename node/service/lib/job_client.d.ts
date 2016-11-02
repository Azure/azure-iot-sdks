// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
import RestApiClient = require('./rest_api_client');
import DeviceMethodParams = require('./device_method_params');
import Query = require('./query');

declare class JobClient {
    constructor(restApiClient: RestApiClient);

    getJob(jobId: string, done: JobClient.JobCallback): void;
    createQuery(jobType?: string, jobStatus?: string, pageSize?: Number): Query;
    cancelJob(jobId: string, done: JobClient.JobCallback): void;
    scheduleDeviceMethod(jobId: string,
                         queryCondition: string,
                         methodParams: DeviceMethodParams,
                         jobStartTime?: Date | JobClient.JobCallback,
                         maxExecutionTimeInSeconds?: Number | JobClient.JobCallback,
                         done?: JobClient.JobCallback): void;

    scheduleTwinUpdate(jobId: string,
                       queryCondition: string,
                       patch: Object,
                       jobStartTime?: Date | JobClient.JobCallback,
                       maxExecutionTimeInSeconds?: Number | JobClient.JobCallback,
                       done?: JobClient.JobCallback): void;

    // Factory methods
    static fromConnectionString(value: string): JobClient;
    static fromSharedAccessSignature(value: string): JobClient;
}

declare namespace JobClient {
    type JobCallback = (err: Error, jobStatus?: any, response?: any) => void;
}

export = JobClient;
