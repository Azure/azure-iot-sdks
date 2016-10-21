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
var printErrorAndExit = require('./common.js').printErrorAndExit;
var printSuccess = require('./common.js').printSuccess;
var configLoc = require('./common.js').configLoc;

program
  .description('Terminate a temporary session on your IoT hub')
  .parse(process.argv);

var loc = configLoc();
var path = path.join(loc.dir, loc.file);

try {
  fs.unlinkSync(path);
  printSuccess('Session successfully terminated.');
  printSuccess('Removed session file: ' + path);
}
catch (err) {
  if (err.code === 'ENOENT') {
    printErrorAndExit('No session information found.');
  } else {
    throw err;
  }
}