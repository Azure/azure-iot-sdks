// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import RestApiClient = require('./rest_api_client');
import DeviceMethodParams = require('./device_method_params');

declare class DeviceMethod {
    params: DeviceMethodParams;

    constructor(params: DeviceMethodParams, restApiClient?: RestApiClient);
    invokeOn(deviceId: string, done?: DeviceMethod.ResponseCallback): void;
}

declare namespace DeviceMethod {
    type ResponseCallback = (err: Error, device: any, response: any) => void;
}

export = DeviceMethod;
