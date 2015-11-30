// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events');
var util = require('util');
var endpoint = require('azure-iot-common').endpoint;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var cron = require('node-crontab');

var defaultOptions = {
  // 'interval' is a number, expressed in seconds. Default will poll for messages once per second.
  interval: 1,
  // 'at' is a Date object. Message(s) will be received at this time.
  at: null,
  // 'cron' is a cron string. Message(s) will be received according to the interval defined by the cron string.
  cron: null,
  // 'drain' is a boolean indicating whether all messages should be received at the same time (as opposed to one at a time, if set to 'false')
  drain: true
};

/**
 * @class HttpReceiver
 * @classdesc Provides a receiver link that can pull messages from the IoT Hub service and settle them.
 * 
 * @emits message When a message is received
 * @emits errorReceived When there was an error trying to receive messages
 * 
 */
function HttpReceiver(config, httpHelper) {
	EventEmitter.call(this);
  this._config = config;
  this._http = httpHelper;
  
  this._opts = defaultOptions;
  this._cronObj = null;
  this._intervalObj = null;
  this._timeoutObj = null;
  this._receiverStarted = false;
  
  
  this.on('newListener', function () {
    if (!this._receiverStarted) {
      this._startReceiver();
    }
  }.bind(this));
  
  this.on('removeListener', function () {
    if (this._receiverStarted && this.listenerCount('message') === 0) {
      this._stopReceiver();
    }
  }.bind(this));
}

util.inherits(HttpReceiver, EventEmitter);

HttpReceiver.prototype._startReceiver = function startReceiver() {
  if (!this._cronObj && !this._intervalObj && !this._timeoutObj) {
    if (this._opts.interval) {
      /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_021: [If opts.interval is set, messages should be received repeatedly at that interval]*/
      this._intervalObj = setInterval(this._receive.bind(this), this._opts.interval * 1000); // this._opts.interval is in seconds but setInterval takes milliseconds.
      this._receiverStarted = true;
    } else if (this._opts.at) {
      /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_003: [if opts.at is set, messages shall be received at the Date and time specified.]*/
      var now = new Date().getTime();
      var at = new Date(this._opts.at).getTime();
      var diff = at - now;
      if (diff < 0) {
        // Looks like we're already late. receive messages right away.
        // We do not set the "_receiverStarted" value though because we're not starting anything.
        this._receive.bind(this)();
      } else {
        this._timeoutObj = setTimeout(this._receive.bind(this), diff);    
        this._receiverStarted = true;  
      }
    } else if (this._opts.cron) {
      /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_020: [If opts.cron is set messages shall be received according to the schedule described by the expression.]*/
      this.cronObj = cron.scheduleJob(this._opts.cron, this._receive.bind(this));
      this._receiverStarted = true;
    }
  }
};

HttpReceiver.prototype._stopReceiver = function stopReceiver() {
  if (this._cronObj) { 
      cron.cancelJob(this._cronObj);
      this._cronObj = null;
      this._receiverStarted = false;
    }
    
    if (this._intervalObj) { 
      clearInterval(this._intervalObj); 
      this._intervalObj = null;
      this._receiverStarted = false;
    }
    
    if (this._timeoutObj) { 
      clearTimeout(this._timeoutObj); 
      this._timeoutObj = null;
      this._receiverStarted = false;
    }
};

HttpReceiver.prototype._receive = function receive() {
  var path = endpoint.messagePath(this._config.deviceId);
  var httpHeaders = {
    'Authorization': this._config.sharedAccessSignature.toString(),
    'iothub-to': path
  };
  
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_017: [If opts.drain is true all messages in the queue should be pulled at once.]*/ 
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_018: [If opts.drain is false, only one message shall be received at a time]*/
  var drainRequester = new EventEmitter();
  drainRequester.on('nextRequest', function () {
    var request = this._http.buildRequest('GET', path + endpoint.versionQueryString(), httpHeaders, this._config.host, function (err, body, res) {
      if (!err) {
        if (body) {
          var msg = this._http.toMessage(res, body);
          this.emit('message', msg);
          if (this._opts.drain) {
            drainRequester.emit('nextRequest');
          }
        }
      } else {
        err.response = res;
        err.responseBody = body;
        this.emit('errorReceived', err);
      }
    }.bind(this));
    request.end();
  }.bind(this));
  
  drainRequester.emit('nextRequest');
};


/**
 * This method sets the options defining how the receiver object should poll the IoT Hub service to get messages.
 * There is only one instance of the receiver object. If the receiver has already been created, calling setOptions will 
 * change the options of the existing instance and restart it.
 * 
 * @param {Object} opts Receiver options formatted as: { interval: (Number), at: (Date), cron: (string), drain: (Boolean) }
 */
