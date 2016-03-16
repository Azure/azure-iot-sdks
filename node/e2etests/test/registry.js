// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Registry = require('azure-iothub').Registry;

var assert = require('chai').assert;
var uuid = require('uuid');

var runTests = function (hubConnectionString) {
  var deviceIdOnly = {
    deviceId: uuid.v4()
  };

  var deviceIdWithKeys = {
    deviceId: uuid.v4(),
    authentication: {
      SymmetricKey: {
        primaryKey: new Buffer("1234567890qwerty").toString('base64'),
        secondaryKey: new Buffer("ytrewq0987654321").toString('base64'),
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
              assert.equal(getResult.authentication.SymmetricKey.primaryKey, deviceIdWithKeys.authentication.SymmetricKey.primaryKey);
              assert.equal(getResult.authentication.SymmetricKey.secondaryKey, deviceIdWithKeys.authentication.SymmetricKey.secondaryKey);
              assert.equal(getResult.disabled, deviceIdWithKeys.disabled);
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

    it('Lists devices and both test devices are there', function (done) {
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

          if (foundOne && foundTwo) {
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
            assert.isNotNull(getErr);
            assert.equal(getErr.message, "Not Found");
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
      deviceIdWithKeys.authentication.SymmetricKey.secondaryKey = new Buffer('qwertyuiopasdfghjkl').toString('base64');
      registry.update(deviceIdWithKeys, function (updateErr, updatedDevice) {
        if (updateErr) {
          done(updateErr);
        } else {
          assert.equal(updatedDevice.authentication.SymmetricKey.secondaryKey, deviceIdWithKeys.authentication.SymmetricKey.secondaryKey);
          registry.get(deviceIdWithKeys.deviceId, function(getErr, getResult) {
            if (getErr) {
              done(getErr);
            } else {
              assert.equal(getResult.authentication.SymmetricKey.secondaryKey, deviceIdWithKeys.authentication.SymmetricKey.secondaryKey);
              done();
            }
          });
        }
      });
    });

    it('Fails to delete a device if it doesn\'t exist', function(done) {
      var registry = Registry.fromConnectionString(hubConnectionString);
      registry.delete('doesntexist' + uuid.v4(), function(delErr){
        assert.isNotNull(delErr);
        assert.equal(delErr.message, "Not Found");
        done();
      });
    });

    it('Deletes the last device created for the test', function (done) {
      var registry = Registry.fromConnectionString(hubConnectionString);
      registry.delete(deviceIdWithKeys.deviceId, function(delErr){
        if(delErr) {
          done(delErr);
        } else {
          registry.get(deviceIdOnly.deviceId, function (getErr) {
            assert.isNotNull(getErr);
            assert.equal(getErr.message, "Not Found");
            done();
          });
        }
      });
    });
  });
};

module.exports = runTests;
