// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

"use strict";

var assert = require('chai').assert;
var errors = require('azure-iot-common').errors;
var endpoint = require('azure-iot-common').endpoint;
var Registry = require('../lib/registry.js');
var DeviceTwin = require('../lib/device_twin.js');
var PackageJson = require('../package.json');

var fakeConfig = { host: 'host', sharedAccessSignature: 'sas' };
var testRegistry = new Registry(fakeConfig, {});

var fakeRegistryValidatesEtag = new Registry(fakeConfig, {
  buildRequest: function (method, path, httpHeaders) {
    assert.equal(httpHeaders['If-Match'], '*');
    return {
      write: function() { },
      end: function() { }
    };
  }
});

var fakeRegistryCallsBack = new Registry(fakeConfig, {
  buildRequest: function (method, path, httpHeaders, host, done) {
    assert.equal(httpHeaders['If-Match'], '*');
    return {
      write: function() { },
      end: function() {
        done();
      }
    };
  }
});

var fakeRegistryCallsBackWithError = new Registry(fakeConfig, {
  buildRequest: function (method, path, httpHeaders, host, done) {
    assert.equal(httpHeaders['If-Match'], '*');
    return {
      write: function() { },
      end: function() {
        done(new errors.DeviceNotFoundError('fake device not found'));
      }
    };
  }
});

