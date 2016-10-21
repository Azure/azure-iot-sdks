#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

// external dependencies
var program = require('commander');
var colorsTmpl = require('colors-tmpl');
var prettyjson = require('prettyjson');

// local dependencies
var inputError = require('./common.js').inputError;
var serviceError = require('./common.js').serviceError;
var getSas = require('./common.js').getSas;

// Azure Event Hubs dependencies
var ServiceClient = require('azure-iothub').Client;

program
  .description('Monitor file upload notifications emitted by devices')
  .option('-l, --login <connectionString>', 'use the connection string provided as argument to use to authenticate with your IoT hub')
  .option('-r, --raw', 'use this flag to return raw output instead of pretty-printed output')
  .parse(process.argv);

var sas = getSas(program.login);
var client = ServiceClient.fromSharedAccessSignature(sas.toString());
client.open(function (err) {
  if (err) {
    inputError('Could not open the connection to the service: ' + err.message);
  } else {
    client.getFileNotificationReceiver(function (err, receiver) {
      if (err) serviceError(err);
      if (!program.raw) {
        console.log(colorsTmpl('\n{yellow}Waiting for file notifications...{/yellow} (Ctrl-C to quit)'));
      }

      receiver.on('errorReceived', function (err) { serviceError(err); });
      receiver.on('message', function (fileNotification) {
        var notif = JSON.parse(fileNotification.data.toString());
        var rendered = program.raw ?
          JSON.stringify(notif) :
          prettyjson.render(notif) + '\n----------------------------\n';

        console.log(rendered);
      });
    });
  }
});