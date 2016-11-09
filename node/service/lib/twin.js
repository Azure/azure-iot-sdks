// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var errors = require('azure-iot-common').errors;
var _ = require('lodash');

/**
 * @class                  module:azure-iothub.Twin
 * @classdesc              Constructs a Twin object that provides APIs to update
 *                         parts of the twin of a device in the IoT Hub service.
 * @param {string|Object}  device      A device identifier string or an object describing the device. If an Object,
 *                                     it must contain a deviceId property.
 * @param {Registry}       registryClient   The HTTP registry client used to execute REST API calls.
 */
function Twin(device, registryClient) {
  /*Codes_SRS_NODE_IOTHUB_TWIN_16_002: [The `Twin(device, registryClient)` constructor shall throw a `ReferenceError` if `device` is falsy.]*/
  if (device === null || device === undefined || device === '') throw new ReferenceError('\'device\' cannot be \'' + device + '\'');
  /*Codes_SRS_NODE_IOTHUB_TWIN_16_003: [The `Twin(device, registryClient)` constructor shall throw a `ReferenceError` if `registryClient` is falsy.]*/
  if (!registryClient) throw new ReferenceError('\'registryClient\' cannot be \'' + registryClient + '\'');

  if (typeof(device) === 'string') {
    /*Codes_SRS_NODE_IOTHUB_TWIN_16_001: [The `Twin(device, registryClient)` constructor shall initialize an empty instance of a `Twin` object and set the `deviceId` base property to the `device` argument if it is a `string`.]*/
    this.deviceId = device;
  } else {
    if(!device.deviceId) {
      /*Codes_SRS_NODE_IOTHUB_TWIN_16_007: [The `Twin(device, registryClient)` constructor shall throw an `ArgumentError` if `device` is an object and does not have a `deviceId` property.]*/
      throw new errors.ArgumentError('\'device\' must have a deviceId property');
    } else {
      /*Codes_SRS_NODE_IOTHUB_TWIN_16_006: [The `Twin(device, registryClient)` constructor shall initialize an empty instance of a `Twin` object and set the properties of the created object to the properties described in the `device` argument if it's an `object`.]*/
      _.merge(this, device);
    }
  }

  /*Codes_SRS_NODE_IOTHUB_TWIN_16_005: [The `Twin(device, registryClient)` constructor shall set the `Twin.etag` to `*`.]*/
  this.etag = this.etag || '*';
  this.tags = this.tags || {};

  this.properties = _.assign({ desired: {} }, this.properties);

  this._registry = registryClient;
}

/**
 * @method            module:azure-iothub.Twin.get
 * @description       Gets the latest version of this device twin from the IoT Hub service.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with three
 *                                arguments: an Error object (can be null), a
 *                                {@link module:azure-iothub.Twin|Twin}
 *                                object representing the created device
 *                                identity, and a transport-specific response
 *                                object useful for logging or debugging.
 */
Twin.prototype.get = function(done) {
  /*Codes_SRS_NODE_IOTHUB_TWIN_16_020: [The `get` method shall call the `getTwin` method of the `Registry` instance stored in `_registry` property with the following parameters:
  - `this.deviceId`
  - `done`]*/
  var self = this;
  self._registry.getTwin(self.deviceId, function(err, result, response) {
    if(err) {
    /*Codes_SRS_NODE_IOTHUB_TWIN_16_022: [The method shall call the `done` callback with an `Error` object if the request failed]*/
      done(err);
    } else {
      /*Codes_SRS_NODE_IOTHUB_TWIN_16_021: [The method shall copy properties, tags, and etag in the twin returned in the callback of the `Registry` method call into its parent object.]*/
      self.properties = result.properties;
      self.tags = result.tags;
      self.etag = result.etag;
      /*Codes_SRS_NODE_IOTHUB_TWIN_16_023: [The method shall call the `done` callback with a `null` error object, its parent instance as a second argument and the transport `response` object as a third argument if the request succeeded.]*/
      done(null, self, response);
    }
  });
};

/**
 * @method            module:azure-iothub.Twin.update
 * @description       Update the device twin with the patch provided as argument.
 * @param {Object}    patch       Object containing the new values to apply to this device twin.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with three
 *                                arguments: an Error object (can be null), a
 *                                {@link module:azure-iothub.Twin|Twin}
 *                                object representing the created device
 *                                identity, and a transport-specific response
 *                                object useful for logging or debugging.
 */
Twin.prototype.update = function(patch, done) {
  /*Codes_SRS_NODE_IOTHUB_TWIN_16_019: [The `update` method shall call the `updateTwin` method of the `Registry` instance stored in `_registry` property with the following parameters:
  - `this.deviceId`
  - `patch`
  - `this.etag`
  - `done`]*/
  var self = this;
  self._registry.updateTwin(self.deviceId, patch, self.etag, function(err, result, response) {
    if(err) {
      /*Codes_SRS_NODE_IOTHUB_TWIN_16_022: [The method shall call the `done` callback with an `Error` object if the request failed]*/
      done(err);
    } else {
      /*Codes_SRS_NODE_IOTHUB_TWIN_16_021: [The method shall copy properties, tags, and etag in the twin returned in the callback of the `Registry` method call into its parent object.]*/
      self.properties = result.properties;
      self.tags = result.tags;
      self.etag = result.etag;
      /*Codes_SRS_NODE_IOTHUB_TWIN_16_023: [The method shall call the `done` callback with a `null` error object, its parent instance as a second argument and the transport `response` object as a third argument if the request succeeded.]*/
      done(null, self, response);
    }
  });
};

/*Codes_SRS_NODE_IOTHUB_TWIN_16_015: [The `toJSON` method shall return a copy of the `Twin` object that doesn't contain the `_registry` private property.]*/
Twin.prototype.toJSON = function() {
  // The registry object has a reference to https which has circular references, so we need to remove it from the JSON.
  var serializable = {};
  _.merge(serializable, this);
  serializable._registry = undefined;

  return serializable;
};

module.exports = Twin;
