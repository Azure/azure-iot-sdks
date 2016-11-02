// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

require('es5-shim');
var Base = require('./mqtt_base.js');
var results = require('azure-iot-common').results;
var errors = require('azure-iot-common').errors;
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var debug = require('debug')('device:mqtt');
var translateError = require('../lib/mqtt-translate-error.js');
var MqttTwinReceiver = require('../lib/mqtt-twin-receiver.js');

var TOPIC_RESPONSE_PUBLISH_FORMAT = "$iothub/%s/res/%d/?$rid=%s";

/**
 * @class        module:azure-iot-device-mqtt.Mqtt
 * @classdesc    Provides MQTT transport for the device [client]{@link module:azure-iot-device.Client} class.
 *
 * @param   {Object}    config  Configuration object derived from the connection string by the client.
 */
/*
 Codes_SRS_NODE_DEVICE_MQTT_12_001: [The `Mqtt` constructor shall accept the transport configuration structure
 Codes_SRS_NODE_DEVICE_MQTT_12_002: [The `Mqtt` constructor shall store the configuration structure in a member variable
 Codes_SRS_NODE_DEVICE_MQTT_12_003: [The Mqtt constructor shall create an base transport object and store it in a member variable.]
*/
function Mqtt(config, provider) {
  EventEmitter.call(this);
  this._config = config;
  /*Codes_SRS_NODE_DEVICE_MQTT_16_016: [The Mqtt constructor shall initialize the `uri` property of the `config` object to `mqtts://<host>`.]*/  
  this._config.uri = "mqtts://" + config.host;
  /* Codes_SRS_NODE_DEVICE_MQTT_18_025: [ If the Mqtt constructor receives a second parameter, it shall be used as a provider in place of mqtt.js ]   */
  if (provider) {
    this._mqtt = new Base(provider);
  } else {
    this._mqtt = new Base();
  }
}

util.inherits(Mqtt, EventEmitter);

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#connect
 * @description         Establishes the connection to the Azure IoT Hub instance using the MQTT protocol.
 *
 * @param {Function}    done   callback that shall be called when the connection is established.
 */
