// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { ServerResponse, ClientRequest, IncomingMessage } from 'http';
import { Message, X509 } from 'azure-iot-common';

declare class Http {
    // x509Options might be casted to done function in case it's optional and then done parameter should be null
    buildRequest(
        method: Http.Method,
        path: string,
        httpHeaders: Http.Dictionary<Http.HeaderLike>,
        host: string,
        x509Options: X509 | Http.DoneCallback,
        done?: Http.DoneCallback): ClientRequest;

    toMessage(response: IncomingMessage, body: Message.BufferConvertible): Message;
    parseErrorBody(body: string): { code: string, message: string };
}

declare namespace Http {
    interface Dictionary<T> {
        [key: string]: T;
    }

    // https://nodejs.org/api/http.html#http_message_headers
    type HeaderLike = string | string[] | number;
    type Method = "GET" | "HEAD" | "POST" | "PUT" | "DELETE" | "CONNECT" | "OPTIONS" | "TRACE";

    type DoneCallback = (err: Error, body?: string, response?: ServerResponse) => void;
}

export = Http;
