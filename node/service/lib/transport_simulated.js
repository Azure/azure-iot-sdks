// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var errors = require('azure-iot-common').errors;
var EventEmitter = require('events');
var util = require('util');

function FeedbackReceiver() {
  EventEmitter.call(this);
}
util.inherits(FeedbackReceiver, EventEmitter);

function SimulatedTransport() {
  this._config = {
    sharedAccessSignature: 'ok'
  };
  this._receiver = new FeedbackReceiver();
  this.FeedbackReceiver = FeedbackReceiver;
}

SimulatedTransport.prototype.connect = function connect(done) {
  if (!!done) done();
};

SimulatedTransport.prototype.disconnect = function disconnect(done) {
  if (!!done) done();
};

SimulatedTransport.prototype.send = function send(deviceId, message, done) {
  if (done) {
    if (deviceId.search(/^no-device/) !== -1) {
      done(new errors.DeviceNotFoundError());
    }
    else {
      done(null, { descriptor: 0x24 });
      if (message.ack === 'full') {
        this._receiver.emit('message', {
          body: [{
            originalMessageId: message.messageId,
            deviceId: deviceId
          }]
        });
      }
    }
  }
};

SimulatedTransport.prototype.getFeedbackReceiver = function (done) {
  if (this._config.sharedAccessSignature === 'fail') {
    done(new Error('error'));
  }
  else {
    done(null, this._receiver);
  }
};

module.exports = SimulatedTransport;