/* Codes_SRS_NODE_DEVICE_MQTT_12_004: [The connect method shall call the connect method on MqttTransport */
Mqtt.prototype.connect = function (done) {
  this._twinReceiver = null;
  this._mqtt.connect(this._config, function (err, result) {
    if (err) {
      if (done) done(err);
    } else {
      this._mqtt.client.on('disconnect', function (err) {
        this.emit('disconnect', err);
      }.bind(this));

      if (done) done(null, result);
    }
  }.bind(this));
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#disconnect
 * @description         Terminates the connection to the IoT Hub instance.
 *
 * @param {Function}    done      Callback that shall be called when the connection is terminated.
 */
/* Codes_SRS_NODE_DEVICE_MQTT_16_001: [The disconnect method should call the disconnect method on MqttTransport.] */
Mqtt.prototype.disconnect = function (done) {
  this._mqtt.disconnect(function (err, result) {
    if (done) done(err, result);
  });
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#sendEvent
 * @description         Sends an event to the server.
 *
 * @param {Message}     message   Message used for the content of the event sent to the server.
 */
/* Codes_SRS_NODE_DEVICE_MQTT_12_005: [The sendEvent method shall call the publish method on MqttTransport */
Mqtt.prototype.sendEvent = function (message, done) {
  this._mqtt.publish(message, done);
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#getReceiver
 * @description         Gets a receiver object that is used to receive and settle messages.
 *
 * @param {Function}    done   callback that shall be called with a receiver object instance.
 */

Mqtt.prototype.getReceiver = function (done) {
  this._mqtt.getReceiver(done);
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#complete
 * @description         Settles the message as complete and calls the done callback with the result.
 *
 * @param {Message}     message     The message to settle as complete.
 * @param {Function}    done        The callback that shall be called with the error or result object.
 */
/*Codes_SRS_NODE_DEVICE_MQTT_16_005: [The ‘complete’ method shall call the callback given as argument immediately since all messages are automatically completed.]*/
Mqtt.prototype.complete = function (message, done) {
  done(null, new results.MessageCompleted());
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#reject
 * @description         Settles the message as rejected and calls the done callback with the result.
 *
 * @throws {Error}      The MQTT transport does not support rejecting messages.
 */
/*Codes_SRS_NODE_DEVICE_MQTT_16_006: [The ‘reject’ method shall throw because MQTT doesn’t support rejecting messages.] */
Mqtt.prototype.reject = function () {
  throw new errors.NotImplementedError('the MQTT transport does not support rejecting messages.');
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#abandon
 * @description         Settles the message as abandoned and calls the done callback with the result.
 *
 * @throws {Error}      The MQTT transport does not support rejecting messages.
 */
/*Codes_SRS_NODE_DEVICE_MQTT_16_004: [The ‘abandon’ method shall throw because MQTT doesn’t support abandoning messages.] */
Mqtt.prototype.abandon = function () {
  throw new errors.NotImplementedError('The MQTT transport does not support abandoning messages.');
};

/**
 * @method          module:azure-iot-device-mqtt.Mqtt#updateSharedAccessSignature
 * @description     This methods sets the SAS token used to authenticate with the IoT Hub service.
 *
 * @param {String}        sharedAccessSignature  The new SAS token.
 * @param {Function}      done      The callback to be invoked when `updateSharedAccessSignature` completes.
 */
Mqtt.prototype.updateSharedAccessSignature = function (sharedAccessSignature, done) {
  /*Codes_SRS_NODE_DEVICE_MQTT_16_008: [The updateSharedAccessSignature method shall disconnect the current connection operating with the deprecated token, and re-iniialize the transport object with the new connection parameters.]*/
  this._mqtt.disconnect(function (err) {
    if (err) {
      /*Codes_SRS_NODE_DEVICE_MQTT_16_009: [The updateSharedAccessSignature method shall call the `done` method with an Error object if updating the configuration or re-initializing the transport object.]*/
      if (done) done(err);
    } else {
      /*Codes_SRS_NODE_DEVICE_MQTT_16_007: [The updateSharedAccessSignature method shall save the new shared access signature given as a parameter to its configuration.]*/
      this._config.sharedAccessSignature = sharedAccessSignature;
      this._mqtt = new Base();
      /*Codes_SRS_NODE_DEVICE_MQTT_16_010: [The updateSharedAccessSignature method shall call the `done` callback with a null error object and a SharedAccessSignatureUpdated object as a result, indicating hat the client needs to reestablish the transport connection when ready.]*/
      done(null, new results.SharedAccessSignatureUpdated(true));
    }
  }.bind(this));
};

/**
 * @method          module:azure-iot-device-mqtt.Mqtt#setOptions
 * @description     This methods sets the MQTT specific options of the transport.
 *
 * @param {object}        options   Options to set.  Currently for MQTT these are the x509 cert, key, and optional passphrase properties. (All strings)
 * @param {Function}      done      The callback to be invoked when `setOptions` completes.
 */

Mqtt.prototype.setOptions = function (options, done) {
  /*Codes_SRS_NODE_DEVICE_MQTT_16_011: [The `setOptions` method shall throw a `ReferenceError` if the `options` argument is falsy]*/
  if (!options) throw new ReferenceError('The options parameter can not be \'' + options + '\'');
  /*Codes_SRS_NODE_DEVICE_MQTT_16_015: [The `setOptions` method shall throw an `ArgumentError` if the `cert` property is populated but the device uses symmetric key authentication.]*/
  if (this._config.sharedAccessSignature && options.cert) throw new errors.ArgumentError('Cannot set x509 options on a device that uses symmetric key authentication.');

  /*Codes_SRS_NODE_DEVICE_MQTT_16_012: [The `setOptions` method shall update the existing configuration of the MQTT transport with the content of the `options` object.]*/
  this._config.x509 = {
    cert: options.cert,
    key: options.key,
    passphrase: options.passphrase
  };

  /*Codes_SRS_NODE_DEVICE_MQTT_16_013: [If a `done` callback function is passed as a argument, the `setOptions` method shall call it when finished with no arguments.]*/
  /*Codes_SRS_NODE_DEVICE_MQTT_16_014: [The `setOptions` method shall not throw if the `done` argument is not passed.]*/
  if (done) done(null);
};

/**
 * @method          module:azure-iot-device-mqtt.Mqtt#sendTwinRequest
 * @description     Send a device-twin specific messager to the IoT Hub instance
 *
 * @param {String}        method    name of the method to invoke ('PUSH', 'PATCH', etc)
 * @param {String}        resource  name of the resource to act on (e.g. '/properties/reported/') with beginning and ending slashes
 * @param {Object}        properties  object containing name value pairs for request properties (e.g. { 'rid' : 10, 'index' : 17 })
 * @param {String}        body  body of request
 * @param {Function}      done  the callback to be invoked when this function completes.
 *
 * @throws {ReferenceError}   One of the required parameters is falsy
 * @throws {ArgumentError}  One of the parameters is an incorrect type
 */
Mqtt.prototype.sendTwinRequest = function(method, resource, properties, body, done) {

  /* Codes_SRS_NODE_DEVICE_MQTT_18_008: [** The `sendTwinRequest` method shall not throw `ReferenceError` if the `done` callback is falsy. **]** */

  /* Codes_SRS_NODE_DEVICE_MQTT_18_009: [** The `sendTwinRequest` method shall throw an `ReferenceError` if the `method` argument is falsy. **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_18_019: [** The `sendTwinRequest` method shall throw an `ReferenceError` if the `resource` argument is falsy. **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_18_011: [** The `sendTwinRequest` method shall throw an `ReferenceError` if the `properties` argument is falsy. **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_18_013: [** The `sendTwinRequest` method shall throw an `ReferenceError` if the `body` argument is falsy. **]** */
  if (!method || !resource || !properties ||  !body)
  {
    throw new ReferenceError('required parameter is missing');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_18_010: [** The `sendTwinRequest` method shall throw an `ArgumentError` if the `method` argument is not a string. **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_18_020: [** The `sendTwinRequest` method shall throw an `ArgumentError` if the `resource` argument is not a string. **]** */
  if (!util.isString(method) || !util.isString(resource))
  {
    throw new errors.ArgumentError('required string parameter is not a string');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_18_012: [** The `sendTwinRequest` method shall throw an `ArgumentError` if the `properties` argument is not a an object. **]** */
  if (!util.isObject(properties))  {
    throw new errors.ArgumentError('required properties parameter is not an object');
  }
  
  /* Codes_SRS_NODE_DEVICE_MQTT_18_022: [** The `propertyQuery` string shall be construced from the `properties` object. **]**   */
  var propString = '';
  Object.keys(properties).forEach(function(key) {
    /* Codes_SRS_NODE_DEVICE_MQTT_18_018: [** The `sendTwinRequest` method shall throw an `ArgumentError` if any members of the `properties` object fails to serialize to a string **]** */
    if (!util.isString(properties[key]) && !util.isNumber(properties[key]) && !util.isBoolean(properties[key])) {
      throw new errors.ArgumentError('required properties object has non-string properties');
    }
    
    /* Codes_SRS_NODE_DEVICE_MQTT_18_023: [** Each member of the `properties` object shall add another 'name=value&' pair to the `propertyQuery` string. **]**   */
    propString += (propString === '') ? '?' : '&';
    propString += key + '=' + properties[key];
  });

  /* Codes_SRS_NODE_DEVICE_MQTT_18_021: [** The topic name passed to the publish method shall be $iothub/twin/`method`/`resource`/?`propertyQuery` **]** */
  var topic = '$iothub/twin/' + method + resource + propString;

  /* Codes_SRS_NODE_DEVICE_MQTT_18_001: [** The `sendTwinRequest` method shall call the publish method on `MqttTransport`. **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_18_015: [** The `sendTwinRequest` shall publish the request with QOS=0, DUP=0, and Retain=0 **]** */
  this._mqtt.client.publish(topic, body.toString(), { qos: 0, retain: false }, function (err, puback) {
    if (done) {
      if (err) {
        /* Codes_SRS_NODE_DEVICE_MQTT_18_016: [** If an error occurs in the `sendTwinRequest` method, the `done` callback shall be called with the error as the first parameter. **]** */
        /* Codes_SRS_NODE_DEVICE_MQTT_18_024: [** If an error occurs, the `sendTwinRequest` shall use the MQTT `translateError` module to convert the mqtt-specific error to a transport agnostic error before passing it into the `done` callback. **]** */
        done(translateError(err));
      } else {
        /* Codes_SRS_NODE_DEVICE_MQTT_18_004: [** If a `done` callback is passed as an argument, The `sendTwinRequest` method shall call `done` after the body has been published. **]** */
        /* Codes_SRS_NODE_DEVICE_MQTT_18_017: [** If the `sendTwinRequest` method is successful, the first parameter to the `done` callback shall be null and the second parameter shall be a MessageEnqueued object. **]** */
        done(null, new results.MessageEnqueued(puback));
      }
    }
  }.bind(this));
};

function validateResponse(response) {
  if(!response) {
    throw new Error('Parameter \'response\' is falsy');
  }
  if(!(response.requestId)) {
    throw new Error('Parameter \'response.requestId\' is falsy');
  }
  if(typeof(response.requestId) === 'string' && response.requestId.length === 0) {
    throw new Error('Parameter \'response.requestId\' is an empty string');
  }
  if(typeof(response.requestId) !== 'string') {
    throw new Error('Parameter \'response.requestId\' is not a string.');
  }
  if(!(response.status)) {
    throw new Error('Parameter \'response.status\' is falsy');
  }
  if(typeof(response.status) !== 'number') {
    throw new Error('Parameter \'response.status\' is not a number');
  }
}

/**
 * @method            module:azure-iot-device-mqtt.Mqtt.Mqtt#sendMethodResponse
 * @description       Sends the response for a device method call to the service.
 *
 * @param {Object}   response     This is the `response` object that was
 *                                produced by the device client object when a
 *                                C2D device method call was received.
 * @param {Function} done         The callback to be invoked when the response
 *                                has been sent to the service.
 *
 * @throws {Error}                If the `response` parameter is falsy or does
 *                                not have the expected shape.
 */
Mqtt.prototype.sendMethodResponse = function(response, done) {
  // Codes_SRS_NODE_DEVICE_MQTT_13_001: [ sendMethodResponse shall throw an Error if response is falsy or does not conform to the shape defined by DeviceMethodResponse. ]
  validateResponse(response);

  // Codes_SRS_NODE_DEVICE_MQTT_13_002: [ sendMethodResponse shall build an MQTT topic name in the format: $iothub/methods/res/<STATUS>/?$rid=<REQUEST ID>&<PROPERTIES> where <STATUS> is response.status. ]
  // Codes_SRS_NODE_DEVICE_MQTT_13_003: [ sendMethodResponse shall build an MQTT topic name in the format: $iothub/methods/res/<STATUS>/?$rid=<REQUEST ID>&<PROPERTIES> where <REQUEST ID> is response.requestId. ]
  // Codes_SRS_NODE_DEVICE_MQTT_13_004: [ sendMethodResponse shall build an MQTT topic name in the format: $iothub/methods/res/<STATUS>/?$rid=<REQUEST ID>&<PROPERTIES> where <PROPERTIES> is URL encoded. ]

  var topicName = util.format(
    TOPIC_RESPONSE_PUBLISH_FORMAT,
    'methods',
    response.status,
    response.requestId
  );

  debug('sending response using topic: ' + topicName);
  // publish the response message
  this._mqtt.client.publish(topicName, JSON.stringify(response.payload), { qos: 0, retain: false }, function(err) {
    if(!!done && typeof(done) === 'function') {
      // Codes_SRS_NODE_DEVICE_MQTT_13_006: [ If the MQTT publish fails then an error shall be returned via the done callback's first parameter. ]
      // Codes_SRS_NODE_DEVICE_MQTT_13_007: [ If the MQTT publish is successful then the done callback shall be invoked passing null for the first parameter. ]
      done(!!err ? translateError(err) : null);
    }
  });
};

/**
 * @method          module:azure-iot-device-mqtt.Mqtt#getTwinReceiver
 * @description     Get a receiver object that handles C2D device-twin traffic
 *
 * @param {Function}  done      the callback to be invoked when this function completes.
 *
 * @throws {ReferenceError}   One of the required parameters is falsy
 */
Mqtt.prototype.getTwinReceiver = function(done) {
  /* Codes_SRS_NODE_DEVICE_MQTT_18_014: [** The `getTwinReceiver` method shall throw an `ReferenceError` if done is falsy **]**  */
  if (!done) {
    throw new ReferenceError('required parameter is missing');
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_18_003: [** If a twin receiver for this endpoint doesn't exist, the `getTwinReceiver` method should create a new `MqttTwinReceiver` object. **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_18_002: [** If a twin receiver for this endpoint has already been created, the `getTwinReceiver` method should not create a new `MqttTwinReceiver` object. **]** */
  if (!this._twinReceiver) {
    this._twinReceiver = new MqttTwinReceiver(this._mqtt.client);
  }

  /* Codes_SRS_NODE_DEVICE_MQTT_18_005: [** The `getTwinReceiver` method shall call the `done` method after it completes **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_18_006: [** If a twin receiver for this endpoint did not previously exist, the `getTwinReceiver` method should return the a new `MqttTwinReceiver` object as the second parameter of the `done` function with null as the first parameter. **]** */
  /* Codes_SRS_NODE_DEVICE_MQTT_18_007: [** If a twin receiver for this endpoint previously existed, the `getTwinReceiver` method should return the preexisting `MqttTwinReceiver` object as the second parameter of the `done` function with null as the first parameter. **]** */
  done(null, this._twinReceiver);
  
};



module.exports = Mqtt;
