// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ConnectionString = require('azure-iothub').ConnectionString;
var deviceSas = require('azure-iot-device').SharedAccessSignature;
var anHourFromNow = require('azure-iot-common').anHourFromNow;
var errors = require('azure-iot-common').errors;

var uuid = require('uuid');

var pem = require('pem');
var Registry = require('azure-iothub').Registry;
var chalk = require('chalk');


var device_provision = function (hubConnectionString, done) {
  var provisionedDevices = [];
  var registry = Registry.fromConnectionString(hubConnectionString);
  var host = ConnectionString.parse(hubConnectionString).HostName;

  function setupDevice(deviceDescription, provisionDescription, done) {
    registry.create(deviceDescription, function (err) {
      if (err) {
        console.log(chalk.red('Device was NOT successfully created: ') + deviceDescription.deviceId);
        done(err);
      } else {
        provisionedDevices.push(provisionDescription);
        done();
      }
    });
  }
  
  function createCertDevice(deviceId, done) {
    var certOptions = {
      selfSigned: true,
      days: 1
    };

    pem.createCertificate(certOptions, function (err, certConstructionResult) {
      if (err) {
        done(err);
      } else {
        pem.getFingerprint(certConstructionResult.certificate, function (err, fingerPrintResult) {
          if (err) {
            done(err);
          } else {
            var thumbPrint = fingerPrintResult.fingerprint.replace(/:/g, '');
            setupDevice(
              {
                deviceId: deviceId,
                status: 'enabled',
                authentication: {
                  x509Thumbprint: {
                    primaryThumbprint: thumbPrint
                  }
                }
              },
              {
                authenticationDescription: 'x509 certificate',
                deviceId: deviceId,
                connectionString: 'HostName=' + host + ';DeviceId=' + deviceId + ';x509=true',
                certificate: certConstructionResult.certificate,
                clientKey: certConstructionResult.clientKey
              },
              done
            );
          }
        });
      }
    });
  }

  function createKeyDevice(deviceId, done) {
    var pkey = new Buffer(uuid.v4()).toString('base64');
    setupDevice(
      {
        deviceId: deviceId,
        status: 'enabled',
        authentication: {
          symmetricKey: {
            primaryKey: pkey,
            secondaryKey: new Buffer(uuid.v4()).toString('base64')
          }
        }
      },
      {
        deviceId: deviceId,
        authenticationDescription:'shared private key',
        primaryKey: pkey,
        connectionString: 'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=' + pkey
      },
      done
    );
  }

  function createSASDevice(deviceId, done) {
    var pkey = new Buffer(uuid.v4()).toString('base64');
    setupDevice(
      {
        deviceId: deviceId,
        status: 'enabled',
        authentication: {
          symmetricKey: {
            primaryKey: pkey,
            secondaryKey: new Buffer(uuid.v4()).toString('base64')
          }
        }
      },
      {
        deviceId: deviceId,
        authenticationDescription: 'application supplied SAS',
        connectionString: deviceSas.create(host, deviceId, pkey, anHourFromNow()).toString()
      },
      done
    );
  }

  function createDeviceSafe(deviceId, createDevice, next) {
    registry.get(deviceId, function(err) {
      if (!err || err.constructor.name !== 'DeviceNotFoundError') {
        var errMessageText = 'error creating e2e test device ' + deviceId + ' ' + (err ? err.constructor.name : 'device already exists');
        console.log(chalk.red(errMessageText));
        done(new errors.DeviceAlreadyExistsError(errMessageText),provisionedDevices);
      } else {
        createDevice(deviceId, function(err){
          if (err) {
            console.log(chalk.red('Could not create certificates or device: ' + err.message));
            done(err, provisionedDevices);
          } else {
            next();
          }
        });
      }
    });
  }

  createDeviceSafe( '0000e2etest-delete-me-node-x509-' + uuid.v4(), createCertDevice, function() {
    createDeviceSafe('0000e2etest-delete-me-node-key-' + uuid.v4(), createKeyDevice, function() {
      createDeviceSafe('0000e2etest-delete-me-node-sas-' + uuid.v4(), createSASDevice, function() {
        done(null,provisionedDevices);
      });
    });
  });
};
module.exports = device_provision;