/*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_001: [The setOptions method shall accept an argument formatted as such:
{
    interval: (Number),
    at: (Date)
    cron: (string)
    drain: (Boolean)
}]*/
HttpReceiver.prototype.setOptions = function setOptions (opts) {
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_019: [If the receiver is already running with a previous configuration, the existing receiver should be restarted with the new configuration]*/
  var restartReceiver = this._receiverStarted;
  if (this._receiverStarted) {
    this._stopReceiver();
  }
  
  if (!opts)
  {
    this._opts = defaultOptions;
  }
  
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_008: [Only one of the interval, at, and cron fields should be populated: if more than one is populated, an ArgumentError shall be thrown.]*/
  if (opts.interval && opts.cron || opts.interval && opts.at || opts.at && opts.cron) {
    throw new ArgumentError('Only one of the (interval|at|cron) fields should be set.');
  }
  
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_002: [opts.interval is not a number, an ArgumentError should be thrown.]*/ 
  if (opts.interval && typeof(opts.interval) !== "number") {
    throw new ArgumentError('The \'interval\' parameter must be a number');
  }
  
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_005: [If opts.interval is a negative number, an ArgumentError should be thrown.]*/
  if (opts.interval && opts.interval <= 0) {
    throw new ArgumentError('the \'interval\' parameter must be strictly greater than 0 (zero)');
  }
  
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_022: [If opts.at is not a Date object, an ArgumentError should be thrown]*/
  if (opts.at && !(opts.at instanceof Date)) {
    throw new ArgumentError('The \'at\' parameter must be a Date');
  }
  
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_004: [if opts.cron is set it shall be a string that can be interpreted as a cron expression]*/
  if (opts.cron && typeof(opts.cron) !== "string") {
    throw new ArgumentError('The \'at\' parameter must be a String and use the cron syntax (see https://www.npmjs.com/package/node-crontab)');
  }
  
  this._opts = opts;
  
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_019: [If the receiver is already running with a previous configuration, the existing receiver should be restarted with the new configuration]*/
  if (restartReceiver) {
    this._startReceiver();
  }
};

/**
 * Sends a completion message to the IoT Hub service, effectively removing the message from the queue and flagging it as succesfully delivered.
 */
HttpReceiver.prototype.complete = function complete(message, done) {
	this._sendFeedback('complete', message, done);
};

/**
 * Sends an abandon message to the IoT Hub service. The message remains in the queue and the service will retry delivering it.
 */
HttpReceiver.prototype.abandon = function abandon(message, done) {
	this._sendFeedback('abandon', message, done);
};

/**
 * Sends a rejection message to the IoT Hub service, effectively removing the message from the queue and flagging it as rejected.
 */
HttpReceiver.prototype.reject = function reject(message, done) {
	this._sendFeedback('reject', message, done);
};

/**
 * This method sends the feedback action to the IoT Hub.
 *
 * @param {String}  action    This parameter must be equal to one of the
 *                            following possible values:
 *
 * | Value    | Action                                                                                  |
 * |----------|-----------------------------------------------------------------------------------------|
 * | abandon  | Directs the IoT Hub to re-enqueue a message so it may be received again later.          |
 * | reject   | Directs the IoT Hub to delete a message from the queue and record that it was rejected. |
 * | complete | Directs the IoT Hub to delete a message from the queue and record that it was accepted. |
 *
 * @param {String}        message   The message for which feedback is being sent.
 * @param {Function}      done      The callback to be invoked when
 *                                  `sendFeedback` completes execution.
 */
HttpReceiver.prototype._sendFeedback = function (action, message, done) {
  var config = this._config;
  var method;
  var path = endpoint.feedbackPath(config.deviceId, message.lockToken);
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature.toString(),
    'If-Match': message.lockToken
  };
  
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_009: [abandon shall construct an HTTP request using information supplied by the caller, as follows:
  POST <config.host>/devices/<config.deviceId>/messages/devicebound/<lockToken>/abandon?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  If-Match: <lockToken>
  Host: <config.host>] 
  */
  if (action === 'abandon') {
    path += '/abandon' + endpoint.versionQueryString();
    method = 'POST';
  }
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_010: [reject shall construct an HTTP request using information supplied by the caller, as follows:
  DELETE <config.host>/devices/<config.deviceId>/messages/devicebound/<lockToken>?api-version=<version>&reject HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  If-Match: <lockToken>
  Host: <config.host>]*/
  else if (action === 'reject') {
    path += endpoint.versionQueryString() + '&reject';
    method = 'DELETE';
  }
  /*Codes_SRS_NODE_DEVICE_HTTP_RECEIVER_16_011: [complete shall construct an HTTP request using information supplied by the caller, as follows:
  DELETE <config.host>/devices/<config.deviceId>/messages/devicebound/<lockToken>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  If-Match: <lockToken>
  Host: <config.host>]*/
  else {
    path += endpoint.versionQueryString();
    method = 'DELETE';
  }

  /*Codes_SRS_NODE_DEVICE_HTTP_05_008: [If any Http method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  var request = this._http.buildRequest(method, path, httpHeaders, config.host, function(err, res, body){
    if (!err) done (null, res);
    else {
      err.response = res;
      err.responseBody = body;
      done(err);
    }
  });
  
  request.end();
};

module.exports = HttpReceiver;