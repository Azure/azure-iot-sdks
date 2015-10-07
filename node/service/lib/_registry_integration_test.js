// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var runTests = require('./_registry_test.js');
var Https = require('azure-iot-common').Https;

function createTransport() {
  return new Https();
}

var host = process.env.IOTHUB_NAME + '.' + process.env.IOTHUB_SUFFIX;
var policy = process.env.IOTHUB_POLICY_NAME;
var key = process.env.IOTHUB_POLICY_KEY;
var deviceId = process.env.IOTHUB_DEVICE_ID;

var connectionString = 'HostName='+host+';CredentialType=type;CredentialScope=scope;SharedAccessKeyName='+policy+';SharedAccessKey='+key;
var badConnStrings = [
  'HostName=bad;CredentialType=type;CredentialScope=scope;SharedAccessKeyName='+policy+';SharedAccessKey='+key,
  'HostName='+host+';CredentialType=type;CredentialScope=scope;SharedAccessKeyName=bad;SharedAccessKey='+key,
  'HostName='+host+';CredentialType=type;CredentialScope=scope;SharedAccessKeyName='+policy+';SharedAccessKey=bad'
];

describe('Over real HTTPS', function () {
  this.timeout(5000);
  runTests(createTransport, connectionString, badConnStrings, deviceId);
});
