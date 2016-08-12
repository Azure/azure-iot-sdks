// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var fs = require('fs');
var pem = require('pem');
var Registry = require('azure-iothub').Registry;
var chalk = require('chalk');
var argv = require('yargs')
           .usage('Usage: $0 --connectionString <IOTHUB CONNECTION STRING> --deviceId <DEVICE ID> --daysValid <number of days valid>')
           .demand(['connectionString', 'deviceId'])
           .default('daysValid', 1)
           .describe('connectionString', 'Azure IoT Hub service connection string that shall have device creation permissions')
           .describe('deviceId', 'Unique identifier for the device that shall be created')
           .describe('daysValid', 'Number of days the certificate will remain valid')
           .argv

var certFile = argv.deviceId + '-cert.pem';
var keyFile = argv.deviceId + '-key.pem';
var thumbprint = null;

function createCertsAndDevice(done) {
  var certOptions = {
    selfSigned: true,
    days: argv.daysValid
  };

  pem.createCertificate(certOptions, function (err, result) {
    if (err) {
      done(err);
    } else {
      fs.writeFileSync(certFile, result.certificate);
      fs.writeFileSync(keyFile, result.clientKey);
      pem.getFingerprint(result.certificate, function (err, result) {
        thumbprint = result.fingerprint.replace(/:/g, '');

        var deviceDescription = {
          deviceId: argv.deviceId,
          status: 'enabled',
          authentication: {
            x509Thumbprint: {
              primaryThumbprint: thumbprint
            }
          }
        };

        registry.create(deviceDescription, function (err, deviceInfo) {
          console.log(chalk.green('Device successfully created:'));
          console.log(JSON.stringify(deviceInfo, null, 2));
        });
      });
    }
  });
}

var registry = Registry.fromConnectionString(argv.connectionString);
registry.get(argv.deviceId, function(err, deviceInfo) {
  if (!err) {
    console.log(chalk.red('Device already exists: ' + argv.deviceId));
    process.exit(1);
  } else {
    createCertsAndDevice(function(err) {
      if (err) {
        console.log(chalk.red('Could not create certificates or device: ' + err.message));
        process.exit(1);
      } else {
        console.log(chalk.green('Device \'' + argv.deviceId + '\' created successfully.'));
        console.log(chalk.white('\tcertificate file: ' + certfile));
        console.log(chalk.white('\tkey file: ' + keyfile));
        process.exit(0);
      }
    });
  }
});