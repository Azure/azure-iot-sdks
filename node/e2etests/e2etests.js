// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';


var deviceAmqp = require('azure-iot-device-amqp');
var deviceAmqpWs = require('azure-iot-device-amqp-ws');
var deviceHttp = require('azure-iot-device-http');
var deviceMqtt = require('azure-iot-device-mqtt');

var device_provision = require('./test/device_provision.js');
var device_service_tests = require('./test/device_service.js');
var registry_tests = require('./test/registry.js');
var file_upload_tests = require('./test/file_upload.js');
var device_acknowledge_tests = require('./test/device_acknowledge_tests.js')
var device_teardown = require('./test/device_teardown.js');

var hubConnectionString = process.env.IOTHUB_CONNECTION_STRING;
var storageConnectionString = process.env.STORAGE_CONNECTION_STRING;
var generalProtocols = [deviceHttp.Http];//, deviceAmqp.Amqp, deviceAmqpWs.AmqpWs];
var acknowledgementProtocols = [deviceHttp.Http, deviceAmqp.Amqp, deviceAmqpWs.AmqpWs]
var deviceToTest;
var protocolToTest;

device_provision(hubConnectionString, function (err, provisionedDevices) {
  if (err) {
    console.log('Unable to create the devices needed.');
  } else {
    for (deviceToTest = 0; deviceToTest < provisionedDevices.length; deviceToTest++) {
      for (protocolToTest = 0; protocolToTest < generalProtocols.length; protocolToTest++) {
        device_service_tests(generalProtocols[protocolToTest], hubConnectionString, provisionedDevices[deviceToTest]);
//        file_upload_tests(generalProtocols[protocolToTest], hubConnectionString, provisionedDevices[deviceToTest]);
      }
      for (protocolToTest = 0; protocolToTest < acknowledgementProtocols.length; protocolToTest++) {
//        device_acknowledge_tests(generalProtocols[protocolToTest], hubConnectionString, provisionedDevices[deviceToTest]);
      }
    }
//    registry_tests(hubConnectionString, storageConnectionString);
  }
  if (provisionedDevices) device_teardown(hubConnectionString, provisionedDevices);
  run();
});
