// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var runTests = require('./_client_test.js');

function getPropertyValue(connectionString, propertyName) {
  var propertyValue = null;
  
  if (connectionString) {  
    var properties = connectionString.split(";");
    for(var i = 0; i < properties.length; i++) {
      var equalPosition = properties[i].indexOf("=");
      
      if (equalPosition > 0 && equalPosition < (properties[i].length - 1)) {
        var name = properties[i].substring(0, equalPosition);
        var value = properties[i].substring(equalPosition + 1, properties[i].length);
      
        if (name === propertyName) {
              propertyValue = value;
              break;
        } 
      }
    }
  }
  
  return propertyValue;
}

var host = getPropertyValue(process.env.IOTHUB_CONNECTION_STRING, "HostName");
var deviceId = process.env.IOTHUB_DEVICE_ID;
var key = process.env.IOTHUB_DEVICE_KEY;

var connectionString = 'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=' + key;
var badConnStrings = [
  'HostName=bad;DeviceId=' + deviceId + ';SharedAccessKey=' + key,
  'HostName=' + host + ';DeviceId=bad;SharedAccessKey=' + key,
  'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=bad;'
];

describe('Over real HTTPS', function () {
  this.timeout(15000);
  runTests(null, connectionString, badConnStrings);
});
