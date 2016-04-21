// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');
var fs = require('fs');
var common = require('./common.js');

// This is the input file that should be in the same folder as this sample, and contain the connection string to the IoT Hub service.
var connectionString = '[IoT Hub Connection String]';
var hostName = iothub.ConnectionString.parse(connectionString).HostName;

common.loadDeviceData(function(err, deviceData) {
  // Step 1: create the registry object, that will be used to create devices
  var registry = iothub.Registry.fromConnectionString(connectionString);
  
  deviceData.forEach(function (deviceInfo) {
    // Step 2: create the new device using the deviceInfo structure that we just assembled. 
    registry.create(deviceInfo, function (err, result) {
      if (err) {
        console.error(err.message);
      } else {
        console.log('device created: ' + result.deviceId);
        // Step 3: the device creation has succeeded. Generate the connection string and append it to the file containing device credentials for use with other samples.
        var deviceConnStr = "HostName=" + hostName + ';DeviceId=' + result.deviceId + ';SharedAccessKey=' + result.authentication.symmetricKey.primaryKey;
        fs.appendFile(common.deviceCredentialsFile, deviceConnStr + '\r\n', function (err) {
          if (err) {
            console.error('Could not write to device credentials file');
          }
        });
      }
    });
  });
});