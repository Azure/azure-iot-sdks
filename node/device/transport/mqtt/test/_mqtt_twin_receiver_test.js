// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var MqttTwinReceiver = require('../lib/mqtt-twin-receiver.js');
var MqttProvider = require('./_fake_mqtt.js');
var sinon = require('sinon');

var provider;
var receiver;

var validateSubscription = function(shortname, topic, done) {
  var subscribe = sinon.spy(provider, 'subscribe');
  receiver.on(shortname, function() {});
  process.nextTick(function() {
    assert(subscribe.withArgs(topic).calledOnce);
    done();
  });
};

var validateUnsubscription = function(shortname, topic, done) {
  var unsubscribe = sinon.spy(provider, 'unsubscribe');
  var func = function() { };
  receiver.on(shortname, func);
  process.nextTick(function() {
    receiver.removeListener(shortname, func);
    process.nextTick(function() {
      assert(unsubscribe.withArgs(topic).calledOnce);
      done();
    });
  });
};

var validateEventFires = function(shortname, topic, done) {
  var handler = sinon.spy();
  receiver.on(shortname, handler);
  provider.fakeMessageFromService(topic, 'fake_body');
  process.nextTick(function() {
    assert(handler.calledOnce);
    provider.fakeMessageFromService(topic, 'fake_body');
    process.nextTick(function() {
      assert(handler.calledTwice);
      done();
    });
  });
};

