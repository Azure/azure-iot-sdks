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
  .description('Get the twin of the specified device')
  .usage('[options] <device-id>')
  .option('-l, --login <connection-string>', 'use the connection string provided as argument to use to authenticate with your IoT Hub instance')
  .option('-r, --raw', 'use this flag to return raw output instead of pretty-printed output')
  .parse(process.argv);

var deviceId = program.args[0];
if(!deviceId) inputError('You must specify a deviceId.');

var sas = getSas(program.login);

var registry = Registry.fromSharedAccessSignature(sas);
registry.getTwin(deviceId, function (err, twin) {
  if (err) serviceError(err);
  else {
    var output = program.raw ? JSON.stringify(twin) : prettyjson.render(twin);
    console.log(output);
  }
});