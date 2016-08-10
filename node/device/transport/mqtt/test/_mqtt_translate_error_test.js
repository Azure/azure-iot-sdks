// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var errors = require('azure-iot-common').errors;
var translateError = require('../lib/mqtt-translate-error.js');

describe('MqttTranslateError', function () {

  it ('creates a properly constructed error', function() {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_001: [** Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 2 properties:
      - `message` shall contain a human-readable error message
      - `transportError` shall contain the MQTT error object **]** */
      
      var transportError = new Error('Dana');
      var agnosticError = translateError(transportError);

      assert(agnosticError.message.constructor.name === 'String');
      assert(agnosticError.transportError ===transportError);
  });


  it('converts client disconnecting error', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_002: [** `translateError` shall return a `NotConnectedError` if the MQTT error message contains the string 'client disconnecting' **]** */
    assert(translateError(Error('client disconnecting')) instanceof errors.NotConnectedError);
  });


  it('converts invalid topic error', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_003: [** `translateError` shall return a `FormatError` if the MQTT error message contains the string 'Invalid topic' **]** */
    assert(translateError(new Error('Invalid topic')) instanceof errors.FormatError);
  });

  it('converts no connection to broker error', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_004: [** `translateError` shall return a `NotConnectedError` if the MQTT error message contains the string 'No connection to broker' **]** */
    assert(translateError(new Error('No connection to broker')) instanceof errors.NotConnectedError);
  });

  it('converts unacceptable protocol version error', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_005: [** `translateError` shall return a `NotImplementedError` if the MQTT error message contains the string 'Unacceptable protocol version' **]**  */
    assert(translateError(new Error('Unacceptable protocol version')) instanceof errors.NotImplementedError);
  });

  it('converts identifier rejected error', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_006: [** `translateError` shall return a `UnauthorizedError` if the MQTT error message contains the string 'Identifier rejected' **]**  */
    assert(translateError(new Error('Identifier rejected')) instanceof errors.UnauthorizedError);
  });

  it('converts server unavilable error', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_007: [** `translateError` shall return a `ServiceUnavailableError` if the MQTT error message contains the string 'Server unavailable' **]**  */
    assert(translateError(new Error('Server unavailable')) instanceof errors.ServiceUnavailableError);
  });

  it('converts bad username or password error', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_008: [** `translateError` shall return a `UnauthorizedError` if the MQTT error message contains the string 'Bad username or password' **]**  */
    assert(translateError(new Error('Bad username or password')) instanceof errors.UnauthorizedError);
  });

  it('converts not authorized error', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_009: [** `translateError` shall return a `UnauthorizedError` if the MQTT error message contains the string 'Not authorized' **]** */
    assert(translateError(new Error('Not authorized')) instanceof errors.UnauthorizedError);
  });

  it('converts unrecognized packet type error', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_010: [** `translateError` shall return a `InternalServerError` if the MQTT error message contains the string 'unrecognized packet type' **]**  */
    assert(translateError(new Error('unrecognized packet type')) instanceof errors.InternalServerError);
  });

  it('returns generic error when strings dont match', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_ERRORS_18_011: [** `translateError` shall return an `Error` if none of the other string rules match **]** */
    assert(translateError(new Error('Zuul')) instanceof Error);
  });
});

 
