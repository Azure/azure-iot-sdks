// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/**
 * @class AmqpMessage
 * @classdesc Creates a new instance of the {@linkcode AmqpMessage} class.
 */
function AmqpMessage() { }

/**
 * Takes a {@linkcode Message} object and creates an AMQP message from it.
 * @param {Message}   message   The {@linkcode Message} object from which to create an AMQP message.
 */
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

  /*Codes_SRS_NODE_IOTHUB_AMQPMSG_05_007: [If the message argument has a messageId property, the properties property of the AmqpMessage object shall have a property named messageId with the same value.]*/
  if (message.messageId) {
    amqpMessage.properties.messageId = message.messageId;
  }

  /*Codes_SRS_NODE_IOTHUB_AMQPMSG_05_008: [If needed, the created AmqpMessage object shall have a property of type Object named applicationProperties.]*/
  function ensureApplicationPropertiesCreated() {
    if (!amqpMessage.applicationProperties) {
      amqpMessage.applicationProperties = {};
    }
  }

  /*Codes_SRS_NODE_IOTHUB_AMQPMSG_05_009: [If the message argument has an ack property, the applicationProperties property of the AmqpMessage object shall have a property named iothub-ack with the same value.]*/
  if (message.ack) {
    ensureApplicationPropertiesCreated();
    amqpMessage.applicationProperties['iothub-ack'] = message.ack;
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