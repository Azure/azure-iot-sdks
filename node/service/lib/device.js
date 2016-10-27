// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var _ = require('lodash');

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
  this.deviceId = null;
  this.generationId = null;
  this.etag = null;
  this.connectionState = 'Disconnected';
  this.status = 'Enabled';
  this.statusReason = null;
  this.connectionStateUpdatedTime = null;
  this.statusUpdatedTime = null;
  this.lastActivityTime = null;
  this.cloudToDeviceMessageCount = 0;
  this.authentication = {
    symmetricKey: {  // <- this is the correct thing (camel-cased) 
      primaryKey: null,
      secondaryKey: null
    },
    x509Thumprint: {
      primaryThumbprint: null,
      secondaryThumbprint: null
    }
  };

  if(jsonData) {
    var userProps = (typeof jsonData === 'string') ? JSON.parse(jsonData) : jsonData;
    if (!userProps.deviceId) {
      throw new ReferenceError('The \'deviceId\' property cannot be \'' + userProps.deviceId + '\'');
    }

    _.merge(this, userProps);
  }

  Object.defineProperty(this.authentication, 'SymmetricKey', {
    enumerable: true,
    get: function () {
      return this.authentication ? this.authentication.symmetricKey : undefined;
    }
  });
};
