// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

require('es5-shim');
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var Message = require('azure-iot-common').Message;
var QueryString = require('querystring');
var URL = require('url');

var TOPIC_METHODS_SUBSCRIBE = "$iothub/methods/POST/#";

/**
 * @class           module:azure-iot-device-mqtt.MqttReceiver
 * @classdesc       Object that is used to receive and settle messages from the server.
 *
 * @param  {Object}  mqttClient    MQTT Client object.
 * @param  {string}  topicMessage  MQTT topic name for receiving C2D messages
 * @throws {ReferenceError}        If either mqttClient or topicMessage is falsy
 * @emits  message                 When a message is received
 */
/**
 * @event module:azure-iot-device-mqtt.MqttReceiver#message
 * @type {Message}
 */
function MqttReceiver(mqttClient, topicMessage) {
  /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_001: [If the topicMessage parameter is falsy, a ReferenceError shall be thrown.]*/
  /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_002: [If the mqttClient parameter is falsy, a ReferenceError shall be thrown.]*/
  if (!mqttClient) { throw new ReferenceError('mqttClient cannot be falsy'); }
  if (!topicMessage) { throw new ReferenceError('topicMessage cannot be falsy'); }

  EventEmitter.call(this);
  this._mqttClient = mqttClient;

  // MQTT topics to subscribe to
  this._topics = {
    'message': {
      name: topicMessage,
      listenersCount: 0,
      subscribeInProgress: false,
      subscribed: false,
      topicMatchRegex: /^devices\/.*\/messages\/devicebound\/.*$/g,
      handler: this._onC2DMessage.bind(this)
    },
    'method': {
      name: TOPIC_METHODS_SUBSCRIBE,
      listenersCount: 0,
      subscribeInProgress: false,
      subscribed: false,
      topicMatchRegex: /^\$iothub\/methods\/POST\/.*$/g,
      handler: this._onDeviceMethod.bind(this)
    }
  };

  var self = this;

  // gets the total number of event listeners across all MQTT topics we are
  // interested in
  var getTotalListeners = function() {
    return Object.keys(self._topics)
                 .map(function(name) {
                   return self._topics[name].listenersCount;
                 })
                 .reduce(function(previous, current) {
                   return previous + current;
                 }, 0);
  };

  var dispatchMqttMessage = this._dispatchMqttMessage.bind(this);

  this.on('newListener', function (eventName) {
    // if the event is a 'method' event then eventName is in the format
    // 'method_{method name}'
    var topic = eventName.indexOf('method_') === 0 ?
                    self._topics.method :
                    eventName === 'message' ?
                      self._topics.message :
                      null;
    if(!!topic) {
      // increment listeners count for this topic
      ++(topic.listenersCount);

      // if this is the first listener then add a listener for the 'message'
      // event on this._mqttClient
      if(getTotalListeners() === 1) {
        self._mqttClient.on('message', dispatchMqttMessage);
      }

      // lazy-init MQTT subscription
      if (topic.subscribed === false && topic.subscribeInProgress === false) {
        // Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_003: [ When a listener is added for the message event, the topic should be subscribed to. ]
        // Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_13_002: [ When a listener is added for the method event, the topic should be subscribed to. ]
        self._setupSubscription(topic);
      }
    }
  });

  this.on('removeListener', function (eventName) {
    // if the event is a 'method' event then eventName is in the format
    // 'method_{method name}'
    var topic = eventName.indexOf('method_') === 0 ?
                    self._topics.method :
                    eventName === 'message' ?
                      self._topics.message :
                      null;
    if(!!topic) {
      // decrement listeners count for this topic
      --(topic.listenersCount);

      // if this is the last listener then remove the listener for the 'message'
      // event on this._mqttClient
      if(getTotalListeners() === 0) {
        self._mqttClient.removeListener('message', dispatchMqttMessage);
      }

      // stop listening for MQTT events if our consumers stop listening for our events
      if (topic.listenersCount === 0 && topic.subscribed === true) {
        // Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_006: [ When there are no more listeners for the message event, the topic should be unsubscribed. ]
        // Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_13_004: [ When there are no more listeners for the method event, the topic should be unsubscribed. ]
        self._removeSubscription(topic);
      }
    }
  });
}

util.inherits(MqttReceiver, EventEmitter);

