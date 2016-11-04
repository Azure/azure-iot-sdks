// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var azureStorage = require('azure-storage');
var Registry = require('azure-iothub').Registry;
var errors = require('azure-iot-common').errors;

var assert = require('chai').assert;
var uuid = require('uuid');
var debug = require('debug')('e2etests:registry');
var Promise = require('bluebird');

var runTests = function (hubConnectionString, storageConnectionString) {
  var deviceIdOnly = {
    deviceId: uuid.v4()
  };

  var deviceIdWithKeys = {
    deviceId: uuid.v4(),
    authentication: {
      symmetricKey: {
        primaryKey: new Buffer("1234567890qwerty").toString('base64'),
        secondaryKey: new Buffer("ytrewq0987654321").toString('base64')
      }
    },
    status: "disabled"
  };

  var deviceIdWithThumbprints = {
    deviceId: uuid.v4(),
    authentication: {
      x509Thumbprint: {
        primaryThumbprint: '0000000000000000000000000000000000000000',
        secondaryThumbprint: '1111111111111111111111111111111111111111'
      }
    },
    status: "disabled"
  };

  describe('Registry', function () {
    this.timeout(30000);
    it('Creates a device with only a deviceId and gets it', function (done){
      var registry = Registry.fromConnectionString(hubConnectionString);

      registry.create(deviceIdOnly, function(createErr, createResult) {
        if (createErr) {
          done(createErr);
        } else {
          assert.equal(createResult.deviceId, deviceIdOnly.deviceId, 'created device doesn\'t have the requested deviceId');
          registry.get(deviceIdOnly.deviceId, function(getErr, getResult) {
            if (getErr) {
              done(getErr);
            } else {
              assert.equal(getResult.deviceId, deviceIdOnly.deviceId);
              done();
            }
          });
        }
      });
    });

    it('Creates a device with secret key parameters and gets it', function (done){
      var registry = Registry.fromConnectionString(hubConnectionString);

      registry.create(deviceIdWithKeys, function(createErr, createResult) {
        if (createErr) {
          done(createErr);
        } else {
          assert.equal(createResult.deviceId, deviceIdWithKeys.deviceId, 'created device doesn\'t have the requested deviceId');
          registry.get(deviceIdWithKeys.deviceId, function(getErr, getResult) {
            if (getErr) {
              done(getErr);
            } else {
              assert.equal(getResult.deviceId, deviceIdWithKeys.deviceId);
              assert.equal(getResult.authentication.symmetricKey.primaryKey, deviceIdWithKeys.authentication.symmetricKey.primaryKey);
              assert.equal(getResult.authentication.symmetricKey.secondaryKey, deviceIdWithKeys.authentication.symmetricKey.secondaryKey);
              assert.equal(getResult.disabled, deviceIdWithKeys.disabled);
              done();
            }
          });
        }
      });
    });

    it('Creates a device with thumbprint parameters and gets it', function (done){
      var registry = Registry.fromConnectionString(hubConnectionString);

      registry.create(deviceIdWithThumbprints, function(createErr, createResult) {
        if (createErr) {
          done(createErr);
        } else {
          assert.equal(createResult.deviceId, deviceIdWithThumbprints.deviceId, 'created device doesn\'t have the requested deviceId');
          registry.get(deviceIdWithThumbprints.deviceId, function(getErr, getResult) {
            if (getErr) {
              done(getErr);
            } else {
              assert.equal(getResult.deviceId, deviceIdWithThumbprints.deviceId);
              assert.equal(getResult.authentication.x509Thumbprint.primaryThumbprint, deviceIdWithThumbprints.authentication.x509Thumbprint.primaryThumbprint);
              assert.equal(getResult.authentication.x509Thumbprint.secondaryThumbprint, deviceIdWithThumbprints.authentication.x509Thumbprint.secondaryThumbprint);
              assert.equal(getResult.disabled, deviceIdWithThumbprints.disabled);
              done();
            }
          });
        }
      });
    });

    it('Fails to create a device with an invalid name', function(done) {
      var registry = Registry.fromConnectionString(hubConnectionString);

      registry.create({deviceId: 'invalid/name' }, function(createErr) {
        assert.isNotNull(createErr);
        done();
      });
    });

    it('Lists devices and all test devices are there', function (done) {
      var registry = Registry.fromConnectionString(hubConnectionString);
      registry.list(function(err, deviceList) {
        if (err) {
          done(err);
        } else {
          var found = 0;

          deviceList.forEach(function (device) {
            if (device.deviceId === deviceIdOnly.deviceId) {
              found++;
            }
            if (device.deviceId === deviceIdWithKeys.deviceId) {
              found++;
            }
            if (device.deviceId === deviceIdWithThumbprints.deviceId) {
              found++;
            }
          });

          if (found === 3) {
            done();
          } else {
            done(new Error('One device not found'));
          }
        }
      });
    });

    it('Deletes a device and then fails to get it', function (done) {
      var registry = Registry.fromConnectionString(hubConnectionString);
      registry.delete(deviceIdOnly.deviceId, function(delErr){
        if(delErr) {
          done(delErr);
        } else {
          registry.get(deviceIdOnly.deviceId, function (getErr) {
            assert.instanceOf(getErr, errors.DeviceNotFoundError);
            done();
          });
        }
      });
    });

    it('Lists devices and one one device remains', function(done) {
      var registry = Registry.fromConnectionString(hubConnectionString);
      registry.list(function(err, deviceList) {
        if (err) {
          done(err);
        } else {
          var foundOne = false;
          var foundTwo = false;

          deviceList.forEach(function (device) {
            if (device.deviceId === deviceIdOnly.deviceId) {
              foundOne = true;
            }
            if (device.deviceId === deviceIdWithKeys.deviceId) {
              foundTwo = true;
            }
          });

          if (foundOne) {
            done(new Error('Device is still there'));
          } else if (!foundTwo) {
            done(new Error('Device missing'));
          } else {
            done();
          }
        }
      });
    });

    it('Updates the device and gets it', function (done) {
      var registry = Registry.fromConnectionString(hubConnectionString);
      deviceIdWithKeys.authentication.symmetricKey.secondaryKey = new Buffer('qwertyuiopasdfghjkl').toString('base64');
      registry.update(deviceIdWithKeys, function (updateErr, updatedDevice) {
        if (updateErr) {
          done(updateErr);
        } else {
          assert.equal(updatedDevice.authentication.symmetricKey.secondaryKey, deviceIdWithKeys.authentication.symmetricKey.secondaryKey);
          registry.get(deviceIdWithKeys.deviceId, function(getErr, getResult) {
            if (getErr) {
              done(getErr);
            } else {
              assert.equal(getResult.authentication.symmetricKey.secondaryKey, deviceIdWithKeys.authentication.symmetricKey.secondaryKey);
              done();
            }
          });
        }
      });
    });

    it('Fails to delete a device if it doesn\'t exist', function(done) {
      var registry = Registry.fromConnectionString(hubConnectionString);
      registry.delete('doesntexist' + uuid.v4(), function(delErr){
        assert.instanceOf(delErr, errors.DeviceNotFoundError);
        done();
      });
    });

    [deviceIdWithKeys.deviceId, deviceIdWithThumbprints.deviceId].forEach(function(deviceId) {
      it('Deletes device created for the test with id ' + deviceId, function (done) {
        var registry = Registry.fromConnectionString(hubConnectionString);
        registry.delete(deviceId, function(delErr){
          if(delErr) {
            done(delErr);
          } else {
            registry.get(deviceId, function (getErr) {
              assert.instanceOf(getErr, errors.DeviceNotFoundError);
              done();
            });
          }
        });
      });
    });
    

    it.skip('Imports then exports devices', function(done) {
      this.timeout(120000);

      var testDeviceCount = 10;
      var registry = Registry.fromConnectionString(hubConnectionString);
      var blobSvc = azureStorage.createBlobService(storageConnectionString);

      var inputContainerName = 'nodee2e-import-' + uuid.v4();
      var outputContainerName = 'nodee2e-export-' + uuid.v4();
      var deviceFile = 'devices.txt';

      var inputBlobSasUrl, outputBlobSasUrl;
      var devicesToImport = [];

      for (var i = 0; i < testDeviceCount; i++) {
        var deviceId = 'nodee2e-' + uuid.v4();
        var device = {
          id: deviceId,
          authentication: {
            symmetricKey: {
              primaryKey: new Buffer(uuid.v4()).toString('base64'),
              secondaryKey: new Buffer(uuid.v4()).toString('base64')
            }
          },
          status: 'enabled'
        };

        devicesToImport.push(device);
      }


      var devicesText = '';
      devicesToImport.forEach(function (device) {
        device.importMode = 'createOrUpdate';
        devicesText += JSON.stringify(device) + '\r\n';
      });

      debug('Devices to import: ' + devicesText);

      var createContainers = function() {
        return new Promise(function(resolve, reject) {
          debug('Create input container');
          blobSvc.createContainerIfNotExists(inputContainerName, function (err) {
            if(err) {
              reject(new Error('Could not create input container: ' + err.message));
            } else {
              var startDate = new Date();
              var expiryDate = new Date(startDate);
              expiryDate.setMinutes(startDate.getMinutes() + 100);
              startDate.setMinutes(startDate.getMinutes() - 100);

              var inputSharedAccessPolicy = {
                AccessPolicy: {
                  Permissions: 'rl',
                  Start: startDate,
                  Expiry: expiryDate
                },
              };

              var outputSharedAccessPolicy = {
                AccessPolicy: {
                  Permissions: 'rwd',
                  Start: startDate,
                  Expiry: expiryDate
                },
              };

              var inputSasToken = blobSvc.generateSharedAccessSignature(inputContainerName, null, inputSharedAccessPolicy);
              inputBlobSasUrl = blobSvc.getUrl(inputContainerName, null, inputSasToken);
              debug('Create output container');
              blobSvc.createContainerIfNotExists(outputContainerName, function (err) {
                if (err) {
                    reject(new Error('Could not create output container: ' + err.message));
                } else {
                    var outputSasToken = blobSvc.generateSharedAccessSignature(outputContainerName, null, outputSharedAccessPolicy);
                    outputBlobSasUrl = blobSvc.getUrl(outputContainerName, null, outputSasToken);
                    resolve();
                }
              });
            }
          });
        });
      };

      var deleteContainers = function() {
        return new Promise(function (resolve, reject) {
          var blobSvc = azureStorage.createBlobService(storageConnectionString);
          debug('Delete input container');
          blobSvc.deleteContainer(inputContainerName, function (err) {
            if(err) {
              reject(new Error('Could not delete input container: ' + err.message));
            } else {
              debug('Delete output container');
              blobSvc.deleteContainer(outputContainerName, function (err) {
                if(err) {
                  reject(new Error('Could not delete output container: ' + err.message));
                } else {
                  resolve();
                }
              });
            }
          });
        });
      };

      var verifyDeviceProperties = function (importedDevice, exportedDevice) {
        return importedDevice.id === exportedDevice.id &&
               importedDevice.authentication.symmetricKey.primaryKey === exportedDevice.authentication.symmetricKey.primaryKey &&
               importedDevice.authentication.symmetricKey.secondaryKey === exportedDevice.authentication.symmetricKey.secondaryKey &&
               importedDevice.status === exportedDevice.status;
      };

      var verifyOutputBlob = function() {
        return new Promise(function (resolve, reject){
          debug('Verifying export blob');
          blobSvc.getBlobToText(outputContainerName, deviceFile, function(err, result) {
            if(err) {
              reject(new Error('Could not get export blob: ' + err.message));
            } else {
              var devicesTextTable = result.split('\r\n');
              var exportedDevices = [];
              for (var i = 0; i < devicesTextTable.length; i++) {
                debug('[' + i + '] ' + devicesTextTable[i]);
                if (devicesTextTable[i].trim()) {
                    exportedDevices.push(JSON.parse(devicesTextTable[i]));
                }
              }

              for (i = 0; i < devicesToImport.length; i++) {
                var deviceFound = false;
                for (var j = 0; j < exportedDevices.length; j++) {
                  if (verifyDeviceProperties(devicesToImport[i], exportedDevices[j])) {
                    deviceFound = true;
                    debug('Found device: ' + devicesToImport[i].id);
                    break;
                  }
                }

                if (!deviceFound) {
                  reject(new Error('Could not find ' + devicesToImport[i].id));
                }
              }
              debug('Found all devices, cleaning device registry...');
              resolve();
            }
          });
        });
      };

      var runExportJob = function() {
        return new Promise(function (resolve, reject){
          debug('Starting export job');
          registry.exportDevicesToBlob(outputBlobSasUrl, false, function (err, result) {
            if(err) {
              reject(new Error('Could not create export job: ' + err.message));
            } else {
              debug('Export job created');
              var exportJobId = result.jobId;
              var jobFinished = false;
              var exportInterval = setInterval(function() {
                registry.getJob(exportJobId, function (err, result) {
                  if (err) {
                    reject(new Error('Could not get export job status: ' + err.message));
                  } else {
                    var status = result.status;
                    if (status === "completed" && !jobFinished) {
                      jobFinished = true;
                      debug('Export job completed');
                      clearInterval(exportInterval);
                      resolve();
                    }
                  }
                });
              }, 1000);
            }
          });
        });
      };

      var runImportJob = function(devicesText) {
        return new Promise(function (resolve, reject){
          blobSvc.createBlockBlobFromText(inputContainerName, deviceFile, devicesText, function (err) {
            if(err) {
              reject(new Error('Could not create blob for import job: ' + err.message));
            } else {
              debug('Starting import job');
              registry.importDevicesFromBlob(inputBlobSasUrl, outputBlobSasUrl, function (err, result) {
                if(err) {
                  reject(new Error('Could not create import job: ' + err.message));
                } else {
                  var importJobId = result.jobId;
                  var jobFinished = false;
                  var importInterval = setInterval(function () {
                    registry.getJob(importJobId, function (err, result) {
                      if (err) {
                        reject(new Error('Could not get import job status: ' + err.message));
                      } else {
                        var status = result.status;
                        if (status === "completed" && !jobFinished) {
                          jobFinished = true;
                          debug('Import job completed');
                          clearInterval(importInterval);
                          resolve();
                        }
                      }
                    });
                  }, 1000);
                }
              });
            }
          });
        });
      };

      var deleteTestDevices = function() {
        return new Promise(function (resolve, reject){
          debug('Cleaning devices');
          var devicesText = '';
          devicesToImport.forEach(function (device) {
            device.importMode = 'delete';
            devicesText += JSON.stringify(device) + '\r\n';
          });

          runImportJob(devicesText)
          .then(resolve)
          .catch(reject);
        });
      };

      createContainers()
      .then(function() { return runImportJob(devicesText); })
      .then(runExportJob)
      .then(verifyOutputBlob)
      .then(deleteTestDevices)
      .then(deleteContainers)
      .then(function() { done(); })
      .catch(function(err) { done(err); });
    });
  });
};

module.exports = runTests;
