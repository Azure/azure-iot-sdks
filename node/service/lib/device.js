// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var SymmetricKey = function (symmetricKeyInfo) {
  var symmKeys = symmetricKeyInfo;
  Object.defineProperties(this, {
    'primaryKey': {
      enumerable: true,
      get: function () {
        var primaryKey;
        if (symmKeys) {
          primaryKey = symmKeys.primaryKey;
        }
        return primaryKey;
      },
    },
    'secondaryKey': {
      enumerable: true,
      get: function () {
        var secondaryKey;
        if (symmKeys) {
          secondaryKey = symmKeys.secondaryKey;
        }
        return secondaryKey;
      },
    }
  });
};

var AuthenticationMechanism = function (authInfo) {
  Object.defineProperties(this, {
    'SymmetricKey': {
      enumerable: true,
      get: function () {
        var symmKeys;
        if (authInfo) {
          if (authInfo.symmetricKey) {
            symmKeys = new SymmetricKey(authInfo.symmetricKey);
          }
        }
        return symmKeys;
      },
    }
  });
};

/**
 * @class           module:azure-iothub.Device
 * @classdesc       Creates a representation of a device for use in the
 *                  identity {@link module:azure-iothub.Registry} APIs.
 * @param {String}  jsonData        An optional JSON representation of the
 *                                  device, which will be mapped to properties
 *                                  in the object. If no argument is provided,
 *                                  Device roperties will be assigned default
 *                                  values.
 * @prop {String}   deviceId        Unique device identifier
 * @prop {String}   generationId    Used to disambiguate devices that have been
 *                                  deleted/recreated with the same `deviceId`
 * @prop {String}   etag            Weak entity tag assigned to this device
 *                                  identity description
 * @prop {String}   connectionState Whether the device is 'connected' or
 *                                  'disconnected'
 * @prop {String}   status          'enabled' (device authorized to connect,
 *                                  can send/receive messages) or 'disabled'
 * @prop {String}   statusReason    128-character string set when the device is
 *                                  disabled
 * @prop {String}   connectionStateUpdatedTime  Timestamp representing the last
 *                                  time `connectionState` changed
 * @prop {String}   statusUpdatedTime Timestamp representing the last time
 *                                  `status` changed
 * @prop {String}   lastActivityTime  Timestamp representing the last time the
 *                                  device authenticated, sent a message, or
 *                                  received a message
 * @prop {String}   cloudToDeviceMessageCount Number of c2d messages waiting to
 *                                  by delivered to the device
 * @prop {Object}   authentication  Contains the symmetric keys used to
 *                                  authenticate this device
 */
module.exports = function Device(jsonData) {
  var body;
  if (jsonData) {
    body = JSON.parse(jsonData);
    if (!body.deviceId) {
      throw new ReferenceError('Argument \'deviceId\' is ' + body.deviceId);
    }
  }
  else {
    body = {
      deviceId: null,
      generationId: null,
      etag: null,
      connectionState: 'Disconnected',
      status: 'Enabled',
      statusReason: null,
      connectionStateUpdatedTime: null,
      statusUpdatedTime: null,
      lastActivityTime: null,
      cloudToDeviceMessageCount: 0,
      authentication: {
        symmetricKey: {
          primaryKey: null,
          secondaryKey: null
        }
      }
    };
  }

  Object.defineProperties(this, {
    'deviceId': {
      enumerable: true,
      get: function () {
        return body.deviceId;
      },
      set: function (value) {
        body.deviceId = value;
      }
    },
    'generationId': {
      enumerable: true,
      get: function () {
        return body.generationId;
      },
    },
    'etag': {
      enumerable: true,
      get: function () {
        return body.etag;
      }
    },
    'connectionState': {
      enumerable: true,
      get: function () {
        return body.connectionState;
      }
    },
    'status': {
      enumerable: true,
      get: function () {
        return body.status;
      },
      set: function (value) {
        if (value.toLowerCase() !== 'enabled' && value.toLowerCase() !== 'disabled') {
          throw new RangeError('status is neither Enabled or Disabled');
        }
        body.status = value;
      }
    },
    'statusReason': {
      enumerable: true,
      get: function () {
        return body.statusReason;
      },
      set: function (value) {
        body.statusReason = value;
      }
    },
    'connectionStateUpdatedTime': {
      enumerable: true,
      get: function () {
        return body.connectionStateUpdatedTime;
      }
    },
    'statusUpdatedTime': {
      enumerable: true,
      get: function () {
        return body.statusUpdatedTime;
      }
    },
    'lastActivityTime': {
      enumerable: true,
      get: function () {
        return body.lastActivityTime;
      }
    },
    'cloudToDeviceMessageCount': {
      enumerable: true,
      get: function () {
        return body.cloudToDeviceMessageCount;
      }
    }
  });

  Object.defineProperty(this, 'authentication', {
    enumerable: true,
    get: function () {
      var key;
      if (body.authentication) {
        key = new AuthenticationMechanism(body.authentication);
      }
      return key;
    }
  });
};
