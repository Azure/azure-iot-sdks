#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var program = require('commander');
var prettyjson = require('prettyjson');
var inputError = require('./common.js').inputError;
var serviceError = require('./common.js').serviceError;
var getSas = require('./common.js').getSas;
var Client = require('azure-iothub').Client;

program
  .description('Call a device method on a specific device')
  .usage('[options] <device-id> <method-name> [method-payload] [method-timeout]')
  .option('-l, --login <connection-string>', 'use the connection string provided as argument to use to authenticate with your IoT Hub instance')
  .option('-r, --raw', 'use this flag to return raw output instead of pretty-printed output')
  .parse(process.argv);

if(!program.args[0]) inputError('You must specify a deviceId.');
if(!program.args[1]) inputError('You must specify the JSON to update the twin with.');

var deviceId = program.args[0];
var methodName = program.args[1];
var methodPayload = program.args[2];
var methodTimeout = program.args[3];

if(!deviceId) inputError('Please provide a valid device id');
if(!methodName) inputError('Please provide a valid method name');

var sas = getSas(program.login);

var client = Client.fromSharedAccessSignature(sas);
var methodParams = {
  methodName: methodName,
  payload: methodPayload || null,
  timeoutInSeconds: !!methodTimeout ? parseInt(methodTimeout) : null
};

client.invokeDeviceMethod(deviceId, methodParams, function(err, methodResult) {
  if (err) {
    serviceError(err);
  } else {
    var output = program.raw ? JSON.stringify(methodResult) : prettyjson.render(methodResult);
    console.log(output);
  }
});