describe('DeviceTwin', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_001: [The `DeviceTwin(device, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the `deviceId` base property to the `device` argument if it is a `string`.]*/
    it('initializes a new instance of the DeviceTwin object when device is a deviceId string', function() {
      var deviceId = 'fakeDeviceId';
      var twin = new DeviceTwin(deviceId, testRegistry);
      assert.instanceOf(twin, DeviceTwin);
      assert.equal(twin.deviceId, deviceId);
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_006: [The `DeviceTwin(device, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the properties of the created object to the properties described in the `device` argument if it's an `object`.]*/
    it('initializes a new instance of the DeviceTwin object when device is a deviceId string', function() {
      var device = {
        deviceId: 'fakeDeviceId'
      };

      var twin = new DeviceTwin(device, testRegistry);
      assert.instanceOf(twin, DeviceTwin);
      assert.equal(twin.deviceId, device.deviceId);
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_002: [The `DeviceTwin(device, registryClient)` constructor shall throw a `ReferenceError` if `device` is undefined, null or an empty string.]*/
    [undefined, null, ''].forEach(function(badDeviceId) {
      it('throws a ReferenceError if `device` is \'' + badDeviceId + '\'', function() {
        assert.throws(function() {
          return new DeviceTwin(badDeviceId, testRegistry);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_003: [The `DeviceTwin(device, registryClient)` constructor shall throw a `ReferenceError` if `registryClient` is falsy.]*/
    [undefined, null, '', 0].forEach(function(badRegistryClient) {
      it('throws a ReferenceError if `registryClient` is \'' + badRegistryClient + '\'', function() {
        assert.throws(function() {
          return new DeviceTwin('deviceId', badRegistryClient);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_007: [The `DeviceTwin(device, registryClient)` constructor shall throw an `ArgumentError` if `device` is an object and does not have a `deviceId` property.]*/
    it('throws an ArgumentError if `device` is an object and doesn\'t have a deviceId property', function() {
      assert.throws(function() {
        return new DeviceTwin({}, testRegistry);
      }, errors.ArgumentError);
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_005: [The `DeviceTwin(device, registryClient)` constructor shall set the `DeviceTwin.etag`, `DeviceTwin.tags.$version` and `DeviceTwin.properties.desired.$version` to `*`.]*/
    ['deviceId', { deviceId: 'deviceId' }].forEach(function(device) {
      var twin = new DeviceTwin(device, testRegistry);
      assert.instanceOf(twin, DeviceTwin);
      assert.equal(twin.etag, '*');
      assert.equal(twin.tags.$version, '*');
      assert.equal(twin.properties.desired.$version, '*');
    });
  });

  /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_008: [The method shall throw a `ReferenceError` if the patch object is falsy.]*/
  function throwsOnBadPatch(fn) {
    [undefined, null].forEach(function(badPatch) {
      it('throws a \'ReferenceError\' if the patch object is ' + badPatch + '\'', function() {
        assert.throws(function() {
          fn(badPatch, true, function() {});
        }, ReferenceError);
        assert.throws(function() {
          fn(badPatch, false, function() {});
        }, ReferenceError);
      });
    });
  }

  /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_011: [The method shall throw an `InvalidEtagError` if the `force` argument is set to false and the corresponding `$version` property is undefined or set to `*`.]*/
  function throwsInvalidEtag(fn) {
    it('throws a \'InvalidEtagError\' if the the \'force\' argument is false and the corresponding etag is set to \'*\'', function() {
      assert.throws(function() {
        fn({ foo: 'bar' }, false, function() {});
      }, errors.InvalidEtagError);
    });
  }

  /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_012: [The method shall throw a ReferenceError if the `force` argument does not meet one of the following conditions:
  - `force` is boolean and `true`,
  - `force` is boolean and `false`.
  - `force` is a function and `done` is `undefined`.
  - `force` is `undefined` and `done` is `undefined`]*/
  function throwsOnInvalidForce(fn) {
    [{}, '', null].forEach(function(badForce) {
      it('throws a \'ReferenceError\' if force is not a boolean or a function', function() {
        assert.throws(function() {
          fn({ foo: 'bar' }, badForce);
        });
        assert.throws(function() {
          fn({ foo: 'bar' }, badForce, function() {});
        });
      });
    });
  }

  /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_010: [The method shall set the `If-Match` header to `*` if the `force` argument is `true`]*/
  function verifyEtagIfForceIsTrue(fn) {
    it('verifies that \'If-Match\' is \'*\' if \'force\' is \'true\'', function() {
      fn({ foo: 'bar' }, true, function() {});
    });
  }

  /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_013: [** The method shall use the `force` argument as the callback if `done` is `undefined` and `force` is a function.]*/
  function verifyForceIsCallback(fn) {
    it('uses the \'force\' argument as the callback if \'done\' is \'undefined\' and \'force\' is a function', function(testCallback) {
      fn({ foo: 'bar' }, testCallback);
    });
  }

  function verifyCallbackWithError(fn) {
    it('calls the \'done\' callback with an \'Error\' if the request fails', function(testCallback) {
      fn({ foo: 'bar' }, function(err) {
        assert.instanceOf(err, errors.DeviceNotFoundError);
        testCallback();
      });
    });
  }

  describe('tags.replace', function() {
    throwsOnBadPatch(new DeviceTwin('deviceId', new Registry({ host: 'host', sharedAccessSignature: 'sas' })).tags.replace);
    throwsInvalidEtag(new DeviceTwin('deviceId', new Registry({ host: 'host', sharedAccessSignature: 'sas' })).tags.replace);
    throwsOnInvalidForce(new DeviceTwin('deviceId', new Registry({ host: 'host', sharedAccessSignature: 'sas' })).tags.replace);
    verifyEtagIfForceIsTrue(new DeviceTwin({
      deviceId: 'deviceId',
        tags: {
          etag: 'etag=='
        }
      }, fakeRegistryValidatesEtag).tags.replace);

    verifyForceIsCallback(new DeviceTwin('deviceId', fakeRegistryCallsBack).tags.replace);
    verifyCallbackWithError(new DeviceTwin('deviceId', fakeRegistryCallsBackWithError).tags.replace);

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_009: [The `replace` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    PUT /twins/<DeviceTwin.deviceId>/tags?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Content-Type: application/json; charset=utf-8
    Request-Id: <guid>
    If-Match: <DeviceTwin.tags.$version> | *

    <tags object>
    ``]*/
    it('creates a valid HTTP request', function(testCallback) {
      var fakeDeviceId = 'deviceId';
      var fakeTwinPatch = {
        foo: 'bar'
      };
      var fakeHttpResponse = { statusCode: 200 };
      var fakeEtag = 'etag==';

      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'PUT');
          assert.equal(path, '/twins/' + fakeDeviceId + '/tags' + endpoint.versionQueryString());
          assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows tracing of requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          assert.equal(httpHeaders['If-Match'], fakeEtag);
          return {
            write: function(body) {
              assert.equal(body, JSON.stringify(fakeTwinPatch));
            },
            end: function() {
              done(null, '', fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      var twin = new DeviceTwin(fakeDeviceId, registry);
      twin.tags.$version = fakeEtag;

      twin.tags.replace(fakeTwinPatch, false, testCallback);
    });
  });

  describe('get', function() {
    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_014: [The `get` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    GET /twins/<DeviceTwin.deviceId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Request-Id: <guid>
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeDeviceId = 'deviceId';
      var fakeDeviceTwin = {
        deviceId: fakeDeviceId
      };
      var fakeHttpResponse = { statusCode: 200 };
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'GET');
          assert.equal(path, '/twins/' + fakeDeviceId + endpoint.versionQueryString());
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function() {
              assert.fail();
            },
            end: function() {
              done(null, JSON.stringify(fakeDeviceTwin), fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.getDeviceTwin(fakeDeviceId, function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.deepEqual(result.deviceId, fakeDeviceId);
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });

  describe('toJSON', function() {
    /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_015: [The `toJSON` method shall return a copy of the `DeviceTwin` object that doesn't contain the `_registry` private property.]*/
    it('does not throw when calling JSON.stringify on a DeviceTwin object', function() {
      var twin = new DeviceTwin('deviceId', new Registry(fakeConfig));
      assert.doesNotThrow(function() {
        JSON.stringify(twin);
      });
    });

    it('returns the twin object without the _registry property', function() {
      var fakeDeviceTwin = {
        deviceId: 'deviceId',
        tags: {
          key1: 'value1'
        },
        properties: {
          desired: {
            key2: 'value2'
          },
          reported: {
            key3: 'value3'
          }
        }
      };

      var twin = new DeviceTwin(fakeDeviceTwin, new Registry(fakeConfig));
      var json = twin.toJSON();
      assert.isUndefined(json._registry);
      assert.equal(json.deviceId, fakeDeviceTwin.deviceId);
      assert.equal(json.tags.key1, fakeDeviceTwin.tags.key1);
      assert.equal(json.properties.desired.key2, fakeDeviceTwin.properties.desired.key2);
      assert.equal(json.properties.desired.key3, fakeDeviceTwin.properties.desired.key3);
    });
  });
});