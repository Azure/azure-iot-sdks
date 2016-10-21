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

// Azure Event Hubs dependencies
var EventHubsClient = require('azure-event-hubs').Client;

program
  .description('Monitor messages sent by devices to the IoT hub')
  .option('-l, --login <connectionString>', 'use the connection string provided as argument to use to authenticate with your IoT hub')
  .option('-r, --raw', 'use this flag to return raw output instead of pretty-printed output')
  .option('-cg, --consumer-group <consumer-group>', 'Specify the consumer group to use when connecting to Event Hubs partitions')
  .option('-st, --start-time <start-time>', 'Specify the time that should be used as a starting point to read messages in the partitions (number of milliseconds since epoch or ISO-8601 string)')
  .parse(process.argv);

if(!program.login) inputError('You must provide a connection string using the --login argument.');

var deviceId = program.args[0];
var connectionString = program.login;

if(!program.raw) {
  if (deviceId) {
    console.log(colorsTmpl('\n{grey}Monitoring events from device {green}' + deviceId + '{/green}...{/grey}'));
  } else {
    console.log(colorsTmpl('\n{grey}Monitoring events from all devices...{/grey}'));
  }
}

var consumerGroup = program.consumerGroup || '$Default';
var startTime = program.startTime ? new Date(program.startTime) : Date.now();

var ehClient = EventHubsClient.fromConnectionString(connectionString);
ehClient.open()
        .then(ehClient.getPartitionIds.bind(ehClient))
        .then(function (partitionIds) {
          return partitionIds.map(function (partitionId) {
            return ehClient.createReceiver(consumerGroup, partitionId, { 'startAfterTime' : startTime}).then(function(receiver) {
              receiver.on('errorReceived', function (error) {
                serviceError(error.message);
              });
              receiver.on('message', function (eventData) {
                if (program.raw) {
                  console.log(JSON.stringify(eventData));
                } else {
                  if (deviceId) {
                    if (eventData.systemProperties['iothub-connection-device-id'] === deviceId) {
                      console.log((typeof eventData.body === 'string') ? eventData.body : JSON.stringify(eventData.body, null, 2));
                      console.log('-------------------');
                    }
                    // ignore message otherwise.
                  } else {
                    console.log(colorsTmpl('{bold}From: {green}' + eventData.systemProperties['iothub-connection-device-id'] + '{/green}{/bold}'));
                    console.log((typeof eventData.body === 'string') ? eventData.body : JSON.stringify(eventData.body, null, 2));
                    console.log('-------------------');
                  }
                }
              });
            });
          });
        })
        .catch(function (error) {
          serviceError(error.message);
        });