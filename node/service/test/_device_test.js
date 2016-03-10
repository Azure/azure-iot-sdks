// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Device = require('../lib/device.js');

var deviceJson = JSON.stringify({ deviceId: 'testDevice', generationId:'123456789012345678', etag:'' });
var invalidDev = JSON.stringify({ deviceId: '' });
var initializeDevice = { 
  deviceId: 'fullDevice',
  generationId:'635756100743433650',
  etag:554111,
  connectionState:'Disconnected',
  status:'Enabled',
  statusReason:'Some Reason',
  connectionStateUpdatedTime:'2015-08-20T18:08:49.9738417',
  statusUpdatedTime:'0001-01-01T00:00:00',
  lastActivityTime:'2015-08-26T01:00:51.6950626',
  cloudToDeviceMessageCount:4,
  authentication:
    {symmetricKey: 
      {primaryKey:"aBcd+eFg9h3jKl2MNO4pQrS90TUVxYzabcdefGH6iJK=",secondaryKey:"ZaBcd+eFg9h3jKl2MNO4pQrS90TUVxYzabcdefGH6iJ="}}
};
var fullDevice = JSON.stringify(initializeDevice);
var deviceName = 'testDevice';

function throwsRedefineError(device, fieldName, descriptor) {
  assert.throws(function () {
    Object.defineProperty(device, fieldName, descriptor);
  }, TypeError, 'Cannot redefine property: '+fieldName);
}

