// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Client = require('azure-iothub').Client;

var connectionString = '[IoT Hub connection string]';

var client = Client.fromConnectionString(connectionString);

client.open(function (err) {
  if (err) {
    console.error('Could not connect: ' + err.message);
  } else {
    console.log('Client connected');

    client.getFileNotificationReceiver(function(err, receiver) {
      if(err) {
        console.error('Could not get file notification receiver: ' + err.message);
      } else {
        receiver.on('message', function(msg) {
          console.log('File uploaded: ');
          console.log(msg.data.toString());
          receiver.complete(msg, function(err) {
            if (err) {
              console.error('Could not complete the message: ' + err.message);
            } else {
              console.log('Message completed');
            }
          });
        });
      }
    });
  }
});