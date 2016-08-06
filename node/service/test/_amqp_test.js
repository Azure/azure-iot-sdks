// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var Amqp = require('../lib/amqp.js');
var assert = require('chai').assert;

var fakeConfig = {
  host: 'hub.host.name',
  hubName: 'hub',
  keyName: 'keyName',
  sharedAccessSignature: 'signature'
};

describe('Amqp', function() {
  var FakeAmqpBase = function() {
    EventEmitter.call(this);

    this.connect = function (uri, sslOptions, callback) {
      callback();
    };

    this.disconnect = function (callback) {
      callback();
    };

    this.setDisconnectHandler = function(eventHandler) {
      this.on('disconnect', eventHandler);
    };
  };

  util.inherits(FakeAmqpBase, EventEmitter);

  describe('#constructor', function() {
    it('sets up \'disconnect\' event forwarding', function(done){
      var fakeAmqpBase = new FakeAmqpBase();
      var amqp = new Amqp(fakeConfig, fakeAmqpBase);
      amqp.on('disconnect', function() {
        done();
      });
      fakeAmqpBase.emit('disconnect');
    });
  });

  describe('#connect', function() {
    /*Tests_SRS_NODE_IOTHUB_SERVICE_AMQP_16_019: [The `connect` method shall call the `connect` method of the base AMQP transport and translate its result to the caller into a transport-agnostic object.]*/
    it('calls the base transport connect method', function(done) {
      var amqp = new Amqp(fakeConfig, new FakeAmqpBase());
      amqp.connect(done);
    });

    /*Tests_SRS_NODE_IOTHUB_SERVICE_AMQP_16_021: [** All asynchronous instance methods shall not throw if `done` is not specified or falsy]*/
    it('does not fail if `done` is not specified or falsy', function() {
      var amqp = new Amqp(fakeConfig, new FakeAmqpBase());
      assert.doesNotThrow(function() {
        amqp.connect();
      });
    });
  });

  describe('#disconnect', function() {
    /*Tests_SRS_NODE_IOTHUB_SERVICE_AMQP_16_020: [** The `disconnect` method shall call the `disconnect` method of the base AMQP transport and translate its result to the caller into a transport-agnostic object.]*/
    it('calls the base transport disconnect method', function(done) {
      var amqp = new Amqp(fakeConfig, new FakeAmqpBase());
      amqp.disconnect(done);
    });

    /*Tests_SRS_NODE_IOTHUB_SERVICE_AMQP_16_021: [** All asynchronous instance methods shall not throw if `done` is not specified or falsy]*/
    it('does not fail if `done` is not specified or falsy', function() {
      var amqp = new Amqp(fakeConfig, new FakeAmqpBase());
      assert.doesNotThrow(function() {
        amqp.connect();
      });
    });
  });

  describe('#getFeedbackReceiver', function() {
    /*Tests_SRS_NODE_IOTHUB_SERVICE_AMQP_16_013: [The `getFeedbackReceiver` method shall request an `AmqpReceiver` object from the base AMQP transport for the `/messages/serviceBound/feedback` endpoint.]*/
    it('requests an AmqpReceiver from the feedback endpoint', function(done) {
      var fakeAmqpBase = {
        getReceiver: function(endpoint, callback) {
          assert.equal(endpoint, '/messages/serviceBound/feedback');
          callback();
        },
        setDisconnectHandler: function () {}
      };
      var amqp = new Amqp(fakeConfig, fakeAmqpBase);
      amqp.getFeedbackReceiver(done);
    });
  });

  describe('#getFileNotificationReceiver', function() {
    /*Tests_SRS_NODE_IOTHUB_SERVICE_AMQP_16_016: [The `getFileNotificationReceiver` method shall request an `AmqpReceiver` object from the base AMQP transport for the `/messages/serviceBound/filenotifications` endpoint.]*/
    it('requests an AmqpReceiver from the file notification endpoint', function(done) {
      var fakeAmqpBase = {
        getReceiver: function(endpoint, callback) {
          assert.equal(endpoint, '/messages/serviceBound/filenotifications');
          callback();
        },
        setDisconnectHandler: function () {}
      };
      var amqp = new Amqp(fakeConfig, fakeAmqpBase);
      amqp.getFileNotificationReceiver(done);
    });
  });
});