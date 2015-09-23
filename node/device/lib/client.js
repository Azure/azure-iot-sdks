// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var HOSTNAME_LEN = "HostName=".length;
var DEVICEID_LEN = "DeviceId=".length;
var SAK_LEN = "SharedAccessKey=".length;

function parseConnString(connString)
{
  var config = {
    host: '',
    hubName: '',
    keyName: '',
    key: ''
  }; 
  var configArray = connString.split(';');
  configArray.forEach(function(element) {
    var pos = element.indexOf("HostName=");
    if (pos >= 0) {
      config.host = element.substring(pos+HOSTNAME_LEN);
      // Look for the first period
      var periodSeparator = config.host.indexOf('.'); 
      config.hubName = config.host.substring(0, periodSeparator);
    }
    else if ( (pos = element.indexOf("DeviceId=") ) >= 0) {
      config.keyName = element.substring(pos+DEVICEID_LEN);
    }
    else if ( (pos = element.indexOf("SharedAccessKey=") ) >= 0) {
      config.key = element.substring(pos+SAK_LEN);
    }
  });
  return config;
}

/**
 * @class Client
 * @classdesc Constructs a {@linkcode Client} object with the given IoT device connection
 *            string and `transport` object.
 * @param {String}  connString    The IoT device connection string
 * @param {Object}  transport     An object that implements the interface
 *                                expected of a transport object. See the
 *                                file **adapters/devdoc/https_requirements.docm**
 *                                for information on what the transport
 *                                object should look like. The {@linkcode Https}
 *                                class is an implementation that uses HTTP as
 *                                the communication protocol.
 */
/*Codes_SRS_NODE_IOTHUB_CLIENT_05_001: [The Client constructor shall accept a transport object]*/
var Client = function (connString, transport) {
  this.config = parseConnString(connString);
  this.transport = transport;
};

/**
 * The [sendEvent]{@linkcode Client#sendEvent} method sends an event message
 * to the IoT Hub as the device indicated by the connection string passed
 * via the constructor.
 * @param {Message}  message    The [message]{@linkcode module:common/message.Message}
 *                              to be sent.
 * @param {Function} done       The callback to be invoked when `sendEvent`
 *                              completes execution.
 * @see [Message]{@linkcode module:common/message.Message}
 */
Client.prototype.sendEvent = function(message, done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_002: [The sendEvent method shall send the event (indicated by the message argument) via the transport associated with the Client instance.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_003: [When the sendEvent method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback.]*/
  this.transport.sendEvent(message, this.config, done);
};

/**
 * The [sendEventBatch]{@linkcode Client#sendEventBatch} method sends a list
 * of event messages to the IoT Hub as the device indicated by the connection
 * string passed via the constructor.
 * @param {array<Message>} messages Array of [Message]{@linkcode module:common/message.Message}
 *                                  objects to be sent as a batch.
 * @param {Function}      done      The callback to be invoked when
 *                                  `sendEventBatch` completes execution.
 */
Client.prototype.sendEventBatch = function(messages, done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_07_004: [The sendEventBatch method shall send the list of events (indicated by the messages argument) via the transport associated with the Client instance.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_07_005: [When the sendEventBatch method completes the callback function shall be invoked with the same arguments as the underlying transport method’s callback.]*/
  this.transport.sendEventBatch(messages, this.config, done);
};

/**
 * The receive method queries the IoT Hub (as the device indicated in the
 * constructor argument) for the next notification in the queue.
 * @param {Function}  done      The callback to be invoked when `receive`
 *                              completes execution.
 */
Client.prototype.receive = function(done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_004: [The receive method shall query the IoT Hub for the next message via the transport associated with the Client instance.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_005: [When the receive method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback.]*/
  this.transport.receive(this.config, done);
};

/**
 * The `abandon` method directs the IoT Hub to re-enqueue a notification
 * message so it may be received again later.
 * @param {Message}   message   The [message]{@linkcode module:common/message.Message}
 *                              to be re-enqueued.
 * @param {Function}  done      The callback to be invoked when `abandon`
 *                              completes execution.
 */
Client.prototype.abandon = function(message, done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_07_001: [The abandon method shall call into the transport’s sendFeedback with the ‘abandon’ keyword and the lockToken.]*/
  this.transport.sendFeedback('abandon', message.lockToken, this.config, done);
};

/**
 * The `reject` method directs the IoT Hub to delete a notification message
 * from the queue and record that it was rejected.
 * @param {Message}   message   The [message]{@linkcode module:common/message.Message}
 *                              to be deleted.
 * @param {Function}  done      The callback to be invoked when `reject`
 *                              completes execution.
 */
Client.prototype.reject = function(message, done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_07_002: [The reject method shall call into the transport’s sendFeedback with the ‘reject’ keyword and the lockToken.]*/
  this.transport.sendFeedback('reject', message.lockToken, this.config, done);
};

/**
 * The `complete` method directs the IoT Hub to delete a notification message
 * from the queue and record that it was accepted.
 * @param {Message}   message   The [message]{@linkcode module:common/message.Message}
 *                              to be accepted.
 * @param {Function}  done      The callback to be invoked when `complete`
 *                              completes execution.
 */
Client.prototype.complete = function(message, done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_07_003: [The complete method shall call into the transport’s sendFeedback with the ‘complete’ keyword and the lockToken.]*/
  this.transport.sendFeedback('complete', message.lockToken, this.config, done);
};

module.exports = Client;
