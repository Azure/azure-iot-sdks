// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var deviceAmqp = require('azure-iot-device-amqp');
var deviceHttp = require('azure-iot-device-http');
var ConnectionString = require('azure-iothub').ConnectionString;

var device_service_tests = require('./test/device_service.js');
var registry_tests = require('./test/registry.js');
var file_upload_tests = require('./test/file_upload.js');

var deviceName = process.env.IOTHUB_DEVICE_ID;
var deviceKey = process.env.IOTHUB_DEVICE_KEY;
var hubConnectionString = process.env.IOTHUB_CONNECTION_STRING;
var storageConnectionString = process.env.STORAGE_CONNECTION_STRING;
var host = ConnectionString.parse(process.env.IOTHUB_CONNECTION_STRING).HostName;
var deviceConnectionString = 'HostName=' + host + ';DeviceId=' + deviceName + ';SharedAccessKey=' + deviceKey;

device_service_tests(deviceAmqp.Amqp, hubConnectionString, deviceConnectionString, deviceName, deviceKey);
device_service_tests(deviceHttp.Http, hubConnectionString, deviceConnectionString, deviceName, deviceKey);

registry_tests(hubConnectionString, storageConnectionString);

file_upload_tests(hubConnectionString, deviceConnectionString, deviceName);