describe('MqttTwinReceiver', function () {
  
  beforeEach(function(done) {
    provider = new MqttProvider();
    receiver = new MqttTwinReceiver(provider);
    done();
  });
    
  describe('#constructor', function () {
    
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_001: [** The `MqttTwinReceiver` constructor shall accept a `client` object **]** */
    it ('accepts a config object', function() {
      assert.equal(receiver._client, provider);
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_002: [** The `MqttTwinReceiver` constructor shall throw `ReferenceError` if the `client` object is falsy **]** */
    it ('throws if client is falsy', function() {
      assert.throws(function() {
        receiver = new MqttTwinReceiver();
      }, ReferenceError);
    });
  });

  describe('response event', function() {

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_003: [** When a listener is added for the `response` event, the appropriate topic shall be asynchronously subscribed to. **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_009: [** The subscribed topic for `response` events shall be '$iothub/twin/res/#' **]** */
    it ('asynchronously subscribes when  a listener is added', function(done) {
      validateSubscription(MqttTwinReceiver.responseEvent, '$iothub/twin/res/#', done);
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_005: [** When there are no more listeners for the `response` event, the topic should be unsubscribed **]** */
    it ('unsubscribes when there are no more listeners', function(done) {
      validateUnsubscription(MqttTwinReceiver.responseEvent, '$iothub/twin/res/#', done);
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_004: [** If there is a listener for the `response` event, a `response` event shall be emitted for each response received. **]** */
    it ('emits a response event for every message', function(done) {
      validateEventFires(MqttTwinReceiver.responseEvent, '$iothub/twin/res/200/?$rid=42', done);
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_006: [** When a `response` event is emitted, the parameter shall be an object which contains `status`, `requestId` and `body` members **]**  */
    it ('has a parameter with an object with the correct properties', function(done) {
      receiver.on(MqttTwinReceiver.responseEvent, function(data) {
        assert.isDefined(data.status);
        assert.isDefined(data.$rid);
        assert.isDefined(data.body);
        done();
      });
      provider.fakeMessageFromService('$iothub/twin/res/200/?$rid=42', 'fake_body');
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_010: [** The topic which receives the response shall be formatted as '$iothub/twin/res/{status}/?$rid={request id}' **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_011: [** The `status` parameter of the `response` event shall be parsed out of the topic name from the {status} field **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_012: [** The `requestId` parameter of the `response` event shall be parsed out of the topic name from the {request id} field **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_013: [** The `body` parameter of the `response` event shall be the body of the received MQTT message **]**  */
    it ('parses the topic name correctly', function(done) {
      receiver.on(MqttTwinReceiver.responseEvent, function(data) {
        assert.equal(data.status, 200);
        assert.equal(data.$rid, 42);
        assert.equal(data.body, 'fake_body');
        done();
      });
      provider.fakeMessageFromService('$iothub/twin/res/200/?$rid=42', 'fake_body');
    });
    
    it ('ignores messages on invalid topics', function(done) {
      receiver.on(MqttTwinReceiver.responseEvent, function() {
        assert.fail();
      });
      /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_014: [** Any messages received on topics which violate the topic name formatting shall be ignored. **]** */
      provider.fakeMessageFromService('garbage');
      provider.fakeMessageFromService('$iothub/twin/PATCH/properties/desired/');
      /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_015: [** the {status} and {request id} fields in the topic name are required. **]** */
      /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_016: [** The {status} and {request id} fields in the topic name shall be strings **]** */
      /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_017: [** The {status} and {request id} fields in the topic name cannot be zero length. **]** */
      provider.fakeMessageFromService('$iothub/twin/res/');
      provider.fakeMessageFromService('$iothub/twin/res/200/$rid=');
      provider.fakeMessageFromService('$iothub/twin/res//$rid=10');
      provider.fakeMessageFromService('$iothub/twin/res//$rid=');
      process.nextTick(done);
    });
    
  }); 

  describe('post event', function() {
    
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_018: [** When a listener is added to the post event, the appropriate topic shall be asynchronously subscribed to. **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_019: [** The subscribed topic for post events shall be $iothub/twin/PATCH/properties/reported/# **]** */
    it ('asynchronously subscribes when  a listener is added', function(done) {
      validateSubscription(MqttTwinReceiver.postEvent, '$iothub/twin/PATCH/properties/desired/#', done);
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_021: [** When there are no more listeners for the post event, the topic should be unsubscribed. **]** */
    it ('unsubscribes when there are no more listeners', function(done) {
      validateUnsubscription(MqttTwinReceiver.postEvent, '$iothub/twin/PATCH/properties/desired/#', done);
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_020: [** If there is a listener for the post event, a post event shal be emitteded for each post message received **]** */
    it ('emits a response for every message', function(done) {
      validateEventFires(MqttTwinReceiver.postEvent, '$iothub/twin/PATCH/properties/desired/#', done);
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_022: [** When a post event it emitted, the parameter shall be the body of the message **]** */
    it ('passes the body as a parameter of the event', function(done) {
      receiver.on(MqttTwinReceiver.postEvent, function(data) {
        assert.equal(data, 'fake_body');
        done();
      });
      provider.fakeMessageFromService('$iothub/twin/PATCH/properties/desired/', 'fake_body');
    });
  });

  describe('error event', function() {

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_023: [** If the `error` event is subscribed to, an `error` event shall be emitted if any asynchronous subscribing operations fails. **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_024: [** When the `error` event is emitted, the first parameter shall be an error object obtained via the MQTT `translateErrror` module. **]** */
    it ('emits an error if subscribing to response event fails', function(done) {
      provider.subscribeShouldFail = true;
      receiver.on('error', function(err) {
        if (err.constructor.name === 'UnauthorizedError') {
          done();
        }
      });
      receiver.on(MqttTwinReceiver.responseEvent, function() {});
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_023: [** If the `error` event is subscribed to, an `error` event shall be emitted if any asynchronous subscribing operations fails. **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_024: [** When the `error` event is emitted, the first parameter shall be an error object obtained via the MQTT `translateErrror` module. **]** */
    it ('emits an error if subscribing to post event fails', function(done) {
      provider.subscribeShouldFail = true;
      receiver.on('error', function(err) {
        if (err.constructor.name === 'UnauthorizedError') {
          done();
        }
      });
      receiver.on(MqttTwinReceiver.postEvent, function() {});
    });

  });

  describe('subscribed event', function() {
    
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_025: [** If the `subscribed` event is subscribed to, a `subscribed` event shall be emitted after an MQTT topic is subscribed to. **]** */
    it ('emits a subscribed event after successful subscription to response event', function(done) {
      receiver.on('subscribed', function() {
        done();
      });
      receiver.on(MqttTwinReceiver.responseEvent, function() {});
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_025: [** If the `subscribed` event is subscribed to, a `subscribed` event shall be emitted after an MQTT topic is subscribed to. **]** */
    it ('emits a subscribed event after successful subscription to post event', function(done) {
      receiver.on('subscribed', function() {
        done();
      });
      receiver.on(MqttTwinReceiver.postEvent, function() {});
    });
    
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_028: [** When the `subscribed` event is emitted, the parameter shall be an object which contains an `eventName` field and an `transportObject` field. **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_026: [** When the `subscribed` event is emitted because the response MQTT topic was subscribed, the `eventName` field shall be the string 'response' **]**  */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_029: [** When the subscribed event is emitted, the `transportObject` field shall contain the object returned by the library in the subscription response. **]** */
    it ('passes the the word "response" as an event parameter when the response topic is successfully subscribed to', function(done) {
      receiver.on('subscribed', function(response) {
        if ((response.eventName === MqttTwinReceiver.responseEvent) && (response.transportObject === 'fake_object')) {
          done();
        }
      });
      receiver.on(MqttTwinReceiver.responseEvent, function() {});
    });

    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_028: [** When the `subscribed` event is emitted, the parameter shall be an object which contains an `eventName` field and an `transportObject` field. **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_027: [** When the `subscribed` event is emitted because the post MQTT topic was subscribed, the `eventName` field shall be the string 'post' **]** */
    /* Tests_SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_029: [** When the subscribed event is emitted, the `transportObject` field shall contain the object returned by the library in the subscription response. **]** */
    it ('passes the the word "post" as an event parameter when the post topic is successfully subscribed to', function(done) {
      receiver.on('subscribed', function(response) {
        if ((response.eventName === MqttTwinReceiver.postEvent) && (response.transportObject === 'fake_object')) {
          done();
        }
      });
      receiver.on(MqttTwinReceiver.postEvent, function() {});
    });

  });
});

