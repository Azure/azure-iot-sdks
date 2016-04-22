// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var util = require('util');
var Base = require('azure-iot-amqp-base').Amqp;
var Amqp = require('./amqp.js');
var PackageJson = require('../package.json');

/**
 * @class       module:azure-iothub.AmqpWs
 * @classdesc   Constructs an {@linkcode Amqp} object that can be used in an application
 *              to connect to IoT Hub instance, using the AMQP protocol over secure websockets.
 *              This class overloads the constructor of the base {@link module:azure-iothub--amqp:Amqp} class from the AMQP transport, and inherits all methods from it.
 */
/*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_WS_16_001: [The `AmqpWs` constructor shall accept a config object with those four properties:
- `host` – (string) the fully-qualified DNS hostname of an IoT Hub
- `hubName` - (string) the name of the IoT Hub instance (without suffix such as .azure-devices.net).
- `keyName` – (string) the name of a key that can be used to communicate with the IoT Hub instance
- `sharedAccessSignature–` (string) the key associated with the key name.]*/
function AmqpWs(config) {
  this._config = config;
  var uri = 'wss://' +
            encodeURIComponent(this._config.keyName) +
            '%40sas.root.' +
            this._config.hubName +
            ':' +
            encodeURIComponent(this._config.sharedAccessSignature) +
            '@' +
            this._config.host + ':443/$iothub/websocket';
 
  this._amqp = new Base(uri, false, PackageJson.name + '/' + PackageJson.version);
}

/*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_WS_16_002: [`AmqpWs` should inherit from `Amqp`.]*/
util.inherits(AmqpWs, Amqp);

module.exports = AmqpWs;