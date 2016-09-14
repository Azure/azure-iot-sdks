// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Client = require('azure-iothub').Client;

var connectionString = '[Hub Connection String]';
var targetDevice = '[Device Id]';
var methodName = '[Method Name]';
var payload = '[Method Payload]';
var timeoutInSeconds = 10;

var client = Client.fromConnectionString(connectionString);

client.invokeMethod(targetDevice, methodName, payload, timeoutInSeconds, function (err, result) {
  if (err) {
    console.error('Failed to invoke method \'' + methodName + '\': ' + err.message);
  } else {
    console.log(methodName + ' on ' + targetDevice + ':');
    console.log(JSON.stringify(result, null, 2));
  }
});