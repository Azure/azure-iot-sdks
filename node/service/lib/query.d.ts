// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Registry = require('./registry');
import DeviceTwin = require('./twin');

declare class Query {
    sql: string;
    pageSize: any;
    hasMoreResults: boolean;

    constructor(sqlQuery: string, pageSize: Number, registry: Registry);
    next(done?: (err: Error, results: any, response: any) => void): void;
    nextAsTwin(done?: (err: Error, results: DeviceTwin[], response: any) => void): void;
    toJSON(): Object;
}

export = Query;
