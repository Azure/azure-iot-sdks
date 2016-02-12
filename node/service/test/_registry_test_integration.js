// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ConnectionString = require('../lib/connection_string.js');
var runTests = require('./_registry_common_testrun.js');

function makeConnectionString(host, policy, key) {
  return 'HostName=' + host + ';SharedAccessKeyName=' + policy + ';SharedAccessKey=' + key;
}

var connectionString = process.env.IOTHUB_CONNECTION_STRING;
var deviceId = process.env.IOTHUB_DEVICE_ID;
var cn = ConnectionString.parse(connectionString);

var badConnStrings = [
  makeConnectionString('bad', cn.SharedAccessKeyName, cn.SharedAccessKey),
  makeConnectionString(cn.HostName, 'bad', cn.SharedAccessKey),
  makeConnectionString(cn.HostName, cn.SharedAccessKeyName, 'bad'),
];

describe('Over real HTTPS', function () {
  this.timeout(60000);
  runTests(null, connectionString, badConnStrings, deviceId);
});
