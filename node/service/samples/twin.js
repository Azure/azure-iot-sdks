// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

"use strict";

var Registry = require('azure-iothub').Registry;

var connectionString = "[IoT Hub Connection String]";
var deviceId = '[Device ID]';

var registry = Registry.fromConnectionString(connectionString);
registry.getTwin(deviceId, function(err, twin) {
  if (err) {
    console.error(err.message);
  } else {
    console.log(JSON.stringify(twin, null, 2));
    var twinPatch = {
      tags: {
        city: "Redmond"
      },
      properties: {
        desired: {
          telemetryInterval: 1000
        }
      }
    };

    // method 1: using the update method directly on the twin
    twin.update(twinPatch, function(err, twin) {
      if (err) {
        console.error(err.message);
      } else {
        console.log(JSON.stringify(twin, null, 2));
        // method 2: using the updateTwin method on the Registry object
        registry.updateTwin(twin.deviceId, { properties: { desired: { telemetryInterval: 2000 }}}, twin.etag, function(err, twin) {
          if (err) {
            console.error(err.message);
          } else {
            console.log(JSON.stringify(twin, null, 2));
          }
        });
      }
    });
  }
});