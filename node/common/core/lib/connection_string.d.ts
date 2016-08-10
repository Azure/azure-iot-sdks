// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// While it doesn't have any parameters that WILL be there,
// there are some that are expected to be there beacuse of the Azure connection string format
declare interface ConnectionString {
    HostName?: string;
    DeviceId?: string;
    SharedAccessKey?: string;
    GatewayHostName?: string;
}

declare namespace ConnectionString {
    export function parse(source: string, requiredFields?: string[]): ConnectionString;
}

export = ConnectionString;
