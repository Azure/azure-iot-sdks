// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');

var connectionString = '[IoT Hub Connection String]';
var deviceId = '[Device Id]';

var registry = iothub.Registry.fromConnectionString(connectionString);

registry.get(deviceId, function(err, deviceInfo) {
  if(err) {
    console.error('Could not get device with ID: ' + deviceId);
  } else {
    var newProperties = deviceInfo.serviceProperties;
    newProperties.properties.sampleProperty = 'sampleValue';
    registry.setServiceProperties(deviceId, newProperties, function (err, result) {
      if (err) {
        console.error('Error setting properties for ' + deviceId + ': ' + err.constructor.name + err.message);
      } else {
        console.log('New service properties for ' + deviceId + ': ');
        console.log(JSON.stringify(result, null, 2));
      }
    });
  }
});