// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Client = require('../client');

declare class DeviceMethodRequest implements Client.DeviceMethodRequest {
  constructor(
    requestId: string,
    methodName: string,
    body?: Buffer
  );

  requestId: string;
  methodName: string;
  payload: any;
}

export = DeviceMethodRequest;
