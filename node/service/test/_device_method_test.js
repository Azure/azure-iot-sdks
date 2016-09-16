// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var endpoint = require('azure-iot-common').endpoint;
var DeviceMethod = require('../lib/device_method.js');

describe('DeviceMethod', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_IOTHUB_DEVICE_METHOD_16_004: [The `DeviceMethod` constructor shall throw a `ReferenceError` if `methodName` is `null`, `undefined` or an empty string.]*/
    [undefined, null, ''].forEach(function(badMethodName) {
      it('throws a \'ReferenceError\' if methodName is \'' + badMethodName + '\'', function() {
        assert.throws(function() {
          return new DeviceMethod(badMethodName, 30, {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICE_METHOD_16_005: [The `DeviceMethod` constructor shall throw a `TypeError` if `methodName` is not a `string`.]*/
    [{}, function() {}, 42].forEach(function(badMethodType) {
      it('throws a \'ReferenceError\' if methodName is \'' + typeof badMethodType + '\'', function() {
        assert.throws(function() {
          return new DeviceMethod(badMethodType, 30, {});
        }, TypeError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICE_METHOD_16_006: [The `DeviceMethod` constructor shall set the `DeviceMethod.timeoutInSeconds` property to the `timeoutInSeconds` argument value.]*/
    it('sets the DeviceMethod.timeoutInSeconds property to the timeOutInSeconds argument value', function() {
      var testTimeout = 42;
      var method = new DeviceMethod('foo', testTimeout, {});
      assert.equal(method.timeoutInSeconds, testTimeout);
    });

    /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_013: [The `DeviceMethod` constructor shall set the `DeviceMethod.name` property to the `methodName` argument value.]*/
    it('sets the DeviceMethod.name property to the methodName argument value', function() {
      var methodName = 'foo';
      var method = new DeviceMethod(methodName, 42, {});
      assert.equal(method.name, methodName);
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICE_METHOD_16_001: [The `DeviceMethod` constructor shall throw a `ReferenceError` if the `restApiClient` argument is falsy.]*/
    [undefined, null, '', 0].forEach(function(badRestClient) {
      it('throw a ReferenceError if the restApiClient argument is \'' + badRestClient + '\'', function() {
        assert.throws(function(){
          return new DeviceMethod('foo', 42, badRestClient);
        }, ReferenceError);
      });
    });
  });

  describe('#invokeOn', function() {
    /*Tests_SRS_NODE_IOTHUB_DEVICE_METHOD_16_008: [The `invokeOn` method shall throw a `ReferenceError` if `deviceId` is `null`, `undefined` or an empty string.]*/
    [undefined, null, ''].forEach(function(badDeviceId) {
      it('throws a ReferenceError if \'deviceId\' is \'' + badDeviceId + '\'', function() {
        var method = new DeviceMethod('foo', 42, {});
        assert.throws(function() {
          method.invokeOn(badDeviceId, {}, function() {});
        });
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICE_METHOD_16_009: [The `invokeOn` method shall invoke the `done` callback with an standard javascript `Error` object if the method execution failed.]*/
    it('calls the done callback with an Error object if the request fails', function(testCallback) {
      var fakeError = new Error('Fake failure');
      var fakeRestClientFails = {
        executeApiCall: function(method, path, headers, body, timeout, callback) {
          callback(fakeError);
        }
      };

      var method = new DeviceMethod('foo', 42, fakeRestClientFails);
      method.invokeOn('deviceId', {}, function(err) {
        assert.equal(err, fakeError);
        testCallback();
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICE_METHOD_16_010: [The `invokeOn` method shall invoke the `done` callback with a `null` first argument, a result second argument and a transport-specific response third argument if the method execution succeede**/
    it('calls the done callback with a null first argument, a result second argument and a transport-specific resonse third argument', function(testCallback) {
      var fakeResult = {
        status: 'success'
      };
      var fakeResponse = {
        statusCode: 200
      };
      var fakeRestClientSucceeds = {
        executeApiCall: function(method, path, headers, body, timeout, callback) {
          callback(null, fakeResult, fakeResponse);
        }
      };

      var method = new DeviceMethod('foo', 42, fakeRestClientSucceeds);
      method.invokeOn('deviceId', {}, function(err, result, response) {
        assert.isNull(err);
        assert.equal(result, fakeResult);
        assert.equal(response, fakeResponse);
        testCallback();
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICE_METHOD_16_011: [The `invokeOn` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    POST /twins/<deviceId>/methods?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature> 
    Content-Type: application/json; charset=utf-8
    Request-Id: <guid>
    {
      "methodName": <DeviceMethod.name>,
      "timeoutInSeconds": <DeviceMethod.timeout>,
      "payloadJson": <payload>
    }
    ```]*/
    it('builds a correct HTTP request', function(testCallback) {
      var fakeMethodName = 'method';
      var fakeTimeout = 42;
      var fakePayload = { foo: 'bar' };
      var fakeDeviceId = 'deviceId';

      var fakeRestClient = {
        executeApiCall: function(method, path, headers, body, timeout, callback) {
          assert.equal(method, 'POST');
          assert.equal(path, '/twins/' + fakeDeviceId + '/methods' + endpoint.versionQueryString());
          assert.equal(headers['Content-Type'], 'application/json; charset=utf-8');
          assert.equal(body.methodName, fakeMethodName);
          assert.equal(body.timeoutInSeconds, fakeTimeout);
          assert.equal(body.payloadJson, fakePayload);
          assert.equal(timeout, fakeTimeout * 1000);
          callback();
        }
      };

      var method = new DeviceMethod(fakeMethodName, fakeTimeout, fakeRestClient);
      method.invokeOn(fakeDeviceId, fakePayload, testCallback);
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICE_METHOD_16_012: [If `payload` is a function and `done` is `undefined`, `payload` shall be used as the callback and the actual payload shall be set to `null`.]*/
    it('payload is an optional argument', function(testCallback) {
      var fakeRestClient = {
        executeApiCall: function(method, path, headers, body, timeout, callback) {
          assert.isNull(body.payloadJson);
          callback();
        }
      };

      var method = new DeviceMethod('fakeMethodName', 42, fakeRestClient);
      method.invokeOn('fakeDeviceId', testCallback);
    });
  });
});