MqttReceiver.prototype._setupSubscription = function (topic) {
  topic.subscribeInProgress = true;
  this._mqttClient.subscribe(topic.name, { qos: 0 }, function (err) {
    topic.subscribeInProgress = false;
    if(!err) {
      topic.subscribed = true;
    }

    // TODO: There doesn't seem to be a way in the current design to surface
    //       the error if the MQTT topic subscription fails. Fix this.
  });
};

MqttReceiver.prototype._removeSubscription = function (topic) {
  this._mqttClient.unsubscribe(topic.name, function (err) {
    if(!err) {
      topic.subscribed = false;

      // TODO: There doesn't seem to be a way in the current design to surface
      //       the error if the MQTT topic unsubscription fails. Fix this.
    }
  });
};

MqttReceiver.prototype._dispatchMqttMessage = function (topic, payload) {
  // dispatch the message to the appropriate handler
  var self = this;
  var targetTopic = null;
  Object.keys(this._topics).some(function(topicIndex) {
    // Turns out regexes are stateful. We need to reset the search index back to
    // the beginning every time we use it.
    var theTopic = self._topics[topicIndex];
    theTopic.topicMatchRegex.lastIndex = 0;
    if(theTopic.topicMatchRegex.test(topic)) {
      targetTopic = theTopic;
    }
    return targetTopic !== null;
  });
  if(!!targetTopic) {
    targetTopic.handler(topic, payload);
  }
};

MqttReceiver.prototype._onC2DMessage = function (topic, payload) {
  // needs proper conversion to transport-agnostic message.
  /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_005: [When a message event is emitted, the parameter shall be of type Message]*/
  var msg = new Message(payload);
  /*Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_16_004: [If there is a listener for the message event, a message event shall be emitted for each message received.]*/
  this.emit('message', msg);
};

function _parseMessage(topic, body) {
  var url, path, query;
  try {
    url = URL.parse(topic);
    path = url.path.split('/');
    query = QueryString.parse(url.query);
  }
  catch(err) {
    return undefined;
  }

  // if the topic has a querystring then 'path' will include it; so
  // we strip it out
  var lastPathComponent = path[path.length - 1];
  if(lastPathComponent.indexOf('?') !== -1) {
    path[path.length - 1] = lastPathComponent.substr(
      0, lastPathComponent.indexOf('?')
    );
  }

  if(path.length > 0 && path[0] === '$iothub') {
    var message = {};
    if(path.length > 1 && path[1].length > 0) {
      // create an object for the module; for example, $iothub/twin/...
      // would result in there being a message.twin object
      var mod = message[path[1]] = {};

      // parse the request ID if there is one
      if(!!(query.$rid)) {
        message.requestId = query.$rid;
      }

      // parse the other properties properties (excluding $rid)
      message.properties = query;
      delete message.properties.$rid;

      // save the body
      message.body = body;

      // parse the verb
      if(path.length > 2 && path[2].length > 0) {
        mod.verb = path[2];

        // This is a topic that looks like this:
        //  $iothub/methods/POST/{method name}?$rid={request id}&{serialized properties}
        // We parse the method name out.
        if(path.length > 3 && path[3].length > 0) {
          mod.methodName = path[3];
        } else {
          // The service published a message on a strange topic name. This is
          // probably a service bug. At any rate we don't know what to do with
          // this strange topic so we throw.
          throw new Error('Device method call\'s MQTT topic name does not include the method name.');
        }
      }
    }

    return message;
  }

  return undefined;
}

MqttReceiver.prototype.onDeviceMethod = function(methodName, callback) {
  this.on('method_' + methodName, callback);
};

MqttReceiver.prototype._onDeviceMethod = function (topic, payload) {
  // The topic name looks like this:
  //  $iothub/methods/POST/{method name}?$rid={request id}&{serialized properties}
  // We parse out the message.

  /* Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_13_005: [ When a method_<METHOD NAME> event is emitted the parameter shall conform to the shape as defined by the interface specified below:

    interface StringMap {
        [key: string]: string;
    }

    interface MethodMessage {
        methods: { methodName: string; };
        requestId: string;
        properties: StringMap;
        body: Buffer;
        verb: string;
    }
  ]*/
  var methodMessage = _parseMessage(topic, payload);
  if(!!methodMessage) {
    // Codes_SRS_NODE_DEVICE_MQTT_RECEIVER_13_003: [ If there is a listener for the method event, a method_<METHOD NAME> event shall be emitted for each message received. ]
    // we emit a message for the event 'method_{method name}'
    this.emit('method_' + methodMessage.methods.methodName, methodMessage);
  }
};

module.exports = MqttReceiver;
