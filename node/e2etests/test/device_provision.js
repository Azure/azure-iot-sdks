// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ConnectionString = require('azure-iothub').ConnectionString;
var deviceSas = require('azure-iot-device').SharedAccessSignature;
var anHourFromNow = require('azure-iot-common').anHourFromNow;

var assert = require('chai').assert;
var debug = require('debug')('e2etests');
var uuid = require('uuid');

var pem = require('pem');
var Registry = require('azure-iothub').Registry;
var chalk = require('chalk');


var device_provision = function (hubConnectionString, done) {
  var provisionedDevices = [];
  var registry = Registry.fromConnectionString(hubConnectionString);
  var host = ConnectionString.parse(hubConnectionString).HostName;
  function createDeviceWithCert(deviceId, done) {
    var certConstructinResult;
    var certOptions = {
      selfSigned: true,
      days: 1
    };

    pem.createCertificate(certOptions, function (err, certConstructionResult) {
      if (err) {
        done(err);
      } else {
        pem.getFingerprint(certConstructionResult.certificate, function (err, fingerPrintResult) {

          var thumbPrint = fingerPrintResult.fingerprint.replace(/:/g, '');
          var deviceDescription = {
            deviceId: deviceId,
            status: 'enabled',
            authentication: {
              x509Thumbprint: {
                primaryThumbprint: thumbPrint
              }
            }
          };

          registry.create(deviceDescription, function (err, deviceInfo) {
            if (err) {
              console.log(chalk.green('Device was NOT successfully created: ') + deviceId);
              done(err);
            } else {
              var i = provisionedDevices.length;
              var connectionString = 'HostName=' + host + ';DeviceId=' + deviceId + ';x509=true';
              provisionedDevices[i] = {};
              provisionedDevices[i].authenticationDescription = 'x509 certificate';
              provisionedDevices[i].deviceId = deviceId;
              provisionedDevices[i].connectionString = connectionString;
              provisionedDevices[i].certificate = certConstructionResult.certificate;
              provisionedDevices[i].clientKey = certConstructionResult.clientKey;
              done(null)
            }
          });
        });
      }
    });
  };

  function createKeyDevice(deviceId, done) {
    var deviceDescription = {
      deviceId: deviceId,
      status: 'enabled',
      authentication: {
        SymmetricKey: {
          primaryKey: new Buffer(uuid.v4()).toString('base64'),
          secondaryKey: new Buffer(uuid.v4()).toString('base64')
        }
      }
    };

    registry.create(deviceDescription, function (err, deviceInfo) {
      if (err) {
        console.log(chalk.green('Device was NOT successfully created: ') + deviceId);
        done(err);
      } else {
        var i = provisionedDevices.length;
        provisionedDevices[i] = {};
        provisionedDevices[i].deviceId = deviceId;
        provisionedDevices[i].authenticationDescription = 'shared private key';
        provisionedDevices[i].primaryKey = deviceDescription.authentication.SymmetricKey.primaryKey;
        provisionedDevices[i].connectionString = 'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=' + provisionedDevices[i].primaryKey;
        done(null);
      }
    });
  };

  function createSASDevice(deviceId, done) {
    var deviceDescription = {
      deviceId: deviceId,
      status: 'enabled',
      authentication: {
        SymmetricKey: {
          primaryKey: new Buffer(uuid.v4()).toString('base64'),
          secondaryKey: new Buffer(uuid.v4()).toString('base64')
        }
      }
    };

    registry.create(deviceDescription, function (err, deviceInfo) {
      if (err) {
        console.log(chalk.green('Device was NOT successfully created: ') + deviceId);
        done(err);
      } else {
        var i = provisionedDevices.length;
        provisionedDevices[i] = {};
        provisionedDevices[i].deviceId = deviceId;
        provisionedDevices[i].authenticationDescription = 'application supplied SAS';
        provisionedDevices[i].connectionString = deviceSas.create(host, deviceId, deviceDescription.authentication.SymmetricKey.primaryKey, anHourFromNow()).toString();
        done(null);
      }
    });
  };

  var deviceId = '0000e2etest-delete-me-node-x509-' + uuid.v4();
  registry.get(deviceId, function(err, deviceInfo) {
    if (!err || err.constructor.name !== 'DeviceNotFoundError') {
      var errMessageText = 'error creating e2e test device ' + deviceId + !err? 'device already exists':err.constructor.name;
      console.log(chalk.red(errMessageText));
      done(new errors.DeviceAlreadyExistsError(errMessageText));
    } else {
      createDeviceWithCert(deviceId, function(err) {
        if (err) {
          console.log(chalk.red('Could not create certificates or device: ' + err.message));
          done(err);
        } else {
          var deviceId = '0000e2etest-delete-me-node-key-' + uuid.v4();
          registry.get(deviceId, function(err, deviceInfo) {
            if (!err || err.constructor.name !== 'DeviceNotFoundError') {
              var errMessageText = 'error creating e2e test device ' + deviceId + !err? 'device already exists':err.constructor.name;
              console.log(chalk.red(errMessageText));
              done(new errors.DeviceAlreadyExistsError(errMessageText), provisionedDevices);
            } else {
              createKeyDevice(deviceId, function(err) {
                if (err) {
                  console.log(chalk.red('Could not create certificates or device: ' + err.message));
                  done(err, provisionedDevices);
                } else {
                  var deviceId = '0000e2etest-delete-me-node-sas-' + uuid.v4();
                  registry.get(deviceId, function(err, deviceInfo) {
                    if (!err || err.constructor.name !== 'DeviceNotFoundError') {
                      var errMessageText = 'error creating e2e test device ' + deviceId + !err? 'device already exists':err.constructor.name;
                      console.log(chalk.red(errMessageText));
                      done(new errors.DeviceAlreadyExistsError(errMessageText), provisionedDevices);
                    } else {
                      createSASDevice(deviceId, function(err) {
                        if (err) {
                          console.log(chalk.red('Could not create certificates or device: ' + err.message));
                          done(err, provisionedDevices);
                        } else {
                          done(null,provisionedDevices)
                        }
                      });
                    }
                  });
                }
              });
            }
          });
        }
      });
    }
  });

  //   it('Create a device with device key based authentication', function (done) {
  //     var deviceId = 'e2etest-delete-me-node-key-' + uuid.v4();
  //     registry.get(deviceId, function(err, deviceInfo) {
  //       if (!err || err.constructor.name !== 'Error') {
  //         console.log(chalk.red('Device already exists: ' + deviceId + ' ' + err.constructor.name));
  //         assert.isNull(err);
  //       } else {
  //         createDeviceWithCert(deviceId, host, registry, function(err) {
  //           if (err) {
  //             console.log(chalk.red('Could not create certificates or device: ' + err.message));
  //           }
  //         });
  //       }
  //     })
  //   });
  //   it('Create a device with device SAS based authentication', function (done) {
  //     var deviceId = 'e2etest-node-SAS-' + uuid.v4();
  //     registry.get(deviceId, function(err, deviceInfo) {
  //       if (!err || err.constructor.name !== 'Error') {
  //         console.log(chalk.red('Device already exists: ' + deviceId + ' ' + err.constructor.name));
  //         assert.isNull(err);
  //       } else {
  //         createDeviceWithCert(deviceId, host, registry, function(err) {
  //           if (err) {
  //             console.log(chalk.red('Could not create certificates or device: ' + err.message));
  //           }
  //         });
  //       }
  //     })
  //   });
  // });
}
module.exports = device_provision;