// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Http } from 'azure-iot-http-base';

declare class RestApiClient {
    constructor(config: RestApiClient.TransportConfig, httpRequestBuilder: Http);
    executeApiCall(method: string, path: string, headers: any, body: any, timeout?: number, done?: RestApiClient.ResponseCallback): void;
    updateSharedAccessSignature(sharedAccessSignature: string): void;

    static translateError(result: any, response: any): Error;
}

declare namespace RestApiClient {
    interface TransportConfig {
        host: string;
        sharedAccessSignature: string;
    }

    type ResponseCallback = (err: Error, device: any, response: any) => void;
}

export = RestApiClient;
