// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var debug = require('debug')('mqtt-common');
var Message = require('azure-iot-common').Message;

/**
 * @class           module:azure-iot-device-mqtt.MqttReceiver
 * @classdesc       Object that is used to receive and settle messages from the server.
 *
 * @param  {Object}  transport   MQTT Client object.
 * @throws {ReferenceError}      If either mqttClient or topic_subscribe is falsy
 * @emits  message  When a message is received
 */
/**
 * @event module:azure-iot-mqtt-base.MqttReceiver#message
 * @type {Message}
 */
function MqttReceiver(mqttClient, topic_subscribe) {
  /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_001: [If the topic_subscribe parameter is falsy, a ReferenceError shall be thrown.]*/
  /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_002: [If the mqttClient parameter is falsy, a ReferenceError shall be thrown.]*/
  if (!mqttClient) { throw new ReferenceError('mqttClient cannot be falsy'); }
  if (!topic_subscribe) { throw new ReferenceError('topic_subscribe cannot be falsy'); }

  debug('creating a new MqttReceiver');
  EventEmitter.call(this);
  this._client = mqttClient;
  this._topic_subscribe = topic_subscribe;
  this._listenersInitialized = false;

  var self = this;

  this.on('removeListener', function () {
    // stop listening for AMQP events if our consumers stop listening for our events
    if (self._listenersInitialized && self.listeners('message').length === 0) {
      debug('Stopped listening for messages');
      self._removeListeners();
    }
  });

  this.on('newListener', function (eventName) {
    // lazy-init AMQP event listeners
    if (!self._listenersInitialized && eventName === 'message') {
      debug('Now listening for messages');
      self._setupListeners();
    }
  });
}

util.inherits(MqttReceiver, EventEmitter);

MqttReceiver.prototype._setupListeners = function () {
  debug('subscribing to ' + this._topic_subscribe);
  /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_003: [When a listener is added for the message event, the topic should be subscribed to.]*/
  this._client.subscribe(this._topic_subscribe, { qos: 1 }, function () {
    debug('subscribed to ' + this._topic_subscribe);
    /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_004: [If there is a listener for the message event, a message event shall be emitted for each message received.]*/
    this._client.on('message', this._onMqttMessage.bind(this));
    this._listenersInitialized = true;
  }.bind(this));
};

MqttReceiver.prototype._removeListeners = function () {
  this._client.removeListener('message', this._onMqttMessage.bind(this));
  /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_006: [When there are no more listeners for the message event, the topic should be unsubscribed]*/
  debug('unsubscribing from ' + this._topic_subscribe);
  this._client.unsubscribe(this._topic_subscribe, function () {
    debug('unsubscribed from ' + this._topic_subscribe);
    this._listenersInitialized = false;
  }.bind(this));
};

MqttReceiver.prototype._onMqttMessage = function (topic, message) {
  // needs proper conversion to transport-agnostic message.
  /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_005: [When a message event is emitted, the parameter shall be of type Message]*/
  var msg = new Message(message);
  debug('Got a message: ' + message);
  this.emit('message', msg);
};

module.exports = MqttReceiver;