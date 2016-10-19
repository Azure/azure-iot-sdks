#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var program = require('commander');
var serviceError = require('./common.js').serviceError;
var printDevice = require('./common.js').printDevice;
var getHostFromSas = require('./common.js').getHostFromSas;
var getSas = require('./common.js').getSas;
var Registry = require('azure-iothub').Registry;

program
  .description('List the device identities currently in your IoT hub device registry')
  .option('-l, --login <connection-string>', 'use the connection string provided as argument to use to authenticate with your IoT hub')
  .option('-d, --display <property-filter>', 'filter the properties of the device that should be displayed using a comma-separated list of property names')
  .option('-r, --raw', 'use this flag to return raw output instead of pretty-printed output')
  .option('--connection-string', 'show the connection string for the newly created device')
  .parse(process.argv);

var sas = getSas(program.login);

var registry = Registry.fromSharedAccessSignature(sas);
registry.list(function (err, devices) {
  if (err) serviceError(err);
  else {
    var host = getHostFromSas(sas);
    devices.forEach(function (device) {
      printDevice(device, host, program.display, program.connectionString, program.raw);
    });
  }
});