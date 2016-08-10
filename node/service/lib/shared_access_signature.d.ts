// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { SharedAccessSignature } from 'azure-iot-common';

export declare function create(host: string, policy: string, key: string, expiry: string): SharedAccessSignature;
export declare function parse(source: string): SharedAccessSignature;
