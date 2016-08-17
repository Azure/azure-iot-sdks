// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var EventEmitter = require('events').EventEmitter;
var util = require('util');

var responseTopic = '$iothub/twin/res/#';
var postTopic = '$iothub/twin/POST/desired';

var FakeMqtt = function() {
  EventEmitter.call(this);

  this.publishShouldSucceed = function (shouldSucceed) {
    this._publishSucceeds = shouldSucceed;
  };

  this.publish = function(topic, message, options, callback) {
    this.publishoptions = options;
    this.topicString = topic;
    if (this._publishSucceeds) {
      callback(null, {puback: 'success'});
    } else {
      callback(new Error('Invalid topic'));
    }
  };

  this.publishWasCalled = function() {
    return this.publishoptions;
  };

  this.connect = function() {
    // This object does double-duty.  It is both a replacement for mqtt.js and also a replacement for the mqtt.js Client object. Gross!
    return this;
  };

  this.subscribe = function(topicName, param, done) {
    if (this.subscribeShouldFail) {
      done (new Error('Not authorized'));
    } else {
      done();
    }
  };

  this.unsubscribe = function(topicName, done) {
    done();
  };

  this.fakeMessageFromService = function(topic, message) {
    this.emit('message', topic, message);
  }
};

util.inherits(FakeMqtt, EventEmitter);

module.exports = FakeMqtt;
