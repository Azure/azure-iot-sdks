// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var sinon = require('sinon');
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var MqttReceiver = require('../lib/mqtt_receiver.js');

describe('MqttReceiver', function () {
  describe('#constructor', function () {
    /*Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_16_001: [If the topic_subscribe parameter is falsy, a ReferenceError shall be thrown.]*/
    [null, undefined, '', 0].forEach(function (topic) {
      it('throws if topic_subscribe is ' + topic, function () {
        assert.throws(function () {
          return new MqttReceiver('client', topic);
        }, ReferenceError);
      });
    });
    /*Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_16_002: [If the mqttClient parameter is falsy, a ReferenceError shall be thrown.]*/
    [null, undefined, '', 0].forEach(function (mqttClient) {
      it('throws if mqttClient is ' + mqttClient, function () {
        assert.throws(function () {
          return new MqttReceiver(mqttClient, 'topic');
        }, ReferenceError);
      });
    });
  });

  var FakeMqttClient = function (client, topic) {
    EventEmitter.call(this);
    this._client = client;
    this._topic = topic;
    this.subscribe = function (topic, options, callback) { callback(); };
    this.unsubscribe = function (topic, callback) { callback(); };
    this.emitMessage = function (eventName, content) {
      this.emit(eventName, content);
    };
  };

  util.inherits(FakeMqttClient, EventEmitter);

  describe('#events', function () {
    /*Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_16_003: [When a listener is added for the message event, the topic should be subscribed to.]*/
    it('subscribes to the topic when a listener is added', function () {
      var topicName = 'topic_subscribe_name';
      var mqttClient = new FakeMqttClient();
      mqttClient.subscribe = sinon.stub(mqttClient, 'subscribe', function (topic, options, callback) {
        assert.isOk(options);
        assert.equal(topic, topicName);
        callback();
      });

      var receiver = new MqttReceiver(mqttClient, topicName);
      receiver.on('message', function () { });
      receiver.on('message', function () { });

      assert(mqttClient.subscribe.calledOnce, 'subscribe is not called once');
      assert(mqttClient.subscribe.calledWith(topicName), 'subscribe is not called with topicName');
    });

    /*Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_16_004: [If there is a listener for the message event, a message event shall be emitted for each message received.]*/
    /*Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_16_005: [When a message event is emitted, the parameter shall be of type Message]*/
    it('emits a message event with a Message object when there is a listener', function (done) {
      var mqttClient = new FakeMqttClient();
      var receiver = new MqttReceiver(mqttClient, 'topic');
      receiver.on('message', function (msg) {
        assert.equal(msg.constructor.name, 'Message');
        done();
      });

      mqttClient.emitMessage('message', 'foo');
    });

    /*Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_16_006: [When there are no more listeners for the message event, the topic should be unsubscribed]*/
    it('unsubscribes from the topic when there are no more listeners', function () {
      var topicName = 'topic_subscribe_name';
      var mqttClient = new FakeMqttClient();
      mqttClient.subscribe = sinon.stub(mqttClient, 'subscribe', function (topic, options, callback) {
        assert.isOk(options);
        assert.equal(topic, topicName);
        callback();
      });
      mqttClient.unsubscribe = sinon.stub(mqttClient, 'unsubscribe', function (topic, callback) {
        assert.equal(topic, topicName);
        callback();
      });

      var listener1 = function () { };
      var listener2 = function () { };
      var receiver = new MqttReceiver(mqttClient, topicName);
      receiver.on('message', listener1);
      receiver.on('message', listener2);

      assert(mqttClient.subscribe.calledOnce, 'subscribe is not called once');
      assert(mqttClient.subscribe.calledWith(topicName), 'subscribe is not called with topicName');

      receiver.removeListener('message', listener1);
      assert(mqttClient.unsubscribe.notCalled);
      receiver.removeListener('message', listener2);
      assert(mqttClient.unsubscribe.calledOnce);
    });
  });
});