#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var program = require('commander');
var azureStorage = require('azure-storage');
var inputError = require('./common.js').inputError;
var serviceError = require('./common.js').serviceError;
var printSuccess = require('./common.js').printSuccess;
var getSas = require('./common.js').getSas;
var Registry = require('azure-iothub').Registry;

program
  .description('Export devices in bulk from the device identity registry. The list of devices gets exported to a blob and then downloaded to a local file.')
  .usage('[options] --storage <storage-connection-string> --output <file-path>')
  .option('-l, --login <connection-string>', 'connection string to use to authenticate with your IoT Hub instance')
  .option('-o, --output <file-path>', 'output file containing device descriptions')
  .option('-s, --storage <storage-connection-string>', 'Azure blob storage connection string to be used during bulk export')
  .option('-x, --exclude-keys', 'exclude symmetric keys in output (keys are included by default)')
  .parse(process.argv);

if(!program.storage) inputError('A storage connection string with permissions to create a blob is required');
if(!program.output) inputError('An output file path is required.');

var outputFile = program.output;
var storageConnectionString = program.storage;
var excludeKeys = program.excludeKeys || false;
var sas = getSas(program.login);

var registry = Registry.fromSharedAccessSignature(sas);
var blobSvc = azureStorage.createBlobService(storageConnectionString);

var startDate = new Date();
var expiryDate = new Date(startDate);
expiryDate.setMinutes(startDate.getMinutes() + 100);
startDate.setMinutes(startDate.getMinutes() - 100);

var outputSharedAccessPolicy = {
  AccessPolicy: {
    Permissions: 'rwd',
    Start: startDate,
    Expiry: expiryDate
  },
};

var outputContainerName = 'exportcontainer';
var deviceFile = 'devices.txt';

blobSvc.createContainerIfNotExists(outputContainerName, function (error) {
  if (error) {
    serviceError('Could not create output container: ' + error.message);
  } else {
    var outputSasToken = blobSvc.generateSharedAccessSignature(outputContainerName, null, outputSharedAccessPolicy);
    var outputSasUrl = blobSvc.getUrl(outputContainerName, null, outputSasToken);
    registry.exportDevicesToBlob(outputSasUrl, excludeKeys, function (error, result) {
      if (error) {
        serviceError('Could not start device export job: ' + error.message);
      } else {
        var jobId = JSON.parse(result).jobId;
        var interval = setInterval(function () {
          registry.getJob(jobId, function (error, result) {
            if (error) {
              serviceError('Could not get export job status: ' + error.message + ' : ' + error.responseBody);
            } else {
              var status = JSON.parse(result).status;
              printSuccess('Job Id: '+ jobId + ': ' + status);
              if (status === "completed") {
                clearInterval(interval);
                blobSvc.getBlobToLocalFile(outputContainerName, deviceFile, outputFile, function(err) {
                  if(err) {
                    serviceError('Could not download export to local file: ' + err.message);
                  } else {
                    printSuccess('Exported devices to: ' + outputFile);
                    blobSvc.deleteBlob(outputContainerName, deviceFile, function(err){
                      if(err) {
                        serviceError('Could not delete temporary export blob');
                      } else {
                        printSuccess('Temporary export blob deleted.');
                      }
                    });
                  }
                });
              }
            }
          });
        }, 1000);
      }
    });
  }
});