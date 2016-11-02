// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import Client = require('../client');

declare class DeviceMethodResponse implements Client.DeviceMethodResponse {
  constructor(
    requestId: string,
    transport: Client.Transport 
  );

  requestId: string;
  isResponseComplete: boolean;
  status: number;
  payload: any;

  send(status: number, payload?: any, done?: (err?: Error) => void): void;
}

export = DeviceMethodResponse;
