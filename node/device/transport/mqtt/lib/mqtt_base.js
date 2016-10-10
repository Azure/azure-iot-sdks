// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var MqttReceiver = require('./mqtt_receiver.js');
var debug = require('debug')('mqtt-common');
var PackageJson = require('../package.json');
var results = require('azure-iot-common').results;
var endpoint = require('azure-iot-common').endpoint;

/**
 * @class           module:azure-iot-device-mqtt.MqttBase
 * @classdesc       Base MQTT transport implementation used by higher-level IoT Hub libraries.
 *                  You generally want to use these higher-level objects (such as [azure-iot-device-mqtt.Mqtt]{@link module:azure-iot-device-mqtt.Mqtt})
 *                  rather than this one.
 *
 * @param {Object}  config      The configuration object derived from the connection string.
 */
/*Codes_SRS_NODE_COMMON_MQTT_BASE_16_004: [The `MqttBase` constructor shall instanciate the default MQTT.JS library if no argument is passed to it.]*/
/*Codes_SRS_NODE_COMMON_MQTT_BASE_16_005: [The `MqttBase` constructor shall use the object passed as argument instead of the default MQTT.JS library if it's not falsy.]*/
function MqttBase(mqttprovider) {
  this.mqttprovider = mqttprovider ? mqttprovider : require('mqtt');
}

/**
 * @method            module:azure-iot-device-mqtt.MqttBase#connect
 * @description       Establishes a secure connection to the IoT Hub instance using the MQTT protocol.
 *
 * @param {Function}  done  Callback that shall be called when the connection is established.
 */
MqttBase.prototype.connect = function (config, done) {
  /*Codes_SRS_NODE_COMMON_MQTT_BASE_16_006: [The `connect` method shall throw a ReferenceError if the config argument is falsy, or if one of the following properties of the config argument is falsy: deviceId, host, and one of sharedAccessSignature or x509.cert and x509.key.]*/
  if ((!config) ||
    (!config.host) ||
    (!config.deviceId) ||
    (!config.sharedAccessSignature && (!config.x509 || !config.x509.cert || !config.x509.key))) {
    throw new ReferenceError('Invalid transport configuration');
  }

  this._receiver = null;
  var uri = config.uri || 'mqtts://' + config.host;
  this._topicTelemetryPublish = "devices/" + config.deviceId + "/messages/events/";
  this._topicMessageSubscribe = "devices/" + config.deviceId + "/messages/devicebound/#";
  debug('topic publish: ' + this._topicTelemetryPublish);
  debug('topic subscribe: ' + this._topicMessageSubscribe);
  var versionString = encodeURIComponent('azure-iot-device/' + PackageJson.version);

  /*Codes_SRS_NODE_COMMON_MQTT_BASE_16_002: [The `connect` method shall use the authentication parameters contained in the `config` argument to connect to the server.]*/
  this._options =
  {
    cmd: 'connect',
    protocolId: 'MQTT',
    protocolVersion: 4,
    clean: false,
    clientId: config.deviceId,
    rejectUnauthorized: true,
    username: config.host + '/' + config.deviceId +
              '/DeviceClientType=' + versionString +
              '&' + endpoint.versionQueryString().substr(1),
    reconnectPeriod: 0  // Client will handle reconnection at the higher level.
  };

  if (config.sharedAccessSignature) {
    this._options.password = new Buffer(config.sharedAccessSignature);
    debug('username: ' + this._options.username);
    debug('uri:      ' + uri);
  } else {
    this._options.cert = config.x509.cert;
    this._options.key = config.x509.key;
    this._options.passphrase = config.x509.passphrase;
  }

  this.client = this.mqttprovider.connect(uri, this._options);
  /*Codes_SRS_NODE_COMMON_MQTT_BASE_16_007: [The `connect` method shall not throw if the `done` argument has not been passed.]*/
  if (done) {
    var errCallback = function (error) {
      done(error);
    };

  /*Codes_SRS_NODE_COMMON_MQTT_BASE_16_003: [The `connect` method shall call the `done` callback with a standard javascript `Error` object if the connection failed.]*/
    this.client.on('error', errCallback);
    this.client.on('close', errCallback);
    this.client.on('offline', errCallback);
    this.client.on('disconnect', errCallback);

    this.client.on('connect', function (connack) {
      debug('Device is connected');
      debug('CONNACK: ' + JSON.stringify(connack));

      this.client.removeListener('close', errCallback);
      this.client.removeListener('offline', errCallback);
      this.client.removeListener('disconnect', errCallback);

      this.client.on('close', function () {
        debug('Device connection to the server has been closed');
        this._receiver = null;
      }.bind(this));

      /*Codes_SRS_NODE_COMMON_MQTT_BASE_12_005: [The `connect` method shall call connect on MQTT.JS  library and call the `done` callback with a `null` error object and the result as a second argument.]*/
      done(null, connack);
    }.bind(this));
  }
};

/**
 * @method            module:azure-iot-device-mqtt.MqttBase#disconnect
 * @description       Terminates the connection to the IoT Hub instance.
 *
 * @param {Function}  done      Callback that shall be called when the connection is terminated.
 */
MqttBase.prototype.disconnect = function (done) {
  this.client.removeAllListeners();
  // The first parameter decides whether the connection should be forcibly closed without waiting for all sent messages to be ACKed.
  // This should be a transport-specific parameter.
  /* Codes_SRS_NODE_HTTP_16_001: [The disconnect method shall call the done callback when the connection to the server has been closed.] */
  this.client.end(false, done);
};

/**
 * @method            module:azure-iot-device-mqtt.MqttBase#publish
 * @description       Publishes a message to the IoT Hub instance using the MQTT protocol.
 *
 * @param {Object}    message   Message to send to the IoT Hub instance.
 * @param {Function}  done      Callback that shall be called when the connection is established.
 */
/* Codes_SRS_NODE_HTTP_12_006: The PUBLISH method shall throw ReferenceError “Invalid message” if the message is falsy */
/* Codes_SRS_NODE_HTTP_12_007: The PUBLISH method shall call publish on MQTT.JS library with the given message */
MqttBase.prototype.publish = function (message, done) {
  if (!message) {
    throw new ReferenceError('Invalid message');
  }

  this.client.publish(this._topicTelemetryPublish, message.data.toString(), { qos: 1, retain: false }, function (err, puback) {
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
 * @method              module:azure-iot-device-mqtt.MqttBase#getReceiver
 * @description         Gets a receiver object that is used to receive and settle messages.
 *
 * @param {Function}    done   callback that shall be called with a receiver object instance.
 */
/*Codes_SRS_NODE_DEVICE_MQTT_BASE_16_002: [If a receiver for this endpoint has already been created, the getReceiver method should call the done() method with the existing instance as an argument.] */
/*Codes_SRS_NODE_DEVICE_MQTT_BASE_16_003: [If a receiver for this endpoint doesn’t exist, the getReceiver method should create a new MqttReceiver object and then call the done() method with the object that was just created as an argument.] */
MqttBase.prototype.getReceiver = function (done) {
  if (!this._receiver) {
    this._receiver = new MqttReceiver(
      this.client,
      this._topicMessageSubscribe
    );
  }

  done(null, this._receiver);
};

module.exports = MqttBase;