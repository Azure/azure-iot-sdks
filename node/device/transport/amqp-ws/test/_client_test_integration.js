// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Registry = require('azure-iothub').Registry;
var AmqpWs = require('../lib/amqp_ws.js');
var clientTests = require('azure-iot-device/test/_client_common_testrun.js');


describe('Over AMQP/WS', function () {
  this.timeout(60000);
  var registry = Registry.fromConnectionString(process.env.IOTHUB_CONNECTION_STRING);
  clientTests.sendEventTests(AmqpWs, registry);
});