// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var Device = require('./device.js');
var Registry = require('./registry.js');
var SimulatedHttps = require('azure-iot-common').SimulatedHttps;

var deviceJson = JSON.stringify(
  {
     deviceId: 'testDevice',
     status: 'Disabled'
  }
);

function badConfigTests(opName, badConnStrings, transportFactory, requestFn) {

  /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_003: [When the get method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a Device object representing the device information returned from IoT Hub.]*/
  function makeRequestWith(connString, test, done) {
    var transport = transportFactory();
    var registry = new Registry(connString, transport);
    requestFn(registry, function (err, res, msg, dev) {
      test(err, res, dev);
      done();
    });
  }

  /*Tests_SRS_NODE_IOTHUB_HTTPS_05_011: [If getDevice encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  function expectNotFoundError(err) {
      assert.equal(err.message, 'getaddrinfo ENOTFOUND bad');
  }

  /*Tests_SRS_NODE_IOTHUB_HTTPS_05_012: [When getDevice receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object
  res - the Node.js http.ServerResponse object returned by the transport]*/
  function expect401Response(err, res) {
    assert.isNotNull(err);
    assert.equal(res.statusCode, 401);
  }

  var tests = [
    { name: 'hostname is malformed', expect: expectNotFoundError },
    { name: 'policy does not exist', expect: expect401Response },
    { name: 'key is wrong', expect: expect401Response }
  ];

  badConnStrings.forEach(function (test, index, array) {
    it('fails to ' + opName + ' when the ' + tests[index].name, function (done) {
      makeRequestWith(test, tests[index].expect, done);
    });
  });
}

