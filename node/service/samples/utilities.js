// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var fs = require('fs');

// This is a file that contains custom device description, tags, etc.
var devicesFileName = 'sampledevices.json';

// This is the output file that will contain all newly created device connection strings.
var deviceCredentialsFile = 'devicecreds.txt';

var loadDevicesData = function (done) {
  fs.readFile(devicesFileName, function(err, data) {
    if (err) {
      done(err);
    }
    done(null, JSON.parse(data));
  });
};

module.exports = {
  deviceCredentialsFile: deviceCredentialsFile,
  loadDevicesData : loadDevicesData
};