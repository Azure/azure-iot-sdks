// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var endpoint = require('azure-iot-common').endpoint;

/**
 * @class                  module:azure-iothub.DeviceMethod
 * @classdesc              Constructs a DeviceMethod object that provides APIs to trigger the execution of a device method.
 * @param {Object}         params              An object describing the method and shall have the following properties: 
 *                                             - methodName          The name of the method that shall be invoked.
 *                                             - payload             [optional] The payload to use for the method call.
 *                                             - timeoutInSeconds    [optional] The number of seconds IoT Hub shall wait for the device 
 *                                                                   to send a response before deeming the method execution a failure.
 * @param {RestApiClient}  restApiClient       The REST client used to execute API calls.
 */
function DeviceMethod(params, restApiClient) {
  if (!params) throw new ReferenceError('params cannot be \'' + params + '\'');
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_004: [The `DeviceMethod` constructor shall throw a `ReferenceError` if `methodName` is `null`, `undefined` or an empty string.]*/
  if (params.methodName === null || params.methodName === undefined || params.methodName === '') throw new ReferenceError('params.methodName cannot be \'' + params.methodName + '\'');
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_005: [The `DeviceMethod` constructor shall throw a `TypeError` if `methodName` is not a `string`.]*/
  if (typeof params.methodName !== 'string') throw new TypeError('methodName must be a string');

  this._client = restApiClient;

  this.params = params;
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_006: [The `DeviceMethod` constructor shall set the `DeviceMethod.params.timeoutInSeconds` property value to the `timeoutInSeconds` argument value or to the default (`30`) if the `timeoutInSeconds` value is falsy.]*/
  this.params.timeoutInSeconds = this.params.timeoutInSeconds || DeviceMethod.defaultTimeout;
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_015: [The `DeviceMethod` constructor shall set the `DeviceMethod.params.payload` property value to the `payload` argument value or to the default (`null`) if the `payload` argument is `null` or `undefined`.]*/
  this.params.payload = (this.params.payload === undefined || this.params.payload === null) ? DeviceMethod.defaultPayload : this.params.payload;
}

DeviceMethod.defaultTimeout = 30;
DeviceMethod.defaultPayload = null;

/**
 * @method            module:azure-iothub.DeviceMethod.invokeOn
 * @description       Invokes the method on the specified device with the specified payload.
 * @param {String}    deviceId    Identifier of the device on which the method will run.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with three
 *                                arguments: an Error object (can be null), a
 *                                {@link module:azure-iothub.Twin|Twin}
 *                                object representing the created device
 *                                identity, and a transport-specific response
 *                                object useful for logging or debugging.
 */
DeviceMethod.prototype.invokeOn = function (deviceId, done) {
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_008: [The `invokeOn` method shall throw a `ReferenceError` if `deviceId` is `null`, `undefined` or an empty string.]*/
  if (deviceId === null || deviceId === undefined || deviceId === '') throw new ReferenceError('deviceId cannot be \'' + deviceId + '\'');

  var path = '/twins/' + deviceId + '/methods' + endpoint.versionQueryString();

  var headers = {
    'Content-Type': 'application/json; charset=utf-8'
  };

  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_011: [The `invokeOn` method shall construct an HTTP request using information supplied by the caller, as follows:
  ```
  POST /twins/<deviceId>/methods?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature> 
  Content-Type: application/json; charset=utf-8
  Request-Id: <guid>
  {
    "methodName": <DeviceMethod.params.methodName>,
    "timeoutInSeconds": <DeviceMethod.params.timeoutInSeconds>,
    "payload": <DeviceMethod.params.payload>
  }
  ```]*/
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_009: [The `invokeOn` method shall invoke the `done` callback with an standard javascript `Error` object if the method execution failed.]*/
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_010: [The `invokeOn` method shall invoke the `done` callback with a `null` first argument, a result second argument and a transport-specific response third argument if the method execution succeede**/
  this._client.executeApiCall('POST', path, headers, this.params, this.params.timeoutInSeconds * 1000, done);
};

module.exports = DeviceMethod;