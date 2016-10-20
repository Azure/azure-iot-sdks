#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var program = require('commander');
var uuid = require('uuid');
var inputError = require('./common.js').inputError;
var serviceError = require('./common.js').serviceError;
var printSuccess = require('./common.js').printSuccess;
var printDevice = require('./common.js').printDevice;
var getHostFromSas = require('./common.js').getHostFromSas;
var getSas = require('./common.js').getSas;
var Registry = require('azure-iothub').Registry;

var info;

program
  .description('Create a device identity in your IoT Hub device registry, either using the specified device id or JSON description.')
  .usage('[options] [device-id|device-json]')
  .option('-a, --auto', 'create a device with an auto-generated device id')
  .option('-cs, --connection-string', 'show the connection string for the newly created device')
  .option('-d, --display <property-filter>', 'comma-separated list of device properties that should be displayed')
  .option('-l, --login <connection-string>', 'connection string to use to authenticate with your IoT Hub instance')
  .option('-k1, --key1 <key>', 'specify the primary key for newly created device')
  .option('-k2, --key2 <key>', 'specify the secondary key for newly created device')
  .option('-r, --raw', 'use this flag to return raw JSON instead of pretty-printed output')
  .option('-x, --x509', 'generate an x509 certificate to authenticate the device')
  .option('-dv, --daysValid', 'number of days the x509 certificate should be valid for')
  .option('-t1, --thumbprint1 <thumbprint>', 'specify the primary thumbprint of the x509 certificate')
  .option('-t2, --thumbprint2 <thumbprint>', 'specify the secondary thumbprint of the x509 certificate')
  .parse(process.argv);

if((program.key1 || program.key2) && (program.x509 || program.thumbprint1 || program.thumbprint2)) {
  inputError('A device can use either x509 certificates or symmetric keys to authenticate but not both.');
}

if(program.daysValid && !program.x509) {
  inputError('The --daysValid option applies only with x509 certificates');
}

if (program.auto && program.args[0]) {
  inputError('You cannot use the \'--auto\' option if you specify the device id or description');
} else if (!program.auto && !program.args[0]) {
  inputError('You must either use the \'--auto\' option or specify a device id or description');
} else if (program.auto) {
  info = {
    deviceId: uuid.v4(),
    status: 'enabled'
  };
} else {
  try {
    // 'create' command expects either deviceId or JSON device description
    info = (program.args[0].charAt(0) !== '{') ? { deviceId: program.args[0], status: 'enabled' } : JSON.parse(program.args[0]);
  }
  catch (e) {
    if (e instanceof SyntaxError) inputError('Device information isn\'t valid JSON');
    else throw e;
  }
}

if(program.x509) {
  if (program.thumbprint1 || program.thumbprint2) {
    info.authentication = {
      x509Thumbprint: {
        primaryThumbprint: program.thumbprint1,
        secondaryThumbprint: program.thumbprint2,
      }
    };
    console.log(JSON.stringify(info, null, 2));
    createDevice(info);
  } else {
    generateCertAndCreateDevice(info);
  }
} else if (program.key1 || program.key2) {
  info.authentication = {
    symmetricKey: {
      primaryKey: program.key1,
      secondaryKey: program.key2,
    }
  };
  createDevice(info);
} else if (isMissingAuth(info)) {
  console.log('No authentication method given. Device will be created with auto-generated symmetric keys.');
  createDevice(info);
} else {
  // Auth info provided as part of the JSON parameter
  createDevice(info);
}

function createDevice(deviceInfo) {
  var sas = getSas(program.login);

  var registry = Registry.fromSharedAccessSignature(sas);
  registry.create(deviceInfo, function (err, createdDeviceInfo) {
    if (err) serviceError(err);
    else {
      printSuccess('Created device ' + deviceInfo.deviceId);
      var host = getHostFromSas(sas);
      printDevice(createdDeviceInfo, host, program.display, program.connectionString, program.raw);
    }
  });
}

function generateCertAndCreateDevice(deviceInfo) {
  var pem = require('pem');
  var fs = require('fs');

  var certFile = deviceInfo.deviceId + '-cert.pem';
  var keyFile = deviceInfo.deviceId + '-key.pem';
  var thumbprint = null;

  var certOptions = {
    selfSigned: true,
    days: program.daysValid || 365
  };

  pem.createCertificate(certOptions, function (err, result) {
    if (err) {
      inputError('You must have OpenSSL installed in your path for iothub-explorer to be able to generate x509 certificates');
    } else {
      fs.writeFileSync(certFile, result.certificate);
      fs.writeFileSync(keyFile, result.clientKey);
      printSuccess('Certificate File: ' + certFile);
      printSuccess('Key File: ' + keyFile);
      pem.getFingerprint(result.certificate, function (err, result) {
        thumbprint = result.fingerprint.replace(/:/g, '');
        deviceInfo.authentication = {
          x509Thumbprint: {
            primaryThumbprint: thumbprint
          }
        };
        createDevice(deviceInfo);
      });
    }
  });
}

function isMissingAuth(deviceInfo) {
  return deviceInfo.authentication ? 
           deviceInfo.authentication.symmetricKeys ?
             deviceInfo.authentication.symmetricKeys.primaryKey || deviceInfo.authentication.symmetricKeys.secondaryKey
           : deviceInfo.authentication.x509Thumbprints ?
             deviceInfo.authentication.x509Thumbprints.primaryThumbprint || deviceInfo.authentication.x509Thumbprints.secondaryThumbprint
           : false
         : false;
}