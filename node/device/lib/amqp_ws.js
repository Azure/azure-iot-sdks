// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var util = require('util');
var Base = require('azure-iot-common').Amqp;
var Amqp = require('./amqp.js');

/** 
 * @class AmqpWS
 * @classdesc Constructs an {@linkcode AmqpWS} object that can be used on a device to send 
 *            and receive messages to and from an IoT Hub instance, using the AMQP protocol over secure websockets.
 * @augments Amqp
 */
function AmqpWS(config) {
  this._config = config;
  var uri = 'wss://';
  uri += encodeURIComponent(this._config.deviceId) +
         '@sas.' +
         this._config.hubName +
         ':' +
         encodeURIComponent(this._config.sharedAccessSignature) +
         '@' +
         this._config.host + ':443/$iothub/websocket';
      
  this._amqp = new Base(uri, false);
}

util.inherits(AmqpWS, Amqp);

module.exports = AmqpWS;