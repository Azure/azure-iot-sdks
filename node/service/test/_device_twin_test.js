// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

"use strict";

var assert = require('chai').assert;
var sinon = require('sinon');
var errors = require('azure-iot-common').errors;
var Registry = require('../lib/registry.js');
var DeviceTwin = require('../lib/device_twin.js');

var fakeConfig = { host: 'host', sharedAccessSignature: 'sas' };
var testRegistry = new Registry(fakeConfig, {});

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

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_005: [The `DeviceTwin(device, registryClient)` constructor shall set the `DeviceTwin.etag` to `*`.]*/
    ['deviceId', { deviceId: 'deviceId' }].forEach(function(device) {
      var twin = new DeviceTwin(device, testRegistry);
      assert.instanceOf(twin, DeviceTwin);
      assert.equal(twin.etag, '*');
    });
  });

  describe('get', function() {
    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_020: [The `get` method shall call the `getDeviceTwin` method of the `Registry` instance stored in `_registry` property with the following parameters:
    - `this.deviceId`
    - `done`]*/
    it('calls the getDeviceTwin method on the Registry', function() {
      var fakeDeviceId = 'deviceId';
      var registry = new Registry(fakeConfig, {});
      var twin = new DeviceTwin(fakeDeviceId, registry);

      sinon.stub(registry, 'getDeviceTwin');

      twin.get(function() {});
      assert(registry.getDeviceTwin.calledWith(fakeDeviceId));
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_021: [The method shall merge the twin returned in the callback of the `Registry` method call with its parent object.]*/
    it('merges the result of the getDeviceTwin call with the current instance', function(testCallback) {
      var fakeDeviceId = 'deviceId';
      var registry = new Registry(fakeConfig, {});
      var twin = new DeviceTwin(fakeDeviceId, registry);

      sinon.stub(registry, 'getDeviceTwin', function(deviceId, callback) {
        callback(new Error('fake error'));
      });

      twin.get(function(err) {
        assert.instanceOf(err, Error);
        testCallback();
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_022: [The method shall call the `done` callback with an `Error` object if the request failed]*/
    it('calls the done callback with an Error object if the request failed', function(testCallback) {
      var fakeDeviceId = 'deviceId';
      var fakeTwinUpdate = { 
        deviceId: fakeDeviceId,
        tags: {
          update: 42
        } 
      };
      var fakeResponse = { statusCode: 200 };
      var registry = new Registry(fakeConfig, {});
      var twin = new DeviceTwin(fakeDeviceId, registry);

      sinon.stub(registry, 'getDeviceTwin', function(deviceId, callback) {
        callback(null, fakeTwinUpdate, fakeResponse);
      });

      twin.get(function() {
        assert.deepEqual(twin.tags, fakeTwinUpdate.tags);
        testCallback();
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_023: [The method shall call the `done` callback with a `null` error object, its parent instance as a second argument and the transport `response` object as a third argument if the request succeede**/
    it('calls the done callback with a null error object, a twin and a response', function(testCallback) {
      var fakeDeviceId = 'deviceId';
      var fakeTwin = { deviceId: fakeDeviceId };
      var fakeResponse = { statusCode: 200 };
      var registry = new Registry(fakeConfig, {});
      var twin = new DeviceTwin(fakeDeviceId, registry);

      sinon.stub(registry, 'getDeviceTwin', function(deviceId, callback) {
        callback(null, fakeTwin, fakeResponse);
      });

      twin.get(function(err, twin, resp) {
        assert.isNull(err);
        assert.equal(twin.deviceId, fakeDeviceId);
        assert.equal(resp, fakeResponse);
        testCallback();
      });
    });
  });

  describe('update', function() {
    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_019: [The `update` method shall call the `updateDeviceTwin` method of the `Registry` instance stored in `_registry` property with the following parameters:
    - `this.deviceId`
    - `patch`
    - `this.etag`
    - `done`]*/
    it('calls the updateDeviceTwin method on the Registry', function() {
      var fakeDeviceId = 'deviceId';
      var fakeEtag = 'etag==';
      var fakePatch = {
        tags: {
          fake: 'fake'
        }
      };

      var registry = new Registry(fakeConfig, {});
      var twin = new DeviceTwin(fakeDeviceId, registry);

      sinon.stub(registry, 'updateDeviceTwin');

      twin.etag = fakeEtag;
      twin.update(fakePatch, function() {});
      assert(registry.updateDeviceTwin.calledWith(fakeDeviceId, fakePatch, fakeEtag));
    });

    
    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_021: [The method shall merge the twin returned in the callback of the `Registry` method call with its parent object.]*/
    it('merges the result of the updateDeviceTwin call with the current instance', function(testCallback) {
      var fakeDeviceId = 'deviceId';
      var registry = new Registry(fakeConfig, {});
      var twin = new DeviceTwin(fakeDeviceId, registry);

      sinon.stub(registry, 'updateDeviceTwin', function(deviceId, patch, etag, callback) {
        callback(new Error('fake error'));
      });

      twin.update({}, function(err) {
        assert.instanceOf(err, Error);
        testCallback();
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_022: [The method shall call the `done` callback with an `Error` object if the request failed]*/
    it('calls the done callback with an Error object if the request failed', function(testCallback) {
      var fakeDeviceId = 'deviceId';
      var fakeTwinUpdate = { 
        deviceId: fakeDeviceId,
        tags: {
          update: 42
        } 
      };
      var fakeResponse = { statusCode: 200 };
      var registry = new Registry(fakeConfig, {});
      var twin = new DeviceTwin(fakeDeviceId, registry);

      sinon.stub(registry, 'updateDeviceTwin', function(deviceId, patch, etag, callback) {
        callback(null, fakeTwinUpdate, fakeResponse);
      });

      twin.update(fakeTwinUpdate, function() {
        assert.deepEqual(twin.tags, fakeTwinUpdate.tags);
        testCallback();
      });
    });

    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_023: [The method shall call the `done` callback with a `null` error object, its parent instance as a second argument and the transport `response` object as a third argument if the request succeede**/
    it('calls the done callback with a null error object, a twin and a response', function(testCallback) {
      var fakeDeviceId = 'deviceId';
      var fakeTwin = { deviceId: fakeDeviceId };
      var fakeResponse = { statusCode: 200 };
      var registry = new Registry(fakeConfig, {});
      var twin = new DeviceTwin(fakeDeviceId, registry);

      sinon.stub(registry, 'updateDeviceTwin', function(deviceId, patch, etag, callback) {
        callback(null, fakeTwin, fakeResponse);
      });

      twin.update({}, function(err, twin, resp) {
        assert.isNull(err);
        assert.equal(twin.deviceId, fakeDeviceId);
        assert.equal(resp, fakeResponse);
        testCallback();
      });
    });
  });

  describe('toJSON', function() {
    /*Tests_SRS_NODE_IOTHUB_DEVICETWIN_16_015: [The `toJSON` method shall return a copy of the `DeviceTwin` object that doesn't contain the `_registry` private property.]*/
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