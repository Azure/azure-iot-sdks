// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

require('es5-shim');
var assert = require('chai').assert;
var sinon = require('sinon');
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var MqttReceiver = require('../lib/mqtt_receiver.js');

describe('MqttReceiver', function () {
  describe('#constructor', function () {
    /*Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_16_001: [If the topicMessage parameter is falsy, a ReferenceError shall be thrown.]*/
    [null, undefined, '', 0].forEach(function (topicMessage) {
      it('throws if topicMessage is ' + topicMessage, function () {
        assert.throws(function () {
          return new MqttReceiver('client', topicMessage);
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
    this.emitMethod = function (topic, payload) {
      this.emit('message', topic, payload);
    };
  };

  util.inherits(FakeMqttClient, EventEmitter);

  describe('#events', function () {
    describe('#message', function() {
      /*Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_16_003: [When a listener is added for the message event, the topic should be subscribed to.]*/
      it('subscribes to the topic when a listener is added', function () {
        var topicName = 'topic_subscribe_name';
        var mqttClient = new FakeMqttClient();
        mqttClient.on = sinon.spy();
        mqttClient.subscribe = sinon.stub(mqttClient, 'subscribe', function (topic, options, callback) {
          assert(mqttClient.on.calledWith('message'));
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

        mqttClient.emitMessage('message', 'devices/foo/messages/devicebound/%24.mid=0209afa7-1e2f-4dc1-8a6f-8500efd81db3&%24.to=%2Fdevices%2Ffoo%2Fmessages%2Fdevicebound');
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
        assert(mqttClient.subscribe.calledWith(topicName), 'subscribe is not called with' + topicName);

        receiver.removeListener('message', listener1);
        assert(mqttClient.unsubscribe.notCalled);
        receiver.removeListener('message', listener2);
        assert(mqttClient.unsubscribe.calledOnce);
      });
    });

    describe('#method', function() {
      // Tests_Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_13_002: [ When a listener is added for the method event, the topic should be subscribed to. ]*/
      it('subscribes to the method topic when a listener is added', function () {
        // setup
        var mqttClient = new FakeMqttClient();
        var onSpy = sinon.spy(mqttClient, 'on');
        var subscribeStub = sinon
              .stub(mqttClient, 'subscribe');

        // stub should call 'done' with no error
        subscribeStub.onCall(0).callsArgWith(2, null);
        subscribeStub.onCall(1).callsArgWith(2, null);

        // test
        var receiver = new MqttReceiver(mqttClient, 'topic_message');
        receiver.on('method_UpdateFirmware', function () { });
        receiver.on('method_Reboot', function () { });
        receiver.on('message', function () { });
        receiver.on('message', function () { });

        // assert
        assert.isTrue(onSpy.calledOnce,
          'mqttClient.on was called more than once');
        assert.isTrue(onSpy.calledWith('message'),
          'mqttClient.on was not called for "message" event');
        assert.isTrue(subscribeStub.calledTwice,
          'mqttClient.subscribe was not called twice');
        assert.isTrue(subscribeStub.firstCall.calledWith('$iothub/methods/POST/#', { qos: 0}),
          'first call to mqttClient.subscribe was not with topic $iothub/methods/POST/#');
        assert.isTrue(subscribeStub.secondCall.calledWith('topic_message'),
          'second call to mqttClient.subscribe was not with topic topic_message');
      });

      // Tests_Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_13_002: [ When a listener is added for the method event, the topic should be subscribed to. ]*/
      it('does not subscribe twice to the same topic for multiple event registrations', function () {
        // setup
        var mqttClient = new FakeMqttClient();
        var onSpy = sinon.spy(mqttClient, 'on');
        var subscribeStub = sinon
              .stub(mqttClient, 'subscribe');

        // stub should call 'done' with no error
        subscribeStub.onCall(0).callsArgWithAsync(2, null);
        subscribeStub.onCall(1).callsArgWith(2, null);

        // test
        var receiver = new MqttReceiver(mqttClient, 'topic_message');
        receiver.on('method_UpdateFirmware', function () { });
        receiver.on('method_Reboot', function () { });
        receiver.on('message', function () { });
        receiver.on('message', function () { });

        // assert
        assert.isTrue(onSpy.calledOnce,
          'mqttClient.on was called more than once');
        assert.isTrue(onSpy.calledWith('message'),
          'mqttClient.on was not called for "message" event');
        assert.isTrue(subscribeStub.calledTwice,
          'mqttClient.subscribe was not called twice');
        assert.isTrue(subscribeStub.firstCall.calledWith('$iothub/methods/POST/#', { qos: 0}),
          'first call to mqttClient.subscribe was not with topic $iothub/methods/POST/#');
        assert.isTrue(subscribeStub.secondCall.calledWith('topic_message'),
          'second call to mqttClient.subscribe was not with topic topic_message');
      });

      // Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_13_003: [ If there is a listener for the method event, a method_<METHOD NAME> event shall be emitted for each message received. ]
      /* Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_13_005: [ When a method_<METHOD NAME> event is emitted the parameter shall conform to the shape as defined by the interface specified below:

        interface StringMap {
            [key: string]: string;
        }

        interface MethodMessage {
            methods: { methodName: string; };
            requestId: string;
            properties: StringMap;
            body: any;
            verb: string;
        }
      ]*/
      it('emits a method event when a method message is received', function() {
        // setup
        var mqttClient = new FakeMqttClient();
        var receiver = new MqttReceiver(mqttClient, 'topic_message');
        var callback = sinon.spy();
        var msg;
        receiver.on('method_Reboot', callback);

        // test
        mqttClient.emitMethod('$iothub/methods/POST/Reboot?$rid=1');

        // assert
        assert.isTrue(callback.calledOnce);
        assert.isArray(callback.args);
        assert.isTrue(callback.args.length >= 1);
        assert.isArray(callback.args[0]);
        assert.isTrue(callback.args[0].length >= 1);
        msg = callback.args[0][0];
        assert.isObject(msg);
        assert.deepEqual(msg, {
          methods: {
            methodName: 'Reboot',
            verb: 'POST'
          },
          requestId: '1',
          properties: {},
          body: undefined
        });
      });

      /* Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_13_005: [ When a method_<METHOD NAME> event is emitted the parameter shall conform to the shape as defined by the interface specified below:

        interface StringMap {
            [key: string]: string;
        }

        interface MethodMessage {
            methods: { methodName: string; };
            requestId: string;
            properties: StringMap;
            body: any;
            verb: string;
        }
      ]*/
      it('emits a method event when a method message is received with properties', function() {
        // setup
        var mqttClient = new FakeMqttClient();
        var receiver = new MqttReceiver(mqttClient, 'topic_message');
        var callback = sinon.spy();
        var msg;
        receiver.on('method_Reboot', callback);

        // test
        mqttClient.emitMethod(
          '$iothub/methods/POST/Reboot?$rid=1&k1=v1&k2=v2&k3=v3'
        );

        // assert
        assert.isTrue(callback.calledOnce);
        assert.isArray(callback.args);
        assert.isTrue(callback.args.length >= 1);
        assert.isArray(callback.args[0]);
        assert.isTrue(callback.args[0].length >= 1);
        msg = callback.args[0][0];
        assert.isObject(msg);
        assert.deepEqual(msg, {
          methods: {
            methodName: 'Reboot',
            verb: 'POST'
          },
          requestId: '1',
          properties: {
            'k1': 'v1',
            'k2': 'v2',
            'k3': 'v3'
          },
          body: undefined
        });
      });

      /* Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_13_005: [ When a method_<METHOD NAME> event is emitted the parameter shall conform to the shape as defined by the interface specified below:

        interface StringMap {
            [key: string]: string;
        }

        interface MethodMessage {
            methods: { methodName: string; };
            requestId: string;
            properties: StringMap;
            body: any;
            verb: string;
        }
      ]*/
      it('emits a method event when a method message is received with payload and properties', function() {
        // setup
        var mqttClient = new FakeMqttClient();
        var receiver = new MqttReceiver(mqttClient, 'topic_message');
        var callback = sinon.spy();
        var msg, payload;
        receiver.on('method_Reboot', callback);

        // test
        payload = new Buffer('Here\'s some payload');
        mqttClient.emitMethod(
          '$iothub/methods/POST/Reboot?$rid=1&k1=v1&k2=v2&k3=v3',
          payload
        );

        // assert
        assert.isTrue(callback.calledOnce);
        assert.isArray(callback.args);
        assert.isTrue(callback.args.length >= 1);
        assert.isArray(callback.args[0]);
        assert.isTrue(callback.args[0].length >= 1);
        msg = callback.args[0][0];
        assert.isObject(msg);
        assert.deepEqual(msg, {
          methods: {
            methodName: 'Reboot',
            verb: 'POST'
          },
          requestId: '1',
          properties: {
            'k1': 'v1',
            'k2': 'v2',
            'k3': 'v3'
          },
          body: payload
        });
      });

      // Tests_SRS_NODE_DEVICE_MQTT_RECEIVER_13_004: [ When there are no more listeners for the method event, the topic should be unsubscribed. ]
      it('unsubscribes from MQTT topic when there are no more listeners', function() {
        // setup
        var mqttClient = new FakeMqttClient();
        var receiver = new MqttReceiver(mqttClient, 'topic_message');
        var callback = function() {};
        var unsubscribeStub = sinon.stub(mqttClient, 'unsubscribe');

        // stub should call 'done' with no error
        unsubscribeStub.onCall(0).callsArgWith(1, null);

        receiver.on('method_Reboot', callback);
        receiver.on('method_UpdateFirmware', callback);

        // test
        receiver.removeListener('method_Reboot', callback);
        receiver.removeListener('method_UpdateFirmware', callback);

        // assert
        assert.isTrue(unsubscribeStub.calledOnce,
          'mqttClient.unsubscribe was not called');
        assert.isTrue(unsubscribeStub.calledWith('$iothub/methods/POST/#'),
          'call to mqttClient.unsubscribe was not with topic $iothub/methods/POST/#');
      });
    });
  });
});
