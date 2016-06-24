// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ConnectionString = require('azure-iot-common').ConnectionString;
var runTests = require('azure-iot-device/test/_client_common_testrun.js');
var Http = require('../lib/http.js');
var fs = require('fs');

var host = ConnectionString.parse(process.env.IOTHUB_CONNECTION_STRING).HostName;
var deviceId = process.env.IOTHUB_DEVICE_ID;
var key = process.env.IOTHUB_DEVICE_KEY;

var x509DeviceId = process.env.IOTHUB_X509_DEVICE_ID;
var x509Certificate = fs.readFileSync(process.env.IOTHUB_X509_CERTIFICATE);
var x509Key = fs.readFileSync(process.env.IOTHUB_X509_KEY);
var x509Passphrase = process.env.IOTHUB_X509_PASSPHRASE;
var x509ConnectionString = 'HostName=' + host + ';DeviceId=' + x509DeviceId + ';x509=true';

var connectionString = 'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=' + key;
var badConnStrings = [
  'HostName=bad;DeviceId=' + deviceId + ';SharedAccessKey=' + key,
  'HostName=' + host + ';DeviceId=bad;SharedAccessKey=' + key,
  'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=bad;'
];

describe('Over real HTTPS with a Shared Access Key', function () {
  this.timeout(60000);
  runTests(Http, connectionString, badConnStrings);
});

describe('Over real HTTPS with an x509 certificate', function() {
  this.timeout(60000);
  runTests(Http, x509ConnectionString, badConnStrings, x509Certificate, x509Key, x509Passphrase);
});
