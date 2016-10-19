#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

// Native node modules
var fs = require('fs');
var path = require('path');

// External dependencies
var program = require('commander');

// Local dependencies
var inputError = require('./common.js').inputError;
var printSuccess = require('./common.js').printSuccess;
var configLoc = require('./common.js').configLoc;

// Azure IoT SDK dependencies
var ConnectionString = require('azure-iothub').ConnectionString;
var SharedAccessSignature = require('azure-iothub').SharedAccessSignature;

program
  .description('Create a temporary session on your IoT hub')
  .option('-d, --duration <duration-in-seconds>', 'time to keep the session open for (in seconds): if not specified, the default is one hour')
  .parse(process.argv);

if(!program.args[0]) inputError('You must specify a connection string.');

var connString = program.args[0];
var nowInSeconds = Math.floor(Date.now() / 1000);
var expiry = program.duration ? nowInSeconds + program.duration : nowInSeconds + 3600;
var cn = ConnectionString.parse(connString);
var sas = SharedAccessSignature.create(cn.HostName, cn.SharedAccessKeyName, cn.SharedAccessKey, expiry);
var loc = configLoc();

if (isNaN(new Date(expiry * 1000))) {
  inputError('Invalid duration.');
}

fs.mkdir(loc.dir, function () {
  var sessionFilePath = path.join(loc.dir, loc.file);
  fs.writeFile(sessionFilePath, sas.toString(), function (err) {
    if (err) inputError(err.toString());
    else {
      printSuccess('Session started, expires on ' + new Date(expiry * 1000).toString());
      printSuccess('Session file: ' + sessionFilePath);
    }
  });
});
