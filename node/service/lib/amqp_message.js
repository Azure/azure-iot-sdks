// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

function AmqpMessage() { }

AmqpMessage.fromMessage = function fromMessage(message) {
  if (!message) throw new ReferenceError('message is \'' + message + '\'');

  /*Codes_SRS_NODE_IOTHUB_AMQPMSG_05_001: [The fromMessage method shall create a new instance of AmqpMessage.]*/
  var amqpMessage = new AmqpMessage();
  /*Codes_SRS_NODE_IOTHUB_AMQPMSG_05_002: [The created AmqpMessage object shall have a property of type Object named properties.]*/
  amqpMessage.properties = {};

  /*Codes_SRS_NODE_IOTHUB_AMQPMSG_05_003: [If the message argument has a to property, the properties property of the AmqpMessage object shall have a property named to with the same value.]*/
  if (message.to) {
    amqpMessage.properties.to = message.to;
  }
  /*Codes_SRS_NODE_IOTHUB_AMQPMSG_05_004: [If the message argument has an expiryTimeUtc property, the properties property of the AmqpMessage object shall have a property named absoluteExpiryTime with the same value.]*/
  if (message.expiryTimeUtc) {
    amqpMessage.properties.absoluteExpiryTime = message.expiryTimeUtc;
  }

  /*Codes_SRS_NODE_IOTHUB_AMQPMSG_05_005: [If message.getData() is truthy, the AmqpMessage object shall have a property named body with the value returned from message.getData().]*/
  var body = message.getData();
  if (body) {
    amqpMessage.body = body;
  }

  /*Codes_SRS_NODE_IOTHUB_AMQPMSG_05_006: [The generated AmqpMessage object shall be returned to the caller.]*/
  return amqpMessage;
};

module.exports = AmqpMessage;