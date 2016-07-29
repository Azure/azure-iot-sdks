// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var deviceSdk = require('azure-iot-device');

function createDeviceClient(deviceTransport, provisionedDevice) {
  var deviceClient;
  if (provisionedDevice.hasOwnProperty('primaryKey')) {
    deviceClient = deviceSdk.Client.fromConnectionString(provisionedDevice.connectionString, deviceTransport);
  } else if (provisionedDevice.hasOwnProperty('certificate')) {
    deviceClient = deviceSdk.Client.fromConnectionString(provisionedDevice.connectionString, deviceTransport);
    var options = {
      cert: provisionedDevice.certificate,
      key: provisionedDevice.clientKey,
    };
    deviceClient.setOptions(options);
  } else {
    deviceClient = deviceSdk.Client.fromSharedAccessSignature(provisionedDevice.connectionString, deviceTransport);
  }
  return deviceClient;
}

function closeDeviceServiceClients(deviceClient, serviceClient, done) {
  var serviceErr = null;
  var deviceErr = null;
  serviceClient.close(function (err) {
    serviceErr = err || deviceErr;
    serviceClient = null;
    if (serviceErr || !deviceClient) {
      done(serviceErr);
    }
  });
  deviceClient.close(function (err) {
    deviceErr = err || serviceErr;
    deviceClient = null;
    if (deviceErr || !serviceClient) {
      done(deviceErr);
    }
  });
}

function closeDeviceEventHubClients(deviceClient, eventHubClient, done) {
  var eventHubErr = null;
  var deviceErr = null;
  eventHubClient.close().then(function () {
    eventHubErr = deviceErr;
    eventHubClient = null;
    if (!deviceClient) {
      done(eventHubErr);
    }
  }).catch(function (err) {
    eventHubErr = err;
    eventHubClient = null;
    if (!deviceClient) {
      done(eventHubErr);
    }
  });
  deviceClient.close(function (err) {
    deviceErr = err || eventHubErr;
    deviceClient = null;
    if (deviceErr || !eventHubClient) {
      done(deviceErr);
    }
  });
}

module.exports = {
  createDeviceClient: createDeviceClient,
  closeDeviceServiceClients: closeDeviceServiceClients,
  closeDeviceEventHubClients: closeDeviceEventHubClients
};
