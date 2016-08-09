// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var errors = require('azure-iot-common').errors;

/**
 * @method          module:azure-iot-device-mqtt.translateError
 * @description     Convert an error returned by MQTT.js into a transport-agnistic error.
 *
 * @param {Object}        mqttError the error returned by the MQTT.js library
 * @return {Object}   transport-agnostic error object
 */

/* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_001: [** Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 2 properties:
- `message` shall contain a human-readable error message
- `transportError` shall contain the MQTT error object **]** */

var translateError = function translateError(mqttError) {
  var err;

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_002: [** `translateError` shall return a `NotConnectedError` if the MQTT error message contains the string 'client disconnecting' **]** */
  if (mqttError.message.indexOf('client disconnecting') > -1) {
    err = new errors.NotConnectedError('mqtt.js returned ' + mqttError.message + ' error');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_003: [** `translateError` shall return a `FormatError` if the MQTT error message contains the string 'Invalid topic' **]** */
  else if (mqttError.message.indexOf('Invalid topic') > -1) {
    err = new errors.FormatError('mqtt.js returned ' + mqttError.message + ' error');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_004: [** `translateError` shall return a `NotConnectedError` if the MQTT error message contains the string 'No connection to broker' **]** */
  else if (mqttError.message.indexOf('No connection to broker') > -1) {
    err = new errors.NotConnectedError('mqtt.js returned ' + mqttError.message + ' error');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_005: [** `translateError` shall return a `NotImplementedError` if the MQTT error message contains the string 'Unacceptable protocol version' **]** */
  else if (mqttError.message.indexOf('Unacceptable protocol version') > -1) {
    err = new errors.NotImplementedError('mqtt.js returned ' + mqttError.message + ' error');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_006: [** `translateError` shall return a `UnauthorizedError` if the MQTT error message contains the string 'Identifier rejected' **]** */
  else if (mqttError.message.indexOf('Identifier rejected') > -1) {
    err = new errors.UnauthorizedError('mqtt.js returned ' + mqttError.message + ' error');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_007: [** `translateError` shall return a `ServiceUnavailableError` if the MQTT error message contains the string 'Server unavailable' **]** */
  else if (mqttError.message.indexOf('Server unavailable' )> -1) {
    err = new errors.ServiceUnavailableError('mqtt.js returned ' + mqttError.message + ' error');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_008: [** `translateError` shall return a `UnauthorizedError` if the MQTT error message contains the string 'Bad username or password' **]** */
  else if (mqttError.message.indexOf('Bad username or password') > -1) {
    err = new errors.UnauthorizedError('mqtt.js returned ' + mqttError.message + ' error');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_009: [** `translateError` shall return a `UnauthorizedError` if the MQTT error message contains the string 'Not authorized' **]** */
  else if (mqttError.message.indexOf('Not authorized') > -1) {
    err = new errors.UnauthorizedError('mqtt.js returned ' + mqttError.message + ' error');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_010: [** `translateError` shall return a `InternalServerError` if the MQTT error message contains the string 'unrecognized packet type' **]** */
  else if (mqttError.message.indexOf('unrecognized packet type') > -1) {
    err = new errors.InternalServerError('mqtt.js returned ' + mqttError.message + ' error');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_ERRORS_18_011: [** `translateError` shall return an `Error` if none of the other string rules match **]** */
  else {
    err = new Error('mqtt.js returned ' + mqttError.message + ' error');
  }
  
  err.transportError = mqttError;
  return err;
  
};

module.exports = translateError;

