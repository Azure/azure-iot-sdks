// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var MqttReceiver = require('./mqtt_receiver.js');
var debug = require('debug')('mqtt-common');
var PackageJson = require('../package.json');
var results = require('azure-iot-common').results;

/**
 * @class           module:azure-iot-mqtt-base.Mqtt
 * @classdesc       Base MQTT transport implementation used by higher-level IoT Hub libraries.
 *                  You generally want to use these higher-level objects (such as [azure-iot-device-mqtt.Mqtt]{@link module:azure-iot-device-mqtt.Mqtt})
 *                  rather than this one.
 *
 * @param {Object}  config      The configuration object derived from the connection string.
 */
/*
 Codes_SRS_NODE_HTTP_12_001: MqttTransport shall accept the following argument:
    config [
        host: host address
        deviceID: device name
        sharedAccessSignature: SAS token  created for IoTHub
        gatewayHostName: gateway host name]
 Codes_SRS_NODE_HTTP_12_002: [MqttTransport shall throw ReferenceError “Invalid transport configuration” error if either of the configuration field is falsy
 Codes_SRS_NODE_HTTP_12_003: [MqttTransport shall create a configuration structure for underlying MQTT.JS library and store it to a member variable
 Codes_SRS_NODE_HTTP_12_004: [MqttTransport shall return an instance itself
*/
function Mqtt(config, mqttprovider) {
  if ((!config) ||
    (!config.host) ||
    (!config.deviceId) ||
    (!config.sharedAccessSignature)) {
    throw new ReferenceError('Invalid transport configuration');
  }

  this.mqttprovider = mqttprovider ? mqttprovider : require('mqtt');

  this._receiver = null;
  this._hostName = 'mqtts://' + config.host;
  this._topic_publish = "devices/" + config.deviceId + "/messages/events/";
  this._topic_subscribe = "devices/" + config.deviceId + "/messages/devicebound/#";
  debug('topic publish: ' + this._topic_publish);
  debug('topic subscribe: ' + this._topic_subscribe);
  var versionString = encodeURIComponent('azure-iot-device/' + PackageJson.version);
  this._options =
  {
    cmd: 'connect',
    protocolId: 'MQTT',
    protocolVersion: 4,
    clean: false,
    clientId: config.deviceId,
    username: config.host + '/' + config.deviceId + '/DeviceClientType=' + versionString,
    password: new Buffer(config.sharedAccessSignature),
    rejectUnauthorized: false,
    reconnectPeriod: 0  // Client will handle reconnection at the higher level.
  };
  debug('username: ' + this._options.username);
  debug('hostname: ' + this._hostName);
}

/**
 * @method            module:azure-iot-mqtt-base.Mqtt#connect
 * @description       Establishes a secure connection to the IoT Hub instance using the MQTT protocol.
 *
 * @param {Function}  done  Callback that shall be called when the connection is established.
 */
/* SRS_NODE_HTTP_12_005: The CONNECT method shall call connect on MQTT.JS  library and return a promise with the result */
Mqtt.prototype.connect = function (done) {
  this.client = this.mqttprovider.connect(this._hostName, this._options);
  if (done) {
    var errCallback = function (error) {
      done(error);
    };
    this.client.on('error', errCallback);

    this.client.on('close', function () {
      debug('Device connection to the server has been closed');
      this._receiver = null;
    }.bind(this));

    this.client.on('offline', function () {
      debug('Device is offline');
    }.bind(this));

    this.client.on('reconnect', function () {
      debug('Device is trying to reconnect');
    }.bind(this));

    this.client.on('connect', function (connack) {
      debug('Device is connected');
      debug('CONNACK: ' + JSON.stringify(connack));
      done(null, connack);
    }.bind(this));
  }
};

/**
 * @method            module:azure-iot-mqtt-base.Mqtt#disconnect
 * @description       Terminates the connection to the IoT Hub instance.
 *
 * @param {Function}  done      Callback that shall be called when the connection is terminated.
 */
Mqtt.prototype.disconnect = function (done) {
  this.client.removeAllListeners();
  // The first parameter decides whether the connection should be forcibly closed without waiting for all sent messages to be ACKed.
  // This should be a transport-specific parameter.
  /* Codes_SRS_NODE_HTTP_16_001: [The disconnect method shall call the done callback when the connection to the server has been closed.] */
  this.client.end(false, done);
};

/**
 * @method            module:azure-iot-mqtt-base.Mqtt#publish
 * @description       Publishes a message to the IoT Hub instance using the MQTT protocol.
 *
 * @param {Object}    message   Message to send to the IoT Hub instance.
 * @param {Function}  done      Callback that shall be called when the connection is established.
 */
/* Codes_SRS_NODE_HTTP_12_006: The PUBLISH method shall throw ReferenceError “Invalid message” if the message is falsy */
/* Codes_SRS_NODE_HTTP_12_007: The PUBLISH method shall call publish on MQTT.JS library with the given message */
Mqtt.prototype.publish = function (message, done) {
  if (!message) {
    throw new ReferenceError('Invalid message');
  }

  this.client.publish(this._topic_publish, message.data.toString(), { qos: 1, retain: false }, function (err, puback) {
    if (done) {
      if (err) {
        done(err);
      } else {
        debug('PUBACK: ' + JSON.stringify(puback));
        done(null, new results.MessageEnqueued(puback));
      }
    }
  }.bind(this));
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#getReceiver
 * @description         Gets a receiver object that is used to receive and settle messages.
 *
 * @param {Function}    done   callback that shall be called with a receiver object instance.
 */
/*Codes_SRS_NODE_DEVICE_MQTT_16_002: [If a receiver for this endpoint has already been created, the getReceiver method should call the done() method with the existing instance as an argument.] */
/*Codes_SRS_NODE_DEVICE_MQTT_16_003: [If a receiver for this endpoint doesn’t exist, the getReceiver method should create a new MqttReceiver object and then call the done() method with the object that was just created as an argument.] */
Mqtt.prototype.getReceiver = function (done) {
  if (!this._receiver) {
    this._receiver = new MqttReceiver(this.client, this._topic_subscribe);
  }

  done(null, this._receiver);
};

module.exports = Mqtt;