function runTests(transportFactory, goodConfig, badConfigs, deviceId) {

  describe('Registry', function () {
    describe('#create', function () {
      /*Tests_SRS_NODE_IOTHUB_HTTPS_05_010: [The sendHttpInfo method shall construct an HTTP request using information supplied by the caller, as follows:
      PUT <config.host>/devices/<id>?api-version=<version> HTTP/1.1
      Authorization: <token generated from config>
      iothub-name: config.hubname
      Host: <config.host>
      ]*/
      /*Test_SRS_NODE_IOTHUB_REGISTRY_07_001: [The create method shall throw a ReferenceError if the supplied deviceId is not valid.] */
      it('fails on invalid deviceId', function(done) {
        var registry = new Registry(goodConfig, transportFactory());
        var deviceInfo = new Device(null);
        assert.throws(function() {
          registry.create(deviceInfo, function(err, response) {
          });
        }, ReferenceError, 'Invalid argument \'deviceId\'');
        done();
      });

      /*Test_SRS_NODE_IOTHUB_REGISTRY_07_002: [When the create method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a Device object representing the new device information created from IoT Hub.] */
      it('creates a new device', function(done) {
        var registry = new Registry(goodConfig, transportFactory());
        var deviceInfo = new Device(deviceJson);
        registry.create(deviceInfo, function(err, response, dev) {
          assert.isNull(err);
          assert.instanceOf(dev, Device);
          assert.equal(deviceInfo.deviceId, dev.deviceId);
          done();
        });
      });

      badConfigTests('create device information', badConfigs, transportFactory, function (registry, done) {
        var deviceInfo = new Device(deviceJson);
        registry.create(deviceInfo, done);
      });
    });

    describe('#get', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_001: [The Registry constructor shall accept a transport object]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_002: [The get method shall request metadata for the device (indicated by the id argument) from the IoT Hub registry service via the transport associated with the Registry instance.]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_003: [When the get method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a Device object representing the device information returned from IoT Hub.]*/
      /*Tests_SRS_NODE_IOTHUB_HTTPS_05_010: [The retrieveHttpInfo method shall construct an HTTP request using information supplied by the caller, as follows:
      GET <config.host>/devices/<id>?api-version=<version> HTTP/1.1
      Authorization: <token generated from config>
      iothub-to: /devices/<id>
      Content-Type: 'application/json; charset=utf-8'
      Host: <config.host>
      ]*/
      it('returns information about the given device', function (done) {
        var registry = new Registry(goodConfig, transportFactory());
        registry.get(deviceId, function (err, res, dev) {
          assert.isNull(err);
          assert.instanceOf(dev, Device);
          assert.equal(deviceId, dev.deviceId);
          done();
        });
      });

      badConfigTests('get device information', badConfigs, transportFactory, function (registry, done) {
        registry.get(deviceId, done);
      });
    });

    describe('#list', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_005: [When the list method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a list of Device object in the form of JSON objects.]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_006: [The returned JSON object will be converted to a list of Device objects.] */
      /*Tests_SRS_NODE_IOTHUB_HTTPS_05_010: [The retrieveHttpInfo method shall construct an HTTP request using information supplied by the caller, as follows:
      GET <config.host>/devices/<id>?api-version=<version> HTTP/1.1
      Authorization: <token generated from config>
      iothub-to: /devices/<id>
      Host: <config.host>
      ]*/
      it('returns information about a list of devices', function(done) {
        var registry = new Registry(goodConfig, transportFactory());
        registry.list(function(err, response, deviceList) {
          assert.isNull(err);
          assert.isArray(deviceList);
          done();
        });
      });

      badConfigTests('list device information', badConfigs, transportFactory, function (registry, done) {
        registry.list(done);
      });
    });

    describe('#update', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_003: [The update method shall throw a ReferenceError if the supplied deviceInfo.deviceId is not valid.] */
      /*Tests_SRS_NODE_IOTHUB_HTTPS_07_010: [The sendHttpInfo method shall construct an HTTP request using information supplied by the caller, as follows:
      PUT <config.host>/devices/<id>?api-version=<version> HTTP/1.1
      Authorization: <token generated from config>
      iothub-name: config.hubname
      Content-Type: 'application/json; charset=utf-8'
      Host: <config.host>
      ]*/
      it('fails on invalid deviceId', function(done) {
        var registry = new Registry(goodConfig, transportFactory());
        var deviceInfo = new Device(null);
        assert.throws(function() {
          registry.update(deviceInfo, function(err, response) {
          });
        }, ReferenceError, 'Invalid argument \'deviceId\'');
        done();
      });

      /*Test_SRS_NODE_IOTHUB_REGISTRY_07_004: [When the update method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a Device object representing the updated device information created from IoT Hub.]*/
      it('updates information about a devices', function(done) {
        var registry = new Registry(goodConfig, transportFactory());
        var device = new Device(deviceJson);
        registry.update(device, function(err, response, dev) {
          assert.isNull(err);
          assert.instanceOf(dev, Device);
          assert.equal(device.deviceId, dev.deviceId);
          done();
        });
      });

      badConfigTests('update device information', badConfigs, transportFactory, function (registry, done) {
        var device = new Device(deviceJson);
        registry.update(device, done);
      });
    });

    describe('#delete', function () {
      /*Tests_SRS_NODE_IOTHUB_HTTPS_07_010: [The retrieveHttpInfo method shall construct an HTTP request using information supplied by the caller, as follows:
      DELETE <config.host>/devices/<id>?api-version=<version> HTTP/1.1
      Authorization: <token generated from config>
      iothub-name: config.hubname
      If-Match: '*'
      Host: <config.host>
      ]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_007: [The delete method shall throw a ReferenceError if the supplied deviceId is not valid. When the delete method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback] */
      it('deletes device if its available', function(done) {
        var registry = new Registry(goodConfig, transportFactory());
        var deviceInfo = new Device(deviceJson);
        registry.delete(deviceInfo.deviceId, function(err, response) {
          assert.isNull(err);
          done();
        });
      });

      badConfigTests('delete device information', badConfigs, transportFactory, function (registry, done) {
        registry.delete(deviceId, done);
      });
    });
  });
}

function createTransport() {
  return new SimulatedHttps();
}

function makeConnectionString(host, policy, key) {
  return 'HostName='+host+';CredentialType=type;CredentialScope=scope;SharedAccessKeyName='+policy+';SharedAccessKey='+key;
}

var connectionString = makeConnectionString('host', 'policy', 'key');
var badConnStrings = [
  makeConnectionString('bad', 'policy', 'key'),
  makeConnectionString('host', 'bad', 'key'),
  makeConnectionString('host', 'policy', 'bad')
];
var deviceId = 'testDevice';

describe('Over simulated HTTPS', function () {
  runTests(createTransport, connectionString, badConnStrings, deviceId);
});

module.exports = runTests;
