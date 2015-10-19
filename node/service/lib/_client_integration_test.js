// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var transportSpecificTests = require('./_client_test.js');
var RealTransport = require('./transport.js');

describe('Over real AMQP', function () {
  this.timeout(10000);
  var opts = {
    transport: null,
    connectionString: process.env.IOTHUB_CONNECTION_STRING,
    id: process.env.IOTHUB_DEVICE_ID
  };
  transportSpecificTests(opts);
});
