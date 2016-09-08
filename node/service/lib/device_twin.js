// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var errors = require('azure-iot-common').errors;
var endpoint = require('azure-iot-common').endpoint;
var _ = require('lodash');

/**
 * @class                  module:azure-iothub.DeviceTwin
 * @classdesc              Constructs a DeviceTwin object that provides APIs to update
 *                         parts of the twin of a device in the IoT Hub service.
 * @param {string|Object}  device      A device identifier string or an object describing the device. If an Object,
 *                                     it must contain a deviceId property.
 * @param {Registry}       registryClient   The HTTP registry client used to execute REST API calls.
 */
function DeviceTwin(device, registryClient) {
  /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_002: [The `DeviceTwin(device, registryClient)` constructor shall throw a `ReferenceError` if `device` is falsy.]*/
  if (device === null || device === undefined || device === '') throw new ReferenceError('\'device\' cannot be \'' + device + '\'');
  /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_003: [The `DeviceTwin(device, registryClient)` constructor shall throw a `ReferenceError` if `registryClient` is falsy.]*/
  if (!registryClient) throw new ReferenceError('\'registryClient\' cannot be \'' + registryClient + '\'');

  if (typeof(device) === 'string') {
    /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_001: [The `DeviceTwin(device, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the `deviceId` base property to the `device` argument if it is a `string`.]*/
    this.deviceId = device;
  } else {
    if(!device.deviceId) {
      /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_007: [The `DeviceTwin(device, registryClient)` constructor shall throw an `ArgumentError` if `device` is an object and does not have a `deviceId` property.]*/
      throw new errors.ArgumentError('\'device\' must have a deviceId property');
    } else {
      /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_006: [The `DeviceTwin(device, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the properties of the created object to the properties described in the `device` argument if it's an `object`.]*/
      _.merge(this, device);
    }
  }

  /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_005: [The `DeviceTwin(device, registryClient)` constructor shall set the `DeviceTwin.etag`, `DeviceTwin.tags.$version` and `DeviceTwin.properties.desired.$version` to `*`.]*/
  this.etag = this.etag || '*';
  this.tags = this.tags || { $version: '*' };

  this.properties = _.assign({ desired: { $version: '*' } }, this.properties);

  this._registry = registryClient;
  
  var self = this;
  /**
   * @method            module:azure-iothub.DeviceTwin.tags.replace
   * @description       Replaces tags stored in the IoT Hub service for this device twin.
   * @param {Object}    newTags     Object containing the new tags to apply to the device twin.
   * @param {Boolean}   force       Boolean indicating whether the replacement should be forced 
   *                                even if the twin changed between the time it was 
   *                                retrieved and the time of this update.
   * @param {Function}  done        The function to call when the operation is
   *                                complete. `done` will be called with three
   *                                arguments: an Error object (can be null), a
   *                                {@link module:azure-iothub.DeviceTwin|DeviceTwin}
   *                                object representing the created device
   *                                identity, and a transport-specific response
   *                                object useful for logging or debugging.
   */
  this.tags.replace = function(newTags, force, done) {
    throwOnInvalidArgs(newTags, force, done);
    var ifMatch = calculateIfMatchValue(self.tags.$version, force);
    var callback = getCallbackArg(force, done);

    /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_009: [The `replace` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    PUT /twins/<DeviceTwin.deviceId>/tags?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Content-Type: application/json; charset=utf-8
    Request-Id: <guid>
    If-Match: <DeviceTwin.tags.$version> | *

    <tags object>
    ``]*/
    var path = '/twins/' + self.deviceId + '/tags' + endpoint.versionQueryString();
    var headers = {
      'Content-Type': 'application/json; charset=utf-8',
      'If-Match': ifMatch
    };

    self._registry._executeApiCall('PUT', path, headers, newTags, callback);
  };
}

/**
 * @method            module:azure-iothub.DeviceTwin.get
 * @description       Gets the latest version of this device twin from the IoT Hub service.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with three
 *                                arguments: an Error object (can be null), a
 *                                {@link module:azure-iothub.DeviceTwin|DeviceTwin}
 *                                object representing the created device
 *                                identity, and a transport-specific response
 *                                object useful for logging or debugging.
 */
DeviceTwin.prototype.get = function(done) {
  /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_014: [The `get` method shall construct an HTTP request using information supplied by the caller, as follows:
  ```
  GET /twins/<DeviceTwin.deviceId>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  Request-Id: <guid>
  ```]*/
  var self = this;
  var path = "/twins/" + self.deviceId + endpoint.versionQueryString();
  self._registry._executeApiCall('GET', path, null, null, function(err, newTwin, response) {
    if (err) {
      done(err);
    } else {
      _.merge(self, newTwin);
      done(null, self, response);
    }
  });
};

/*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_015: [The `toJSON` method shall return a copy of the `DeviceTwin` object that doesn't contain the `_registry` private property.]*/
DeviceTwin.prototype.toJSON = function() {
  // The registry object has a reference to https which has circular references, so we need to remove it from the JSON.
  var serializable = {};
  _.merge(serializable, this);
  serializable._registry = undefined;

  return serializable;
};

function throwOnInvalidArgs(patch, force, done) {
  /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_008: [The method shall throw a `ReferenceError` if the patch object is falsy.]*/
  if(!patch) throw new ReferenceError('The patch object cannot be \'' + patch + '\'');

  /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_012: [The method shall throw a ReferenceError if the `force` argument does not meet one of the following conditions:
  - `force` is boolean and `true`,
  - `force` is boolean and `false`.
  - `force` is a function and `done` is `undefined`
  - `force` is `undefined` and `done` is `undefined`]*/
  if (!(force === true ||
      force === false ||
      force === undefined ||
      (typeof(force) === 'function' && done === undefined))) {
    throw new ReferenceError('invalid second parameter');
  }
}

function calculateIfMatchValue(etag, force) {
  /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_011: [The method shall throw an `InvalidEtagError` if the `force` argument is set to false and `DeviceTwin.tags.$version` is undefined or set to `*`.]*/
  if (force === false && etag === '*') {
    throw new errors.InvalidEtagError('\'force\' cannot be \'false\' if \'tags.$version\' is \'*\'');
  }

  /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_010: [The method shall set the `If-Match` header to `*` if the `force` argument is `true`]*/
  return force ? '*' : etag;
}

function getCallbackArg(force, done) {
  /*Codes_SRS_NODE_IOTHUB_DEVICETWIN_16_013: [The `force` argument shall be optional, in other words, the method shall use the `force` argument as the callback if `done` is `undefined` and `force` is a function.]*/
  return (!done && typeof(force) === 'function') ? force : done;
}

module.exports = DeviceTwin;