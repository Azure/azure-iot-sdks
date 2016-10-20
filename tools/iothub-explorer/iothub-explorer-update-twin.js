#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var program = require('commander');
var prettyjson = require('prettyjson');
var inputError = require('./common.js').inputError;
var serviceError = require('./common.js').serviceError;
var getSas = require('./common.js').getSas;
var Registry = require('azure-iothub').Registry;

program
  .description('Update the twin of a device with the JSON description provided on the command line')
  .usage('[options] <device-id> <device-json>')
  .option('-l, --login <connection-string>', 'use the connection string provided as argument to use to authenticate with your IoT Hub instance')
  .option('-r, --raw', 'use this flag to return raw output instead of pretty-printed output')
  .parse(process.argv);

if(!program.args[0]) inputError('You must specify a deviceId.');
if(!program.args[1]) inputError('You must specify the JSON to update the twin with.');

var deviceId = program.args[0];
var twinJson = program.args[1];

var updateInfo;
try {
  updateInfo = JSON.parse(twinJson);
}
catch (e) {
  if (e instanceof SyntaxError) inputError('device-json isn\'t valid JSON');
  else throw e;
}

var sas = getSas(program.login);

var registry = Registry.fromSharedAccessSignature(sas);
registry.getTwin(deviceId, function (err, twin){
  if (err) {
    serviceError(err);
  } else {
    registry.updateTwin(twin.deviceId, updateInfo, twin.etag, function (err, updatedTwin){
      if (err) {
        serviceError(err);
      } else {
        var output = program.raw ? JSON.stringify(updatedTwin) : prettyjson.render(updatedTwin);
        console.log(output);
      }
    });
  }
});