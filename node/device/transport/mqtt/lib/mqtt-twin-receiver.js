// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var translateError = require('../lib/mqtt-translate-error.js');
var querystring = require('querystring');
var url = require('url');

// $iothub/twin/PATCH/properties/reported/?$rid={request id}&$version={base version}

/* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_009: [** The subscribed topic for `response` events shall be '$iothub/twin/res/#' **]** */
var responseTopic = '$iothub/twin/res/#';

/* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_019: [** The subscribed topic for post events shall be $iothub/twin/PATCH/properties/desired/# **]** */
var postTopic = '$iothub/twin/PATCH/properties/desired/#';
        
/**
 * @class        module:azure-iot-device-mqtt.MqttTwinReceiver
 * @classdesc    Acts as a receiver for device-twin traffic
 * 
 * @param {Object} config   configuration object
 * @fires MqttTwinReceiver#subscribed   an MQTT topic has been successfully subscribed to
 * @fires MqttTwinReceiver#error    an error has occured while subscribing to an MQTT topic
 * @fires MqttTwinReceiver#response   a response message has been received from the service
 * @fires MqttTwinReceiver#post a post message has been received from the service
 * @throws {ReferenceError} If client parameter is falsy.
 *
 */
function MqttTwinReceiver(client) {
  /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_001: [** The `MqttTwinReceiver` constructor shall accept a `client` object **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_002: [** The `MqttTwinReceiver` constructor shall throw `ReferenceError` if the `client` object is falsy **]** */
  if (!client) {
    throw new ReferenceError('required parameter is missing');
  }
  this._client = client;
  EventEmitter.call(this);

  this.on("newListener", this._handleNewListener.bind(this));
  this.on("removeListener", this._handleRemoveListener.bind(this));
  this._boundMessageHandler = this._onMqttMessage.bind(this); // need to save this so that calls to add & remove listeners can be matched by the EventEmitter.
}

MqttTwinReceiver.errorEvent = 'error';
MqttTwinReceiver.responseEvent = 'response';
MqttTwinReceiver.postEvent = 'post';
MqttTwinReceiver.subscribedEvent = 'subscribed';


util.inherits(MqttTwinReceiver, EventEmitter);

MqttTwinReceiver.prototype._handleNewListener = function(eventName) {
  var self = this;

  if (eventName === MqttTwinReceiver.responseEvent) {
    if (EventEmitter.listenerCount(this, MqttTwinReceiver.responseEvent) === 0) { // array of listeners gets updated _after_ firing this event
      this._startListeningIfFirstSubscription();
      /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_003: [** When a listener is added for the `response` event, the appropriate topic shall be asynchronously subscribed to. **]** */
      process.nextTick( function() {
        self._client.subscribe(responseTopic, { qos: 0 }, function(err, transportObject) {
          if (err) {
            self._handleError(err);
          } else {
            /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_025: [** If the `subscribed` event is subscribed to, a `subscribed` event shall be emitted after an MQTT topic is subscribed to. **]** */
            /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_028: [** When the `subscribed` event is emitted, the parameter shall be an object which contains an `eventName` field and an `transportObject` field. **]** */
            /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_026: [** When the `subscribed` event is emitted because the response MQTT topic was subscribed, the parameter shall be the string 'response' **]**  */
            /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_029: [** When the subscribed event is emitted, the `transportObject` field shall contain the object returned by the library in the subscription response. **]** */
            self.emit(MqttTwinReceiver.subscribedEvent, { 'eventName' : MqttTwinReceiver.responseEvent, 'transportObject' : transportObject });
          }
        });
      });
     }
  } else if (eventName === MqttTwinReceiver.postEvent) {
    if (EventEmitter.listenerCount(this, MqttTwinReceiver.postEvent) === 0) {// array of listeners gets updated _after_ firing this event
      this._startListeningIfFirstSubscription();
      /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_018: [** When a listener is added to the post event, the appropriate topic shall be asynchronously subscribed to. **]** */
      process.nextTick( function() {
        self._client.subscribe(postTopic, { qos: 0 }, function(err, transportObject) {
          if (err) {
            self._handleError(err);
          } else {
            /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_025: [** If the `subscribed` event is subscribed to, a `subscribed` event shall be emitted after an MQTT topic is subscribed to. **]** */
            /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_028: [** When the `subscribed` event is emitted, the parameter shall be an object which contains an `eventName` field and an `transportObject` field. **]** */
            /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_027: [** When the `subscribed` event is emitted because the post MQTT topic was subscribed, the `eventName` field shall be the string 'post' **]** */
            /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_029: [** When the subscribed event is emitted, the `transportObject` field shall contain the object returned by the library in the subscription response. **]** */
            self.emit(MqttTwinReceiver.subscribedEvent, { 'eventName' : MqttTwinReceiver.postEvent, 'transportObject' : transportObject });
          }
        });
      });
    }
  }
};

MqttTwinReceiver.prototype._handleRemoveListener = function(eventName) {
  var self = this;

  if (eventName === MqttTwinReceiver.responseEvent) {
    if (EventEmitter.listenerCount(this, MqttTwinReceiver.responseEvent) === 0) { // array of listeners gets updated _before_ firing this event
      this._stopListeningIfLastUnsubscription();
      /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_005: [** When there are no more listeners for the `response` event, the topic should be unsubscribed **]** */
      self._client.unsubscribe(responseTopic, function(err) {
        if (err) {
          self._handleError(err);
        }
      });
    }
  } else if (eventName === MqttTwinReceiver.postEvent) {
    if (EventEmitter.listenerCount(this, MqttTwinReceiver.postEvent) === 0) { // array of listeners gets updated _before_ firing this event
      this._stopListeningIfLastUnsubscription();
      /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_021: [** When there are no more listeners for the post event, the topic should be unsubscribed. **]** */
      self._client.unsubscribe(postTopic, function(err) {
        if (err) {
          self._handleError(err);
        }
      });
    }
  }
};

MqttTwinReceiver.prototype._startListeningIfFirstSubscription = function() {
  // this method is called _before_ the new listener is added to the array of listeners
  if ((EventEmitter.listenerCount(this, MqttTwinReceiver.responseEvent) === 0) && (EventEmitter.listenerCount(this, MqttTwinReceiver.postEvent) === 0)) {
    this._client.on('message', this._boundMessageHandler);
  }
};

MqttTwinReceiver.prototype._stopListeningIfLastUnsubscription = function() {
  // this method is called _after_ the listener is removed from the array of listeners
  if ((EventEmitter.listenerCount(this, MqttTwinReceiver.responseEvent)) && (EventEmitter.listenerCount(this, MqttTwinReceiver.postEvent) === 0)) {
    this._client.removeListener('message', this._boundMessageHandler);
  }
};

MqttTwinReceiver.prototype._onMqttMessage = function (topic, message) {
  if (topic.indexOf('$iothub/twin/res') === 0) {
    this._onResponseMessage(topic, message);
  } else if (topic.indexOf('$iothub/twin/PATCH') === 0) {
    this._onPostMessage(topic, message);
  }
  /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_014: [** Any messages received on topics which violate the topic name formatting shall be ignored. **]** */
};


MqttTwinReceiver.prototype._onResponseMessage = function(topic, message){
  var urlObject, path, query;
  
  /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_014: [** Any messages received on topics which violate the topic name formatting shall be ignored. **]** */
  try {
    urlObject = url.parse(topic);
    path = urlObject.path.split('/');
    query = querystring.parse(urlObject.query);
  } catch(err) {
    return;
  }

/* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_006: [** When a `response` event is emitted, the parameter shall be an object which contains `status`, `requestId` and `body` members **]**  */
/* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_010: [** The topic which receives the response shall be formatted as '$iothub/twin/res/{status}/?$rid={request id}' **]** */
/* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_015: [** the {status} and {request id} fields in the topic name are required. **]** */
/* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_016: [** The {status} and {request id} fields in the topic name shall be strings **]** */
/* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_017: [** The {status} and {request id} fields in the topic name cannot be zero length. **]** */
  if ((path[0] === '$iothub') &&
      (path[1] === 'twin') &&
      (path[2] === 'res') &&
      (path[3]) &&
      (path[3].toString().length > 0) &&
      (query.$rid) &&
      (query.$rid.toString().length > 0))
  {
    /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_011: [** The `status` parameter of the `response` event shall be parsed out of the topic name from the {status} field **]** */
    /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_012: [** The `requestId` parameter of the `response` event shall be parsed out of the topic name from the {request id} field **]** */
    /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_013: [** The `body` parameter of the `response` event shall be the body of the received MQTT message **]**  */
    /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_011: [** The `status` parameter of the `response` event shall be parsed out of the topic name from the {status} field **]** */
    /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_012: [** The `requestId` parameter of the `response` event shall be parsed out of the topic name from the {request id} field **]** */
    /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_013: [** The `body` parameter of the `response` event shall be the body of the received MQTT message **]**  */
    var response = {
      'topic' : topic,
      'status' : path[3],
      '$rid' : query.$rid,
      'body' : message
    };

    /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_004: [** If there is a listener for the `response` event, a `response` event shall be emitted for each response received. **]** */
    this.emit(MqttTwinReceiver.responseEvent, response);
  }
};

MqttTwinReceiver.prototype._onPostMessage = function(topic, message) {
  /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_020: [** If there is a listener for the post event, a post event shal be emitteded for each post message received **]** */
  if (topic.indexOf('$iothub/twin/PATCH/properties/desired') === 0)
  {
  /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_022: [** When a post event it emitted, the parameter shall be the body of the message **]** */
    this.emit(MqttTwinReceiver.postEvent, message);
  }
};

MqttTwinReceiver.prototype._handleError = function(err) {
  /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_023: [** If the `error` event is subscribed to, an `error` event shall be emitted if any asynchronous subscribing operations fails. **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_024: [** When the `error` event is emitted, the first parameter shall be an error object obtained via the MQTT `translateErrror` module. **]** */
  this.emit(MqttTwinReceiver.errorEvent, translateError(err));
};


module.exports = MqttTwinReceiver;





