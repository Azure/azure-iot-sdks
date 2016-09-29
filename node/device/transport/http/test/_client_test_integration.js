// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Registry = require('azure-iothub').Registry;
var Http = require('../lib/http.js');
var clientTests = require('azure-iot-device/test/_client_common_testrun.js');

describe('Over real HTTPS', function () {
  this.timeout(60000);
  var registry = Registry.fromConnectionString(process.env.IOTHUB_CONNECTION_STRING);
  clientTests.sendEventTests(Http, registry);
  clientTests.sendEventBatchTests(Http, registry);
});
