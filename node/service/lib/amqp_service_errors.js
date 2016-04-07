// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var errors = require('azure-iot-common').errors;
var translateCommonError = require('azure-iot-amqp-base').translateError;

/*Codes_SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_010: [ `translateError` shall accept 2 argument:
*- A custom error message to give context to the user.
*- the AMQP error object itself]
*/
var translateError = function translateError(message, amqpError) {
  var error;
  /*Codes_SRS_NODE_DEVICE_AMQP_SERVICE_ERRORS_16_001: [ `translateError` shall return an `DeviceMaximumQueueDepthExceededError` if the AMQP error condition is `amqp:resource-limit-exceeded`.] */
  if (amqpError.constructor.name === 'AMQPError' && amqpError.condition.contents === 'amqp:resource-limit-exceeded') {
    error = new errors.DeviceMaximumQueueDepthExceededError(message);
  } else {
    error = translateCommonError(message, amqpError);
  }

  error.amqpError = amqpError;

  return error;
};

module.exports = translateError;