// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/**
 * @class           module:azure-iot-device.deviceMethod.DeviceMethodResponse
 * @classdesc       `DeviceMethodResponse` provides functionality that allows
 *                  a device method implementation to construct and send a
 *                  response back to the service for a device method call. An
 *                  instance of this class is passed as the second parameter
 *                  to the callback registered via the
 *                  {@link module:azure-iot-device.Client#onDeviceMethod} method.
 *                  method.
 * @param {String}  requestId         The request identifier supplied by the
 *                                    service for this device method call.
 * @param {Object}  transport         An object that implements the interface
 *                                    expected of a transport object, e.g.,
 *                                    {@link module:azure-iot-device~Http|Http}.
 */
function DeviceMethodResponse(requestId, transport) {
  // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_002: [ DeviceMethodRequest shall throw an Error if requestId is an empty string. ]
  if(typeof(requestId) === 'string' && requestId.length === 0) {
    throw new Error('requestId must not be an empty string');
  }

  // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_001: [ DeviceMethodRequest shall throw a ReferenceError if requestId is falsy or is not a string. ]
  if(typeof(requestId) !== 'string') {
    throw new ReferenceError('requestId must be a string');
  }

  // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_006: [ DeviceMethodResponse shall throw a ReferenceError if transport is falsy. ]
  if (!transport) throw new ReferenceError('transport is \'' + transport + '\'');

  this.requestId = requestId;
  this.properties = {};
  this.bodyParts = [];
  this.isResponseComplete = false;
  this.status = null;
  this._transport = transport;
}

/**
 * @method            module:azure-iot-device.deviceMethod.DeviceMethodResponse#write
 * @description       Appends the data that's supplied to the output buffer to
 *                    be sent as the payload for the method's response.
 *
 * @param {String|Buffer}   methodName   The data to be appended to the output
 *                                        buffer.
 *
 * @throws {ReferenceError}       If the `data` parameter is falsy.
 * @throws {Error}                If the response was previously completed via a
 *                                a call to {@link module:azure-iot-device.deviceMethod.DeviceMethodResponse#end}.
 */
DeviceMethodResponse.prototype.write = function(data) {
  // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_005: [ DeviceMethodResponse.write shall throw an Error object if it is invoked after DeviceMethodResponse.end has been called ].
  if(this.isResponseComplete) {
    throw new Error('This response has already ended. Cannot write to it anymore.');
  }
  if(!!data) {
    // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_004: [ The DeviceMethodResponse.write method shall accumulate the data passed to it as internal state. ]
    this.bodyParts.push(Buffer.isBuffer(data) ? data : new Buffer(data));
  } else {
    // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_003: [ DeviceMethodResponse.write shall throw a ReferenceError if data is falsy. ]
    throw new ReferenceError('"data" is "' + data + '"');
  }
};

/**
 * @method            module:azure-iot-device.deviceMethod.DeviceMethodResponse#end
 * @description       Sends the device method's response back to the service via
 *                    the underlying transport object using the status parameter
 *                    as the status of the method call.
 *
 * @param {Number}    status      A numeric status code to be sent back to the
 *                                service.
 * @param {Function}  done        An optional callback function which will be
 *                                called once the response has been sent back to
 *                                the service. An error object is passed as an
 *                                argument to the function in case an error
 *                                occurs.
 *
 * @throws {ReferenceError}       If the `status` parameter is not a number.
 * @throws {Error}                If this response has already been sent to the
 *                                service in a previous call to it. This method
 *                                should be called only once.
 */
DeviceMethodResponse.prototype.end = function(status, done) {
  // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_009: [ DeviceMethodResponse.end shall throw an Error object if it is called more than once for the same request. ]
  if(this.isResponseComplete) {
    throw new Error('This response has already ended. Cannot end the same response more than once.');
  }

  if(typeof(status) === 'number') {
    this.status = status;
    this.isResponseComplete = true;
    // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_008: [ DeviceMethodResponse.end shall notify the service and supply the response for the request along with the status by calling sendMethodResponse on the underlying transport object. ]
    // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_010: [ DeviceMethodResponse.end shall invoke the callback specified by done if it is not falsy. ]
    // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_011: [ DeviceMethodResponse.end shall pass the status of sending the response to the service to done. ]
    this._transport.sendMethodResponse(this, done);
  } else {
    // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_007: [ DeviceMethodResponse.end shall throw a ReferenceError if status is undefined or not a number. ]
    throw new ReferenceError('"status" is "' + status + '". Expected a number.');
  }
};

module.exports = DeviceMethodResponse;
