// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import RestApiClient = require('./rest_api_client');

declare class DeviceMethod {
    name: string;
    payload: any;
    timeout: number;

    constructor(methodName: string, timeoutInSeconds: Number, restApiClient: RestApiClient);
    invokeOn(deviceId: string, payload?: any, done?: DeviceMethod.ResponseCallback): void;
}

declare namespace DeviceMethod {
    type ResponseCallback = (err: Error, device: any, response: any) => void;
}

export = DeviceMethod;