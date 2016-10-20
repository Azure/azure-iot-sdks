#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

// external dependencies
var program = require('commander');
var colorsTmpl = require('colors-tmpl');

// local dependencies
var inputError = require('./common.js').inputError;
var serviceError = require('./common.js').serviceError;
var getSas = require('./common.js').getSas;
var createMessageFromArgument = require('./common.js').createMessageFromArgument;

// Azure Event Hubs dependencies
var ServiceClient = require('azure-iothub').Client;

program
  .description('Send a message to device (cloud-to-device/C2D).')
  .option('-l, --login <connectionString>', 'use the connection string provided as argument to use to authenticate with your IoT hub')
  .option('-r, --raw', 'use this flag to return raw output instead of pretty-printed output')
  .option('-a, --ack <ack-type>', 'set the type of feedback that you would like to receive: none|positive|negative|full')
  .parse(process.argv);

if(!program.args[0]) inputError('You need to specify a device id.');
if(!program.args[1]) inputError('You need to specify a message.');

var deviceId = program.args[0];
var messageArg = program.args[1];
var ack = program.ack;

var sas = getSas(program.login);
var client = ServiceClient.fromSharedAccessSignature(sas.toString());
client.open(function (err) {
  if (err) {
    inputError('Could not open the connection to the service: ' + err.message);
  } else {
    var message = createMessageFromArgument(messageArg, ack);
    client.send(deviceId, message, function (err) {
      if (err) serviceError(err);
      if(program.raw) {
        console.log(message.messageId);
      } else {
        var successMessage = '{green}Message sent with id: {/green}' + message.messageId;
        if (program.ack) successMessage += '. {grey}Acknowledgement requested: ' + program.ack + '{/grey}';
        console.log(colorsTmpl(successMessage));
      }
      client.close(function(err) {
        if(err) serviceError(err);
      });
    });
  }
});