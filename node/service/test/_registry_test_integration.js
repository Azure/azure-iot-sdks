// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var uuid = require('uuid');
var Registry = require('../lib/registry.js');
var ConnectionString = require('../lib/connection_string.js');

function makeConnectionString(host, policy, key) {
  return 'HostName=' + host + ';SharedAccessKeyName=' + policy + ';SharedAccessKey=' + key;
}

var goodConnectionString = process.env.IOTHUB_CONNECTION_STRING;
var cn = ConnectionString.parse(goodConnectionString);

var badConnectionStrings = [
  makeConnectionString('bad' + uuid.v4(), cn.SharedAccessKeyName, cn.SharedAccessKey),
  makeConnectionString(cn.HostName, 'bad', cn.SharedAccessKey),
  makeConnectionString(cn.HostName, cn.SharedAccessKeyName, 'bad'),
];

var basicDevice = {
  deviceId: 'testDevice' + Math.random(),
  status: 'disabled'
};

var x509Device = {
  deviceId: 'testDevice' + Math.random(),
  starts: 'disabled',
  authentication: {
    x509Thumbprint : {
      primaryThumbprint: '0000000000000000000000000000000000000000',
      secondaryThumbprint: '1111111111111111111111111111111111111111'
    }
  }
};

function badConfigTests(opName, badConnStrings, requestFn) {
  function makeRequestWith(connString, test, done) {
    var registry = Registry.fromConnectionString(connString);
    requestFn(registry, function (err, dev, res) {
      test(err, dev, res);
      done();
    });
  }

  function expectNotFoundError(err) {
    assert.include(err.message, 'getaddrinfo ENOTFOUND');
  }

  function expect401Response(err) {
    assert.isNotNull(err);
    assert.equal(err.response.statusCode, 401);
  }

  var tests = [
    { name: 'hostname is malformed', expect: expectNotFoundError },
    { name: 'policy does not exist', expect: expect401Response },
    { name: 'key is wrong', expect: expect401Response }
  ];

  badConnStrings.forEach(function (connStr, index) {
    it('fails to ' + opName + ' when the ' + tests[index].name, function (done) {
      makeRequestWith(connStr, tests[index].expect, done);
    });
  });
}

describe('Over real HTTPS', function () {
  this.timeout(60000);
  describe('Registry', function () {
    describe('#create', function () {
      [basicDevice, x509Device].forEach(function(device) {
        it('creates a new device with deviceId: ' + device.deviceId, function (done) {
          var registry = Registry.fromConnectionString(goodConnectionString);
          registry.create(device, function (err, dev) {
            if (err) {
              done(err);
            } else {
              assert.equal(device.deviceId, dev.deviceId);
              assert.notEqual(device, dev);
              done();
            }
          });
        });
      });

      badConfigTests('create device information', badConnectionStrings, function (registry, done) {
        registry.create(basicDevice, done);
      });
    });

    describe('#get', function () {
      it('returns information about the given device', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString);
        registry.get(basicDevice.deviceId, function (err, dev) {
          if (err) {
            done(err);
          } else {
            assert.equal(basicDevice.deviceId, dev.deviceId);
            basicDevice = dev;
            done();
          }
        });
      });

      badConfigTests('get device information', badConnectionStrings, function (registry, done) {
        registry.get(basicDevice.deviceId, done);
      });
    });

    describe('#list', function () {
      it('returns information about a list of devices', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString);
        registry.list(function (err, deviceList) {
          if (err) {
            done(err);
          } else {
            assert.isArray(deviceList);
            done();
          }
        });
      });

      badConfigTests('list device information', badConnectionStrings, function (registry, done) {
        registry.list(done);
      });
    });

    describe('#update', function () {
      it('updates information about a device', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString);
        registry.update(basicDevice, function (err, dev) {
          if (err) {
            done(err);
          } else {
            assert.equal(basicDevice.deviceId, dev.deviceId);
            done();
          }
        });
      });

      badConfigTests('update device information', badConnectionStrings, function (registry, done) {
        registry.update(basicDevice, done);
      });
    });

    describe('#delete', function () {
      [basicDevice, x509Device].forEach(function(device) {
        it('deletes the given device with deviceId: ' + device.deviceId, function (done) {
          var registry = Registry.fromConnectionString(goodConnectionString);
          registry.delete(device.deviceId, function (err) {
            done(err);
          });
        });
      });

      badConfigTests('delete device information', badConnectionStrings, function (registry, done) {
        registry.delete(basicDevice.deviceId, done);
      });
    });
  });
});
