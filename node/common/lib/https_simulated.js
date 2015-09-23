// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Message = require('./message.js');

var INVALID_CONFIG_NAME = 'bad';

function Response(statusCode) {
  this.statusCode = statusCode;
}

function SimulatedHttps() {
  this.handleRequest = function(config, done, isServiceCall, successMsg) {

    if (config.host === INVALID_CONFIG_NAME) {
      done(new Error('getaddrinfo ENOTFOUND bad'));
    }
    else if (config.keyName === INVALID_CONFIG_NAME) {
      done(new Error(), new Response(isServiceCall ? 401 : 404));
    }
    else if (config.key === INVALID_CONFIG_NAME) {
      done(new Error(), new Response(401));
    }
    else {
      var msg = successMsg ? successMsg : new Message(''); 
      done(null, new Response(204), msg);
    }
  };
}

SimulatedHttps.prototype.sendEvent = function (message, config, done) {
  this.handleRequest(config, done);
};

SimulatedHttps.prototype.sendEventBatch = function (message, config, done) {
  this.handleRequest(config, done);
};

SimulatedHttps.prototype.receive = function (config, done) {
  this.handleRequest(config, done);
};

SimulatedHttps.prototype.sendFeedback = function (feedbackAction, lockToken, config, done) {
  if (lockToken === '') {
    done(new Error('lockToken not valid'), null);
  }
  else {
    this.handleRequest(config, done);
  }
};

SimulatedHttps.prototype.createDevice = function (path, deviceInfo, config, done) {
  this.handleRequest(config, done, true, new Message('{ "deviceId": "testDevice" }'));
};

SimulatedHttps.prototype.updateDevice = function (path, deviceInfo, config, done) {
  this.handleRequest(config, done, true, new Message('{ "deviceId": "testDevice" }'));
};

SimulatedHttps.prototype.getDevice = function (path, config, done) {
  this.handleRequest(config, done, true, new Message('{ "deviceId": "testDevice" }'));
};

SimulatedHttps.prototype.listDevice = function (path, config, done) {
  this.handleRequest(config, done, true, new Message("[{ \"deviceId\": \"testDevice1\" },{ \"deviceId\": \"testDevice2\" }]"));
};

SimulatedHttps.prototype.deleteDevice = function (path, config, done) {
  this.handleRequest(config, done, true, new Message('{ "deviceId": "testDevice" }'));
};

module.exports = SimulatedHttps;