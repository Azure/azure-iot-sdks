// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/**
 * The `Registry` provides a means for clients to use the Azure IoT Hub device
 * identity service, known as the *Registry*.  A consumer can add, remove,
 * update, or read device metadata from the Registry through the transport
 * supplied by the caller (e.g., [Https]{@linkcode module:adapters/https.Https}).
 *
 * @example
 * 'use strict';
 * var Registry = require('./src/iothub').service.Registry;
 * var Device = require('./src/iothub').service.Device;
 * var Https = require('./src/iothub_https').Https;
 *
 * function print(err, res) {
 *   if (err) console.log(err.toString());
 *   if (res) console.log(res.statusCode + ' ' + res.statusMessage);
 * }
 *
 * var connString = ‘Connection String’
 * var registry = new Registry(connString, new Https());
 *
 * registry.get('deviceId', function (err, res, dev) {
 *   print(err, res);
 *   if (!err) console.log(dev.id);
 * });
 *
 * @module service/registry
 */

'use strict';

var Device = require('./device.js');
var endpoint = require('azure-iot-common').endpoint;

var HOSTNAME_LEN = "HostName=".length;
var SAKN_LEN = "SharedAccessKeyName=".length;
var SAK_LEN = "SharedAccessKey=".length;

function parseConnString(connString)
{
  var config = {
    host: '',
    hubName: '',
    keyName: '',
    key: ''
  };
  var configArray = connString.split(';');
  configArray.forEach(function(element) {
    var pos = element.indexOf("HostName=");
    if (pos >= 0) {
      config.host = element.substring(pos+HOSTNAME_LEN);
      // Look for the first period
      var periodSeparator = config.host.indexOf('.');
      config.hubName = config.host.substring(0, periodSeparator);
    }
    else if ( (pos = element.indexOf("SharedAccessKeyName=") ) >= 0) {
      config.keyName = element.substring(pos+SAKN_LEN);
    }
    else if ( (pos = element.indexOf("SharedAccessKey=") ) >= 0) {
      config.key = element.substring(pos+SAK_LEN);
    }
  });
  return config;
}

/**
 * @class Registry
 * @classdesc Constructs a {@linkcode Registry} object with the given IoT
 *            registry connection string and `transport` object.
 * @param {String}  connString    The IoT registry connection string
 * @param {Object}  transport     An object that implements the interface
 *                                expected of a transport object. See the
 *                                file **adapters/devdoc/https_requirements.docm**
 *                                for information on what the transport
 *                                object should look like. The {@linkcode Https}
 *                                class is an implementation that uses HTTP as
 *                                the communication protocol.
 */
/*Codes_SRS_NODE_IOTHUB_REGISTRY_05_001: [The Registry constructor shall accept a transport object]*/
function Registry(connString, transport) {
  this.config = parseConnString(connString);
  this.transport = transport;
}

/**
 * Creates a device with the given ID.
 * @param {Object}   deviceInfo   The object must include a `deviceId` property
 *                                with a valid device identifier.
 * @param {Function} done         The callback to be invoked when `create`
 *                                completes execution.
 */
Registry.prototype.create = function (deviceInfo, done) {
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_001: [The create method shall throw a ReferenceError if the supplied deviceId is not valid.] */
  if (!deviceInfo.deviceId) {
    throw new ReferenceError('Invalid argument \'deviceId\'');
  }
  else {
    var path = endpoint.devicePath(deviceInfo.deviceId);
    this.transport.createDevice(path, deviceInfo, this.config, function (err, res, msg) {
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_002: [When the create method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a Device object representing the the new device information created from IoT Hub.]*/
      if (err) {
        deviceInfo = undefined;
      }
      else if (msg) {
        deviceInfo = new Device(msg.getData() );
      }
      done(err, res, deviceInfo);
    });
  }
};

/**
 * Updates an existing device with the given ID.
 * @param {Object}   deviceInfo   The object must include a `deviceId` property
 *                                with a valid device identifier of an existing
 *                                device.
 * @param {Function} done         The callback to be invoked when `update`
 *                                completes execution.
 */
Registry.prototype.update = function (deviceInfo, done) {
  /* Codes_SRS_NODE_IOTHUB_REGISTRY_07_003: [The update method shall throw a ReferenceError if the supplied deviceInfo.deviceId is not valid.]*/
  if (!deviceInfo.deviceId) {
    throw new ReferenceError('Invalid argument \'deviceId\'');
  }
  else {
    var path = endpoint.devicePath(deviceInfo.deviceId);
    this.transport.updateDevice(path, deviceInfo, this.config, function sendFromResponse(err, res, msg) {
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_004: [When the update method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a Device object representing the updated device information created from IoT Hub.]*/
      if (err) {
        deviceInfo = undefined;
      }
      else if (msg) {
        deviceInfo = new Device(msg.getData() );
      }
      done(err, res, deviceInfo);
    });
  }
};

/**
 * This method requests metadata (in the form of a Device object) about the
 * device with the given ID.
 * @param {String}   deviceId   A valid device identifier
 * @param {Function} done       The callback to be invoked when `get`
 *                              completes execution.
 */
Registry.prototype.get = function (deviceId, done) {
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_002: [The get method shall request metadata for the device (indicated by the id argument) from the IoT Hub registry service via the transport associated with the Registry instance.]*/
  var path = endpoint.devicePath(deviceId);
  this.transport.getDevice(path, this.config, function (err, res, msg) {
    /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_003: [When the get method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a Device object representing the device information returned from IoT Hub.]*/
    var dev;
    if (msg) {
      dev = new Device(msg.getData() );
    }
    done(err, res, dev);
  });
};

/**
 * This method requests metadata (in the form of a Device object) for the top
 * 1000 devices in the system.
 * @param {Function} done     The callback to be invoked when `list`
 *                            completes execution.
 */
Registry.prototype.list = function (done) {
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_005: [When the list method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a list of Device object in the form of JSON objects.]*/
  var path = endpoint.devicePath('');
  this.transport.listDevice(path, this.config, function (err, res, msg) {
    var devList = [];
    if (msg) {
      var jsonArray = JSON.parse(msg.getData() );
      jsonArray.forEach(function(jsonElement) {
        /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_006: [The returned JSON object will be converted to a list of Device objects.]*/
        var devItem = new Device(JSON.stringify(jsonElement) );
        devList.push(devItem);
      });
    }
    done(err, res, devList);
  });
};

/**
 * This method removes a device with the given ID.
 * @param {String}   deviceId   A valid device identifier
 * @param {Function} done       The callback to be invoked when `delete`
 *                              completes execution.
 */
Registry.prototype.delete = function (deviceId, done) {
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_007: [The delete method shall throw a ReferenceError if the supplied deviceId is not valid. When the delete method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback]*/
  var path = endpoint.devicePath(deviceId);
  this.transport.deleteDevice(path, this.config, function (err, res, msg) {
    done(err, res);
  });
};

module.exports = Registry;