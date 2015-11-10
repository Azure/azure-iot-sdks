// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var runTests = require('./_registry_test.js');

var host = process.env.IOTHUB_NAME + '.' + process.env.IOTHUB_SUFFIX;
var policy = process.env.IOTHUB_POLICY_NAME;
var key = process.env.IOTHUB_POLICY_KEY;
var deviceId = process.env.IOTHUB_DEVICE_ID;

function makeConnectionString(host, policy, key) {
  return 'HostName='+host+';SharedAccessKeyName='+policy+';SharedAccessKey='+key;
}

var connectionString = makeConnectionString(host, policy, key);
var badConnStrings = [
  makeConnectionString('bad', policy, key),
  makeConnectionString(host, 'bad', key),
  makeConnectionString(host, policy, 'bad'),
];

describe('Over real HTTPS', function () {
  this.timeout(15000);
  runTests(null, connectionString, badConnStrings, deviceId);
});
