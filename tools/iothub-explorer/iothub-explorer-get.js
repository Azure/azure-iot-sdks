#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var program = require('commander');
var inputError = require('./common.js').inputError;
var serviceError = require('./common.js').serviceError;
var printDevice = require('./common.js').printDevice;
var getHostFromSas = require('./common.js').getHostFromSas;
var getSas = require('./common.js').getSas;
var Registry = require('azure-iothub').Registry;

program
  .description('Get a device identity from your IoT hub device registry')
  .usage('[options] <device-id>')
  .option('-c, --connection-string', 'show the connection string for the newly created device')
  .option('-d, --display <property-filter>', 'filter the properties of the device that should be displayed using a comma-separated list of property names')
  .option('-l, --login <connection-string>', 'use the connection string provided as argument to use to authenticate with your IoT Hub instance')
  .option('-r, --raw', 'use this flag to return raw output instead of pretty-printed output')
  .parse(process.argv);

if(!program.args[0]) inputError('You must specify a deviceId.');
var sas = getSas(program.login);
var deviceId = program.args[0];

var registry = Registry.fromSharedAccessSignature(sas);
registry.get(deviceId, function (err, device) {
  if (err) serviceError(err);
  else {
    var host = getHostFromSas(sas);
    printDevice(device, host, program.display, program.connectionString, program.raw);
  }
});