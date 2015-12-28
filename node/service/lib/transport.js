// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var amqp10 = require('amqp10');
var AmqpMessage = require('./amqp_message.js');
var errors = require('azure-iot-common').errors;
var EventEmitter = require('events');
var util = require('util');

function FeedbackReceiver(amqpReceiver) {
  EventEmitter.call(this);
  this._amqpReceiver = amqpReceiver;

  this.on('newListener', function (event) {
    // lazy-init AMQP event listeners
    if (event === 'message' && this.listenerCount('message') === 0) {
      this._setupAmqpReceiverListeners();
    }
  }.bind(this));

  this.on('removeListener', function (event) {
    // stop listening for AMQP events if our consumers stop listening for our events
    if (event === 'message' && this.listenerCount('message') === 0) {
      this._removeAmqpReceiverListeners();
    }
  }.bind(this));
}

util.inherits(FeedbackReceiver, EventEmitter);

FeedbackReceiver.prototype._onAmqpErrorReceived = function (err) {
  this.emit('errorReceived', translateError(err));
};
  
FeedbackReceiver.prototype._onAmqpMessage = function (feedbackRecords) {
  this.emit('message', feedbackRecords);
};
  
FeedbackReceiver.prototype._setupAmqpReceiverListeners = function () {
  this._amqpReceiver.on('errorReceived', this._onAmqpErrorReceived.bind(this));
  this._amqpReceiver.on('message', this._onAmqpMessage.bind(this));
};

FeedbackReceiver.prototype._removeAmqpReceiverListeners = function () {
  this._amqpReceiver.removeListener('errorReceived', this._onAmqpErrorReceived.bind(this));
  this._amqpReceiver.on('message', this._onAmqpMessage.bind(this));
};

function Transport(config) {
  this._config = config;
  this._amqp = new amqp10.Client(amqp10.Policy.merge({
    senderLink: {
      reattach: {
        retries: 0,
        forever: false
      }
    }
  }, amqp10.Policy.EventHub));
  this._connectPromise = null;
  this._senderPromise = null;
  this._receiverPromise = null;
  this._feedbackReceiver = null;

  this.FeedbackReceiver = FeedbackReceiver;
}

Transport.prototype.connect = function connect(done) {
  var cfg = this._config;

  var uri = 'amqps://' +
    encodeURIComponent(cfg.keyName) + '%40sas.root.' + cfg.hubName + ':' +
    encodeURIComponent(cfg.sharedAccessSignature) + '@' +
    cfg.host;

  this._amqp.on('client:errorReceived', function (err) {
    this.disconnect(function () {
      if (done) done(translateError(err));
    });
  }.bind(this));

  if (!this._connectPromise) {
    this._connectPromise = this._amqp.connect(uri);
  }

  this._connectPromise
    .then(function () {
      if (done) done();
    })
    .catch(function (err) {
      if (done) done(translateError(err));
    });
};

Transport.prototype.disconnect = function disconnect(done) {
  this._connectPromise = null;
  this._amqp.disconnect()
    .then(function () {
      if (done) done();
    })
    .catch(function (err) {
      if (done) done(err);
    });
};

Transport.prototype.send = function send(deviceId, message, done) {
  var endpoint = '/messages/devicebound';
  var to = '/devices/' + encodeURIComponent(deviceId) + '/messages/devicebound';

  var amqpMessage = AmqpMessage.fromMessage(message);
  amqpMessage.properties.to = to;

  this.connect(function (err) {
    if (err) {
      if (done) done(err);
    }
    else {
      if (!this._senderPromise) {
        this._senderPromise = this._amqp.createSender(endpoint);
      }

      this._senderPromise
        .then(function (sender) {
          sender.on('errorReceived', function (err) {
            if (done) done(translateError(err));
          });
          return sender.send(amqpMessage);
        })
        .then(function (state) {
          if (done) done(null, state);
        })
        .catch(function (err) {
          if (done) done(translateError(err));
        });
    }
  }.bind(this));
};

Transport.prototype.getFeedbackReceiver = function getFeedbackReceiver(done) {
  if (!this._feedbackReceiver) {
    this._setupReceiverLink(done);
  }
  else {
    done(null, this._feedbackReceiver);
  }
};

Transport.prototype._setupReceiverLink = function setupReceiverLink(done) {
  var endpoint = '/messages/serviceBound/feedback';

  this.connect(function (err) {
    if (err) {
      if (done) done(err);
    }
    else {
      if (!this._receiverPromise) {
        this._receiverPromise = this._amqp.createReceiver(endpoint);
      }

      this._receiverPromise
        .then(function (receiver) {
          this._feedbackReceiver = new FeedbackReceiver(receiver);
          done(null, this._feedbackReceiver);
        }.bind(this))
        .catch(function (err) {
          if (done) done(translateError(err));
        });
    }
  }.bind(this));
};

function translateError(err) {
  var error = err;

  if (err.constructor.name === 'AMQPError') {
    if (err.condition.contents === 'amqp:resource-limit-exceeded') {
      error = new errors.DeviceMaximumQueueDepthExceededError(err.description);
    }
    else if (err.condition.contents === 'amqp:not-found') {
      error = new errors.DeviceNotFoundError(err.description);
    }
    else if (err.condition.contents === 'amqp:unauthorized-access') {
      error = new errors.UnauthorizedError(err.description);
    }
    else {
      error = new Error(err.description);
    }
    error.transport = err;
  }
  else if (err instanceof amqp10.Errors.AuthenticationError) {
    error = new errors.UnauthorizedError(err.message);
  }

  return error;
}

module.exports = Transport;