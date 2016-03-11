// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var EventEmitter = require('events');
var util = require('util');

var FakeMqtt = function() {
  EventEmitter.call(this);

  this.publishShouldSucceed = function (shouldSucceed) {
    this._publishSucceeds = shouldSucceed;
  };

  this.publish = function(topic, message, options, callback) {
    this.publishoptions = options;
    if (this._publishSucceeds) {
      callback(null, {puback: 'success'});
    } else {
      callback(new Error('publish failed'));
    }
  };

  setTimeout(function(){
    this.emit('connect');
  }.bind(this), 0);
};

FakeMqtt.connect = function() {
  return new FakeMqtt();
};

util.inherits(FakeMqtt, EventEmitter);

module.exports = FakeMqtt;