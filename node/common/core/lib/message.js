// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Properties = require('./properties.js');

/**
 * @class module:azure-iot-common.Message
 * @classdesc Constructs a {@linkcode Message} object with the given data.
 *            `data` can be a Node [Buffer]{@linkcode https://nodejs.org/api/globals.html#globals_class_buffer}
 *            object or anything that can be used to construct a `Buffer`
 *            object from.
 * @property {Object}   properties      A map containing string
 *                                      keys and values for storing
 *                                      custom message properties.
 * @property {String}   messageId       Used to correlate two-way
 *                                      communication.
 *                                      Format: A case-sensitive string
 *                                      (up to 128 char long) of ASCII
 *                                      7-bit alphanumeric chars and the
 *                                      following special symbols:
 *                                      <br/>`- : . + % _ # * ? ! ( ) , = @ ; $ '`
 * @property {String}   to              Destination of the message
 * @property {Date}     expiryTimeUtc   Expiry time in UTC interpreted by hub on
 *                                      C2D messages. Ignored in other cases.
 * @property {String}   lockToken       Used by receiver to Abandon, Reject or
 *                                      Complete the message
 * @property {String}   correlationId   Used in message responses and feedback
 * @property {String}   userId          Used to specify the entity creating the
 *                                      message
 * @see {@link https://nodejs.org/api/globals.html#globals_class_buffer|Buffer}
 */
/*Codes_SRS_NODE_IOTHUB_MESSAGE_07_004: [The Message constructor shall accept a variable message that will be transmitted.]*/
function Message(data) {
  this.data = data;

  this.properties = new Properties();
  this.messageId = '';
  this.to = '';
  this.expiryTimeUtc = null;
  this.lockToken = '';
  this.correlationId = '';
  this.userId = '';
}

/**
 * @method          module:azure-iot-common.Message#getData
 * @description     Returns the data that was passed in to the [constructor]{@link Message}.
 * 
 * @returns {*} The data that was passed to the [constructor]{@link Message}.
 */
Message.prototype.getData = function() {
  /*Codes_SRS_NODE_IOTHUB_MESSAGE_07_003: [The getData function shall return a representation of the body of the message as the type that was presented during construction.]*/
  return this.data;
};

/**
 * @method          module:azure-iot-common.Message#getBytes
 * @description     Returns the data passed to the constructor as a Node
 *                  [Buffer]{@linkcode https://nodejs.org/api/globals.html#globals_class_buffer}
 *                  of bytes.
 * 
 * @returns {Buffer}
 */
Message.prototype.getBytes = function() {
  if (Buffer.isBuffer(this.data) ) {
    /*Codes_SRS_NODE_IOTHUB_MESSAGE_07_001: [If the data message that is store is of type Buffer then the data object will get returned unaltered.]*/
    return this.data;
  }
  else {
    /*Codes_SRS_NODE_IOTHUB_MESSAGE_07_002: [If the data message is of any other type then the data will be converted to a Buffer object and returned.]*/
    return new Buffer(this.data);
  }
};

module.exports = Message;