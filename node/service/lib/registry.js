// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
 * @class           module:azure-iothub.Registry
 * @classdesc       Constructs a Registry object with the given IoT Hub
 *                  connection string and transport object. The Registry class
 *                  provides access to IoT Hub identity service.
 * @param {String}  connString  A connection string which encapsulates the
 *                              appropriate (read and/or write) Registry
 *                              permissions.
 * @param {Object}  transport   An object that implements the interface
 *                              expected of a transport object. See the file
 *                              **node/common/lib/devdoc/https_requirements.docm**
 *                              for information on what the transport object
 *                              should look like. The
 *                              {@link module:azure-iothub.Https} class is one
 *                              such implementation of the transport.
 */
/*Codes_SRS_NODE_IOTHUB_REGISTRY_05_001: [The Registry constructor shall accept a transport object]*/
function Registry(connString, transport) {
  this.config = parseConnString(connString);
  this.transport = transport;
}

/**
 * @method            module:azure-iothub.Registry#create
 * @description       Creates a new device identity on an IoT hub.
 * @param {Object}    deviceInfo  The object must include a `deviceId` property
 *                                with a valid device identifier.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with three
 *                                arguments: an Error object (can be null), a
 *                                transport-specific response object useful for
 *                                logging or debugging, and a
 *                                {@link module:azure-iothub.Device} object
 *                                representing the created device.
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
 * @method            module:azure-iothub.Registry#update
 * @description       Updates an existing device identity on an IoT hub with
 *                    the given device information.
 * @param {Object}    deviceInfo  The object must include a `deviceId` property
 *                                with a valid device identifier.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with three
 *                                arguments: an Error object (can be null), a
 *                                transport-specific response object useful for
 *                                logging or debugging, and a
 *                                {@link module:azure-iothub.Device} object
 *                                representing the updated device identity.
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
 * @method            module:azure-iothub.Registry#get
 * @description       Requests information about an existing device identity
 *                    on an IoT hub.
 * @param {String}    deviceId    The identifier of an existing device identity.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with three
 *                                arguments: an Error object (can be null), a
 *                                transport-specific response object useful for
 *                                logging or debugging, and a
 *                                {@link module:azure-iothub.Device} object
 *                                representing the returned device identity.
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
 * @method            module:azure-iothub.Registry#list
 * @description       Requests information about the first 1000 device
 *                    identities on an IoT hub.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with three
 *                                arguments: an Error object (can be null), a
 *                                transport-specific response object useful for
 *                                logging or debugging, and an array of
 *                                {@link module:azure-iothub.Device} objects
 *                                representing the listed device identities.
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
 * @method            module:azure-iothub.Registry#delete
 * @description       Removes an existing device identity from an IoT hub.
 * @param {String}    deviceId    The identifier of an existing device identity.
 * @param {Function}  done        The function to call when the operation is
 *                                complete. `done` will be called with two
 *                                arguments: an Error object (can be null) and
 *                                a transport-specific response object useful
 *                                for logging or debugging.
 */
Registry.prototype.delete = function (deviceId, done) {
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_007: [The delete method shall throw a ReferenceError if the supplied deviceId is not valid. When the delete method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback]*/
  var path = endpoint.devicePath(deviceId);
  this.transport.deleteDevice(path, this.config, function (err, res) {
    done(err, res);
  });
};

module.exports = Registry;