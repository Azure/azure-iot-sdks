#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

// External dependencies
var program = require('commander');

// Local dependencies
var inputError = require('./common.js').inputError;
var serviceError = require('./common.js').serviceError;
var printSuccess = require('./common.js').printSuccess;
var getHostFromSas = require('./common.js').getHostFromSas;
var getSas = require('./common.js').getSas;

// Azure IoT SDK dependencies
var deviceSas = require('azure-iot-device').SharedAccessSignature;
var Registry = require('azure-iothub').Registry;

program
  .description('Generate a shared access signature token for the given device with an expiry time <num-seconds> from now')
  .option('-l, --login <connectionString>', 'use the connection string provided as argument to use to authenticate with your IoT hub')
  .option('-d, --duration <durationInSeconds>', 'expiration time (in seconds): if not specified, the default is one hour')
  .parse(process.argv);

if(!program.args[0]) inputError('You must specify a device id.');

var deviceId = program.args[0];
var nowInSeconds = Math.floor(Date.now() / 1000);
var expiry = program.duration ? nowInSeconds + parseInt(program.duration) : nowInSeconds + 3600;

if (isNaN(new Date(expiry * 1000))) {
  inputError('Invalid duration');
}

var serviceSas = getSas(program.login);
var registry = Registry.fromSharedAccessSignature(serviceSas);

registry.get(deviceId, function (err, device) {
  if (err)
    serviceError(err);
  else {
    var key = device.authentication.SymmetricKey.primaryKey || device.authentication.SymmetricKey.secondaryKey;
    if (!key) {
      inputError('Cannot create a SAS for this device. It does not use symmetric key authentication.');
    }
    var host = getHostFromSas(serviceSas);
    var sas = deviceSas.create(host, deviceId, key, expiry);
    if (program.raw) {
      console.log(sas.toString());
    } else {
      printSuccess('Shared Access Key for ' + deviceId + ':');
      console.log(sas.toString());
      printSuccess('is valid until: ' + new Date(expiry * 1000).toString());
    }
  }
});