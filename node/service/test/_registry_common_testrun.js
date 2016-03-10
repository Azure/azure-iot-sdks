// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var assert = require('chai').assert;
var Device = require('../lib/device.js');
var Registry = require('../lib/registry.js');
var RegistryHttp = require('../lib/registry_http.js');

var deviceJson = JSON.stringify({
  deviceId: 'testDevice' + Math.random(),
  status: 'Disabled'
});

function badConfigTests(opName, badConnStrings, Transport, requestFn) {

  /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_003: [When the get method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a Device object representing the new device identity returned from the IoT hub.]*/
  function makeRequestWith(connString, test, done) {
    var registry = Registry.fromConnectionString(connString, Transport);
    requestFn(registry, function (err, dev, res) {
      test(err, dev, res);
      done();
    });
  }

  /*Tests_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  function expectNotFoundError(err) {
    assert.include(err.message, 'getaddrinfo ENOTFOUND');
  }

  /*Tests_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js http.ServerResponse object attached as the property response.]*/
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

function runTests(Transport, goodConnectionString, badConnectionStrings, deviceId) {

  /*Tests_SRS_NODE_IOTHUB_HTTP_05_001: [The Http constructor shall accept an object with three properties:
  host – (string) the fully-qualified DNS hostname of an IoT hub
  hubName – (string) the name of the IoT hub, which is the first segment of hostname
  sharedAccessSignature – (string) a shared access signature generated from the credentials of a policy with the appropriate registry persmissions (read and/or write).]*/
  describe('Registry', function () {
    describe('#fromConnectionString', function () {
      var connStr = 'HostName=a.b.c;SharedAccessKeyName=name;SharedAccessKey=key';

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_008: [The fromConnectionString method shall throw ReferenceError if the value argument is falsy.]*/
      it('throws when value is falsy', function () {
        assert.throws(function () {
          return Registry.fromConnectionString();
        }, ReferenceError, 'value is \'undefined\'');
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_009: [Otherwise, it shall derive and transform the needed parts from the connection string in order to create a new instance of the default transport (azure-iothub.Http).]*/
      it('creates an instance of the default transport', function () {
        var registry = Registry.fromConnectionString(connStr);
        assert.instanceOf(registry._transport, RegistryHttp);
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_010: [The fromConnectionString method shall return a new instance of the Registry object, as by a call to new Registry(transport).]*/
      it('returns an instance of Registry', function () {
        var registry = Registry.fromConnectionString(connStr);
        assert.instanceOf(registry, Registry);
      });
    });

    describe('#fromSharedAccessSignature', function () {
      var token = 'SharedAccessSignature sr=audience&sig=signature&se=expiry&skn=keyname';

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_011: [The fromSharedAccessSignature method shall throw ReferenceError if the value argument is falsy.]*/
      it('throws when value is falsy', function () {
        assert.throws(function () {
          return Registry.fromSharedAccessSignature();
        }, ReferenceError, 'value is \'undefined\'');
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_012: [Otherwise, it shall derive and transform the needed parts from the shared access signature in order to create a new instance of the default transport (azure-iothub.Http).]*/
      it('creates an instance of the default transport', function () {
        var registry = Registry.fromSharedAccessSignature(token);
        assert.instanceOf(registry._transport, RegistryHttp);
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_013: [The fromSharedAccessSignature method shall return a new instance of the Registry object, as by a call to new Registry(transport).]*/
      it('returns an instance of Registry', function () {
        var registry = Registry.fromSharedAccessSignature(token);
        assert.instanceOf(registry, Registry);
      });
    });

    describe('#create', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_001: [The create method shall throw ArgumentError if the first argument does not contain a deviceId property.]*/
      it('throws when deviceInfo.deviceId is falsy', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        var deviceInfo = new Device(null);
        assert.throws(function () {
          registry.create(deviceInfo);
        }, ArgumentError, 'The object \'deviceInfo\' is missing the property: deviceId');
        done();
      });

      /*Tests_SRS_NODE_IOTHUB_HTTP_05_002: [The createDevice method shall construct an HTTP request using information supplied by the caller, as follows:
      PUT <path>?api-version=<version> HTTP/1.1
      Authorization: <config.sharedAccessSignature>
      iothub-name: <config.hubName>
      Content-Type: application/json; charset=utf-8
      If-Match: *
      Host: <config.host>

      <deviceInfo>
      ]
      */
      /*Tests_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
      err - null
      body – the body of the HTTP response
      response - the Node.js http.ServerResponse object returned by the transport]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_002: [When the create method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a Device object representing the new device identity returned from the IoT hub.]*/
      it('creates a new device', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        var deviceInfo = new Device(deviceJson);
        registry.create(deviceInfo, function (err, dev) {
          if (err) {
            done(err);
          } else {
            assert.instanceOf(dev, Device);
            assert.equal(deviceInfo.deviceId, dev.deviceId);
            assert.notEqual(deviceInfo, dev);
            done();
          }
        });
      });

      badConfigTests('create device information', badConnectionStrings, Transport, function (registry, done) {
        var deviceInfo = new Device(deviceJson);
        registry.create(deviceInfo, done);
      });
    });

    describe('#get', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_006: [The get method shall throw ReferenceError if the supplied deviceId is falsy.]*/
      it('throws if deviceId is falsy', function () {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        assert.throws(function () {
          registry.get();
        }, ReferenceError, 'deviceId is \'undefined\'');
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_002: [The get method shall request metadata for the device (indicated by the deviceId argument) from an IoT hub’s identity service via the transport associated with the Registry instance.]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_003: [When the get method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a Device object representing the device information returned from IoT Hub.]*/
      /*Tests_SRS_NODE_IOTHUB_HTTP_05_004: [The getDevice method shall construct an HTTP request using information supplied by the caller, as follows:
      GET <path>?api-version=<version> HTTP/1.1
      Authorization: <config.sharedAccessSignature>
      iothub-name: <config.hubName>
      Host: <config.host>
      ]
      */
      /*Tests_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
      err - null
      body – the body of the HTTP response
      response - the Node.js http.ServerResponse object returned by the transport]*/
      it('returns information about the given device', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        registry.get(deviceId, function (err, dev) {
          if (err) {
            done(err);
          } else {
            assert.instanceOf(dev, Device);
            assert.equal(deviceId, dev.deviceId);
            done();
          }
        });
      });

      badConfigTests('get device information', badConnectionStrings, Transport, function (registry, done) {
        registry.get(deviceId, done);
      });
    });

    describe('#list', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_004: [The list method shall request information about devices from an IoT hub’s identity service via the transport associated with the Registry instance.]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_006: [The JSON array returned from the service shall be converted to a list of Device objects.]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_005: [When the list method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and an array of Device objects representing up to 1000 devices from the IoT hub.]*/
      /*Tests_SRS_NODE_IOTHUB_HTTP_05_005: [The listDevices method shall construct an HTTP request using information supplied by the caller, as follows:
      GET <path>?api-version=<version> HTTP/1.1
      Authorization: <config.sharedAccessSignature>
      iothub-name: <config.hubName>
      Host: <config.host>
      ]
      */
      /*Tests_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
      err - null
      body – the body of the HTTP response
      response - the Node.js http.ServerResponse object returned by the transport]*/
      it('returns information about a list of devices', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        registry.list(function (err, deviceList) {
          if (err) {
            done(err);
          } else {
            assert.isArray(deviceList);
            done();
          }
        });
      });

      badConfigTests('list device information', badConnectionStrings, Transport, function (registry, done) {
        registry.list(done);
      });
    });

    describe('#update', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_003: [The update method shall throw ArgumentError if the first argument does not contain a deviceId property.]*/
      it('throws when deviceInfo.deviceId is falsy', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        var deviceInfo = new Device(null);
        assert.throws(function () {
          registry.update(deviceInfo);
        }, ArgumentError, 'The object \'deviceInfo\' is missing the property: deviceId');
        done();
      });

      /*Tests_SRS_NODE_IOTHUB_HTTP_05_003: [The updateDevice method shall construct an HTTP request using information supplied by the caller, as follows:
      PUT <path>?api-version=<version> HTTP/1.1
      Authorization: <config.sharedAccessSignature>
      iothub-name: <config.hubName>
      Content-Type: application/json; charset=utf-8
      Host: <config.host>

      <deviceInfo>
      ]
      */
      /*Tests_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
      err - null
      body – the body of the HTTP response
      response - the Node.js http.ServerResponse object returned by the transport]*/
      /*Test_SRS_NODE_IOTHUB_REGISTRY_07_004: [When the update method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a Device object representing the new device identity returned from the IoT hub.]*/
      it('updates information about a device', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        var device = new Device(deviceJson);
        registry.update(device, function (err, dev) {
          if (err) {
            done(err);
          } else {
            assert.instanceOf(dev, Device);
            assert.equal(device.deviceId, dev.deviceId);
            done();
          }
        });
      });

      badConfigTests('update device information', badConnectionStrings, Transport, function (registry, done) {
        var device = new Device(deviceJson);
        registry.update(device, done);
      });
    });

    describe('#delete', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_007: [The delete method shall throw ReferenceError if the supplied deviceId is falsy.]*/
      it('throws if deviceId is falsy', function () {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        assert.throws(function () {
          registry.delete();
        }, ReferenceError, 'deviceId is \'undefined\'');
      });

      /*Tests_SRS_NODE_IOTHUB_HTTP_05_006: [The deleteDevice method shall construct an HTTP request using information supplied by the caller, as follows:
      DELETE <path>?api-version=<version> HTTP/1.1
      Authorization: <config.sharedAccessSignature>
      iothub-name: <config.hubName>
      If-Match: *
      Host: <config.host>
      ]
      */
      /*Tests_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
      err - null
      body – the body of the HTTP response
      response - the Node.js http.ServerResponse object returned by the transport]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_007: [The delete method shall delete the given device from an IoT hub’s identity service via the transport associated with the Registry instance.]*/
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_005: [When the delete method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null).]*/
      it('deletes the given device', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        var deviceInfo = new Device(deviceJson);
        registry.delete(deviceInfo.deviceId, function (err) {
          done(err);
        });
      });

      badConfigTests('delete device information', badConnectionStrings, Transport, function (registry, done) {
        registry.delete(deviceId, done);
      });
    });
  });
}

module.exports = runTests;