describe('Device', function () {
  describe('#enumerable', function() {
    it('is enumerable', function () {
      var device = new Device(deviceJson);

      var allProps = [];
      for (var prop in device) {
        if (device.hasOwnProperty(prop)) {
          allProps.push(prop.toString());
        }
      }
      assert.include(allProps, 'deviceId');
      assert.include(allProps, 'generationId');
      assert.include(allProps, 'etag');
      assert.include(allProps, 'connectionState');
      assert.include(allProps, 'status');
      assert.include(allProps, 'statusReason');
      assert.include(allProps, 'connectionStateUpdatedTime');
      assert.include(allProps, 'statusUpdatedTime');
      assert.include(allProps, 'lastActivityTime');
      assert.include(allProps, 'cloudToDeviceMessageCount');
      assert.include(allProps, 'authentication');
    });
  });

  describe('#constructor', function () {
    it('creates a Device with the given id', function () {
      var device = new Device(deviceJson);
      assert.equal(device.deviceId, deviceName);
    });

    it('throws if \'deviceId\' is falsy', function () {
      function throwsReferenceError(ctorArg) {
        assert.throws(function () {
          return new Device(ctorArg);
        }, ReferenceError);
      }
      throwsReferenceError(invalidDev);
    });
    
    it('JSON is created correctly', function() {
      var device = new Device(fullDevice);
      
      assert.equal(device.deviceId, initializeDevice.deviceId);
      assert.equal(device.generationId, initializeDevice.generationId);
      assert.equal(device.etag, initializeDevice.etag);
      assert.equal(device.connectionState, initializeDevice.connectionState);
      assert.equal(device.status, initializeDevice.status);
      assert.equal(device.statusReason, initializeDevice.statusReason);
      assert.equal(device.connectionStateUpdatedTime, initializeDevice.connectionStateUpdatedTime);
      assert.equal(device.statusUpdatedTime, initializeDevice.statusUpdatedTime);
      assert.equal(device.lastActivityTime, initializeDevice.lastActivityTime);
      assert.equal(device.cloudToDeviceMessageCount, initializeDevice.cloudToDeviceMessageCount);
    });
  });

  describe('#deviceId', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      
      throwsRedefineError(device, 'deviceId', { configurable: true });
      throwsRedefineError(device, 'deviceId', { enumerable: false });
      throwsRedefineError(device, 'deviceId', { set: function () { } });
      throwsRedefineError(device, 'deviceId', { get: function () { return deviceName; } });
      throwsRedefineError(device, 'deviceId', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, 'deviceId', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.deviceId;
      }, TypeError, 'Cannot delete property \'deviceId\' of #<Device>');
    });
  });

  describe('#generationId', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      
      throwsRedefineError(device, 'generationId', { configurable: true });
      throwsRedefineError(device, 'generationId', { enumerable: false });
      throwsRedefineError(device, 'generationId', { set: function () { } });
      throwsRedefineError(device, 'generationId', { get: function () { return deviceName; } });
      throwsRedefineError(device, 'generationId', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, 'generationId', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.generationId;
      }, TypeError, 'Cannot delete property \'generationId\' of #<Device>');
    });
  });

  describe('#etag', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      
      throwsRedefineError(device, 'etag', { configurable: true });
      throwsRedefineError(device, 'etag', { enumerable: false });
      throwsRedefineError(device, 'etag', { set: function () { } });
      throwsRedefineError(device, 'etag', { get: function () { return deviceName; } });
      throwsRedefineError(device, 'etag', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, 'etag', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.etag;
      }, TypeError, 'Cannot delete property \'etag\' of #<Device>');
    });
  });

  describe('#connectionState', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      
      throwsRedefineError(device, 'connectionState', { configurable: true });
      throwsRedefineError(device, 'connectionState', { enumerable: false });
      throwsRedefineError(device, 'connectionState', { set: function () { } });
      throwsRedefineError(device, 'connectionState', { get: function () { return deviceName; } });
      throwsRedefineError(device, 'connectionState', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, 'connectionState', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.connectionState;
      }, TypeError, 'Cannot delete property \'connectionState\' of #<Device>');
    });
  });

  describe('#status', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      var fieldName = 'statusReason';

      throwsRedefineError(device, fieldName, { configurable: true });
      throwsRedefineError(device, fieldName, { enumerable: false });
      throwsRedefineError(device, fieldName, { set: function () { } });
      throwsRedefineError(device, fieldName, { get: function () { return deviceName; } });
      assert.throws(function () {
        Object.defineProperty(device, fieldName, { value: 'NotEnabledOrDisabled' });
        }, TypeError, 'Cannot redefine property: status');
      assert.doesNotThrow(function () {
        Object.defineProperty(device, fieldName, { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.status;
      }, TypeError, 'Cannot delete property \'status\' of #<Device>');
    });
    
    it('Assignments allowed to enable and disabled', function() {
      var device = new Device(deviceJson);
      assert.doesNotThrow(function() {
        device.status = 'Enabled';
        device.status = 'Disabled';
        device.status = 'enabled';
        device.status = 'disabled';
      });
    });
  });
  
  describe('#statusReason', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      var fieldName = 'statusReason';
      
      throwsRedefineError(device, fieldName, { configurable: true });
      throwsRedefineError(device, fieldName, { enumerable: false });
      throwsRedefineError(device, fieldName, { set: function () { } });
      throwsRedefineError(device, fieldName, { get: function () { return deviceName; } });
      throwsRedefineError(device, fieldName, { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, fieldName, { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.statusReason;
      }, TypeError, 'Cannot delete property \'statusReason\' of #<Device>');
    });
  });
  
  describe('#connectionStateUpdatedTime', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      var fieldName = 'connectionStateUpdatedTime';
      
      throwsRedefineError(device, fieldName, { configurable: true });
      throwsRedefineError(device, fieldName, { enumerable: false });
      throwsRedefineError(device, fieldName, { set: function () { } });
      throwsRedefineError(device, fieldName, { get: function () { return deviceName; } });
      throwsRedefineError(device, fieldName, { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, fieldName, { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.connectionStateUpdatedTime;
      }, TypeError, 'Cannot delete property \'connectionStateUpdatedTime\' of #<Device>');
    });
  });
  
  describe('#statusUpdatedTime', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      var fieldName = 'statusUpdatedTime';
      
      throwsRedefineError(device, fieldName, { configurable: true });
      throwsRedefineError(device, fieldName, { enumerable: false });
      throwsRedefineError(device, fieldName, { set: function () { } });
      throwsRedefineError(device, fieldName, { get: function () { return deviceName; } });
      throwsRedefineError(device, fieldName, { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, fieldName, { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.statusUpdatedTime;
      }, TypeError, 'Cannot delete property \'statusUpdatedTime\' of #<Device>');
    });
  });
  
  describe('#lastActivityTime', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      var fieldName = 'lastActivityTime';
      
      throwsRedefineError(device, fieldName, { configurable: true });
      throwsRedefineError(device, fieldName, { enumerable: false });
      throwsRedefineError(device, fieldName, { set: function () { } });
      throwsRedefineError(device, fieldName, { get: function () { return deviceName; } });
      throwsRedefineError(device, fieldName, { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, fieldName, { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.lastActivityTime;
      }, TypeError, 'Cannot delete property \'lastActivityTime\' of #<Device>');
    });
  });

  describe('#cloudToDeviceMessageCount', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      var fieldName = 'cloudToDeviceMessageCount';
      
      throwsRedefineError(device, fieldName, { configurable: true });
      throwsRedefineError(device, fieldName, { enumerable: false });
      throwsRedefineError(device, fieldName, { set: function () { } });
      throwsRedefineError(device, fieldName, { get: function () { return deviceName; } });
      throwsRedefineError(device, fieldName, { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, fieldName, { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.cloudToDeviceMessageCount;
      }, TypeError, 'Cannot delete property \'cloudToDeviceMessageCount\' of #<Device>');
    });
  });

  describe('#authentication', function () { 
    it('cannot be configured or deleted', function () {
      var device = new Device(deviceJson);
      var fieldName = 'authentication';
      
      throwsRedefineError(device, fieldName, { configurable: true });
      throwsRedefineError(device, fieldName, { enumerable: false });
      throwsRedefineError(device, fieldName, { set: function () { } });
      throwsRedefineError(device, fieldName, { get: function () { return deviceName; } });
      throwsRedefineError(device, fieldName, { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(device, fieldName, { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete device.authentication;
      }, TypeError, 'Cannot delete property \'authentication\' of #<Device>');
    });
  });
});
