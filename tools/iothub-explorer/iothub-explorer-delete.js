#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var program = require('commander');
var inputError = require('./common.js').inputError;
var serviceError = require('./common.js').serviceError;
var printSuccess = require('./common.js').printSuccess;
var getSas = require('./common.js').getSas;
var Registry = require('azure-iothub').Registry;

program
  .description('Delete a device identity from your IoT hub device registry')
  .usage('[options] <device-id>')
  .option('-l, --login <connection-string>', 'connection string to use to authenticate with your IoT hub')
  .parse(process.argv);

if(!program.args[0]) {
  inputError('You must specify a deviceId');
}

var deviceId = program.args[0];
var sas = getSas(program.login);

var registry = Registry.fromSharedAccessSignature(sas);
registry.delete(deviceId, function (err) {
  if (err) serviceError(err);
  else printSuccess('Deleted device ' + deviceId);
});