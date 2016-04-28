// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');
var queries = require('./query_examples.js');

// The connection string used here must have 'registry read' permissions to be able to use the query APIs.
var connectionString = '[IoT Hub Connection String]';

// The Registry object has APIs to edit the list of devices registered with your IoT Hub service.
var registry = iothub.Registry.fromConnectionString(connectionString);

// Create a function that will be used to process the results of device queries.
var printResults = function(queryDescription) {
  return function (err, devices) {
    if (err) {
      console.log('Oops: an error happened: ' + err.message + 'when trying to: ' + queryDescription);
    } else {
      console.log('-----');
      console.log(queryDescription + ': Found ' + devices.length + ' devices:');
      if (devices) {
        devices.forEach(function(device) {
          console.log(device.deviceId);
          console.log(JSON.stringify(device.serviceProperties.tags));
          console.log(JSON.stringify(device.serviceProperties.properties, null, 2));
        });
      } else {
          console.log('No device found matching those tags');
      }
      console.log('-----');
    }
  };
};

// Querying for device using only tags just requires an array and a maximum number of devices that can be returned. 
registry.queryDevicesByTags(['bacon'], 100, printResults('Query all devices with the tag \'bacon\''));

// The following three requests are defined in query_examples.js. 
registry.queryDevices(queries.filterQuery, printResults('Query all devices where CustomerId is 123456'));
registry.queryDevices(queries.sortQuery, printResults('Query all devices and sort them by QoS level ascending'));

registry.queryDevices(queries.aggregateQuery, function(err, result) {
  console.log('Compute the sum of the weight of all devices grouped by CustomerId');
  if(err) {
    console.error(err.message);
  } else {
    console.log(JSON.stringify(result, null, 2));
  }
});