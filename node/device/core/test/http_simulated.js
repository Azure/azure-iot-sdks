// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events').EventEmitter;
var Message = require('azure-iot-common').Message;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var SharedAccessSignature = require('../lib/shared_access_signature.js');
var results = require('azure-iot-common').results;

function Response(statusCode) {
  this.statusCode = statusCode;
}

function makeError(statusCode) {
  var err = new Error();
  err.response = new Response(statusCode);
  return err;
}

function SimulatedHttp(config) {
  this._receiver = null;
  this.handleRequest = function (done) {
    if (this._x509) {
        done(null, new results.MessageEnqueued(new Response(204)));
    } else {
      var sig = SharedAccessSignature.parse(config.sharedAccessSignature);

      if (config.host.indexOf('bad') >= 0) {                      // bad host
        done(new Error('getaddrinfo ENOTFOUND bad'));
      }
      else if (config.deviceId.indexOf('bad') >= 0) {             // bad policy
        done(makeError(404));
      }
      else {
        var cmpSig = (SharedAccessSignature.create(config.host, config.deviceId, 'bad', sig.se)).toString();
        if (config.sharedAccessSignature === cmpSig) {  // bad key
          done(makeError(401));
        }
        else {
          done(null, new results.MessageEnqueued(new Response(204)));
        }
      }
    }
  };
}

SimulatedHttp.prototype.setOptions = function() {
  this._x509 = true;
};

SimulatedHttp.prototype.sendEvent = function (message, done) {
  this.handleRequest(function (err, response) {
    done(err, response);
  });
};

SimulatedHttp.prototype.sendEventBatch = function (message, done) {
  this.handleRequest(function (err, response) {
    done(err, response);
  });
};

SimulatedHttp.prototype.receive = function (done) {
  this.handleRequest(function (err, response) {
    done(err, err ? null : new Message(''), response);
  });
};

SimulatedHttp.prototype.getReceiver = function (done) {
  if (!this._receiver) { this._receiver = new EventEmitter(); }
  done(null, this._receiver);
};

SimulatedHttp.prototype.sendFeedback = function (feedbackAction, message, done) {
  if (!message.lockToken) {
    done(new ArgumentError('invalid lockToken'));
  }
  else if (message.lockToken === 'FFA945D3-9808-4648-8DD7-D250DDE66EA9') {
    done(makeError(412));
  }
  else {
    this.handleRequest(function (err, res) {
      done(err, res);
    });
  }
};

module.exports = SimulatedHttp;