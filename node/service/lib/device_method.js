// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var endpoint = require('azure-iot-common').endpoint;

/**
 * @class                  module:azure-iothub.DeviceMethod
 * @classdesc              Constructs a DeviceMethod object that provides APIs to trigger the execution of a device method.
 * @param {String}         methodName          The name of the method that shall be invoked.
 * @param {Number}         timeoutInSeconds    The number of seconds IoT Hub shall wait for the device 
 *                                             to send a response before deeming the method execution a failure.
 * @param {RestApiClient}  restApiClient       The REST client used to execute API calls.
 */
function DeviceMethod(methodName, timeoutInSeconds, restApiClient) {
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_004: [The `DeviceMethod` constructor shall throw a `ReferenceError` if `methodName` is `null`, `undefined` or an empty string.]*/
  if (methodName === null || methodName === undefined || methodName === '') throw new ReferenceError('methodName cannot be \'' + methodName + '\'');
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_005: [The `DeviceMethod` constructor shall throw a `TypeError` if `methodName` is not a `string`.]*/
  if (typeof methodName !== 'string') throw new TypeError('methodName must be a string');
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_001: [The `DeviceMethod` constructor shall throw a `ReferenceError` if the `restApiClient` argument is falsy.]*/
  if (!restApiClient) throw new ReferenceError('restApiClient cannot be \'' + restApiClient + '\'');
  this._client = restApiClient;

  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_013: [The `DeviceMethod` constructor shall set the `DeviceMethod.name` property to the `methodName` argument value.]*/
  this.name = methodName;

  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_006: [The `DeviceMethod` constructor shall set the `DeviceMethod.timeout` property to the `timeoutInSeconds` argument value.]*/
  this.timeout = timeoutInSeconds;
}

DeviceMethod.defaultTimeout = 30;

/**
 * @method            module:azure-iothub.DeviceMethod.invokeOn
 * @description       Invokes the method on the specified device with the specified payload.
 * @param {String}    deviceId    Identifier of the device on which the method will run.
 * @param {Object}    payload     Payload to be used when executing the method on the specified device.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with three
 *                                arguments: an Error object (can be null), a
 *                                {@link module:azure-iothub.DeviceTwin|DeviceTwin}
 *                                object representing the created device
 *                                identity, and a transport-specific response
 *                                object useful for logging or debugging.
 */
DeviceMethod.prototype.invokeOn = function (deviceId, payload, done) {
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_008: [The `invokeOn` method shall throw a `ReferenceError` if `deviceId` is `null`, `undefined` or an empty string.]*/
  if (deviceId === null || deviceId === undefined || deviceId === '') throw new ReferenceError('deviceId cannot be \'' + deviceId + '\'');

  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_012: [If `payload` is a function and `done` is `undefined`, `payload` shall be used as the callback and the actual payload shall be set to `null`.]*/
  if (typeof payload === 'function' && !done) {
    done = payload;
    payload = null;
  }

  var path = '/twins/' + deviceId + '/methods' + endpoint.versionQueryString();

  var headers = {
    'Content-Type': 'application/json; charset=utf-8'
  };

  var requestBody = {
    methodName: this.name,
    timeoutInSeconds: this.timeout,
    payloadJson: payload
  };

  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_011: [The `invokeOn` method shall construct an HTTP request using information supplied by the caller, as follows:
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
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_009: [The `invokeOn` method shall invoke the `done` callback with an standard javascript `Error` object if the method execution failed.]*/
  /*Codes_SRS_NODE_IOTHUB_DEVICE_METHOD_16_010: [The `invokeOn` method shall invoke the `done` callback with a `null` first argument, a result second argument and a transport-specific response third argument if the method execution succeede**/
  this._client.executeApiCall('POST', path, headers, requestBody, this.timeout * 1000, done);
};

module.exports = DeviceMethod;