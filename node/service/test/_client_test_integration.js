// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var transportSpecificTests = require('./_client_common_testrun.js');

describe('Over real AMQP', function () {
  this.timeout(60000);
  var opts = {
    transport: null,
    connectionString: process.env.IOTHUB_CONNECTION_STRING,
    id: process.env.IOTHUB_DEVICE_ID
  };
  transportSpecificTests(opts);
});
