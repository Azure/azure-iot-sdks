// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var anHourFromNow = require('azure-iot-common').authorization.anHourFromNow;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var ConnectionString = require('./connection_string.js');
var DefaultTransport = require('./registry_http.js');
var Device = require('./device.js');
var endpoint = require('azure-iot-common').endpoint;
var ServiceToken = require('azure-iot-common').authorization.ServiceToken;

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
 *                              {@link module:azure-iothub.Http} class is one
 *                              such implementation of the transport.
 */
/*Codes_SRS_NODE_IOTHUB_REGISTRY_05_001: [The Registry constructor shall accept a transport object]*/
function Registry(transport) {
  this._transport = transport;
}

Registry.fromConnectionString = function fromConnectionString(value, Transport) {
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_008: [The fromConnectionString method shall throw ReferenceError if the value argument is falsy.]*/
  if (!value) throw new ReferenceError('value is \'' + value + '\'');

  Transport = Transport || DefaultTransport;

  /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_009: [Otherwise, it shall derive and transform the needed parts from the connection string in order to create a new instance of the default transport (azure-iothub.Http).]*/
  var cn = ConnectionString.parse(value);
  var sas = new ServiceToken(cn.HostName, cn.SharedAccessKeyName, cn.SharedAccessKey, anHourFromNow());

  var config = {
    host: cn.HostName,
    hubName: cn.HostName.split('.', 1)[0],
    sharedAccessSignature: sas.toString()
  };

  /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_010: [The fromConnectionString method shall return a new instance of the Registry object, as by a call to new Registry(transport).]*/
  return new Registry(new Transport(config));
};

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
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_001: [The create method shall throw ArgumentError if the first argument does not contain a deviceId property.]*/
  if (!deviceInfo.deviceId) {
    throw new ArgumentError('The object \'deviceInfo\' is missing the property: deviceId');
  }
  else {
    var path = endpoint.devicePath(deviceInfo.deviceId);
    this._transport.createDevice(path, deviceInfo, function (err, body, response) {
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_002: [When the create method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a Device object representing the new device identity returned from the IoT hub.]*/
      if (err) {
        deviceInfo = null;
      }
      else if (body) {
        deviceInfo = new Device(body);
      }
      done(err, deviceInfo, response);
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
  /* Codes_SRS_NODE_IOTHUB_REGISTRY_07_003: [The update method shall throw ArgumentError if the first argument does not contain a deviceId property.]*/
  if (!deviceInfo.deviceId) {
    throw new ArgumentError('The object \'deviceInfo\' is missing the property: deviceId');
  }

  var path = endpoint.devicePath(deviceInfo.deviceId);
  this._transport.updateDevice(path, deviceInfo, function (err, body, response) {
    /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_004: [When the update method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a Device object representing the new device identity returned from the IoT hub.]*/
    if (err) {
      deviceInfo = undefined;
    }
    else if (body) {
      deviceInfo = new Device(body);
    }
    done(err, deviceInfo, response);
  });
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
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_006: [The get method shall throw ReferenceError if the supplied deviceId is falsy.]*/
  if (!deviceId) {
    throw new ReferenceError('deviceId is \'' + deviceId + '\'');
  }

  /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_002: [The get method shall request metadata for the device (indicated by the deviceId argument) from an IoT hub’s identity service via the transport associated with the Registry instance.]*/
  var path = endpoint.devicePath(deviceId);
  this._transport.getDevice(path, function (err, body, response) {
    /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_003: [When the get method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback, plus a Device object representing the device information returned from IoT Hub.]*/
    var dev;
    if (body) {
      dev = new Device(body);
    }
    done(err, dev, response);
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
  var path = endpoint.devicePath('');
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_004: [The list method shall request information about devices from an IoT hub’s identity service via the transport associated with the Registry instance.]*/
  this._transport.listDevices(path, function (err, body, response) {
    var devList = [];
    if (body) {
      var jsonArray = JSON.parse(body);
      jsonArray.forEach(function (jsonElement) {
        /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_006: [The JSON array returned from the service shall be converted to a list of Device objects.]*/
        var devItem = new Device(JSON.stringify(jsonElement));
        devList.push(devItem);
      });
    }
    /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_005: [When the list method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and an array of Device objects representing up to 1000 devices from the IoT hub.]*/
    done(err, devList, response);
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
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_07_007: [The delete method shall throw ReferenceError if the supplied deviceId is falsy.]*/
  if (!deviceId) {
    throw new ReferenceError('deviceId is \'' + deviceId + '\'');
  }

  var path = endpoint.devicePath(deviceId);
  /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_007: [The delete method shall delete the given device from an IoT hub’s identity service via the transport associated with the Registry instance.]*/
  this._transport.deleteDevice(path, function (err, body, response) {
    /*Codes_SRS_NODE_IOTHUB_REGISTRY_05_005: [When the delete method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null).]*/
    done(err, null, response);
  });
};

module.exports = Registry;