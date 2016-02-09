// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');
var azureStorage = require('azure-storage');

var iothubConnectionString = '[IoT Connection String]';
var storageConnectionString = '[Storage Connection String]';

var registry = iothub.Registry.fromConnectionString(iothubConnectionString);
var blobSvc = azureStorage.createBlobService(storageConnectionString);

var startDate = new Date();
var expiryDate = new Date(startDate);
expiryDate.setMinutes(startDate.getMinutes() + 100);
startDate.setMinutes(startDate.getMinutes() - 100);

var inputSharedAccessPolicy = {
  AccessPolicy: {
    Permissions: 'rl',
    Start: startDate,
    Expiry: expiryDate
  },
};

var outputSharedAccessPolicy = {
  AccessPolicy: {
    Permissions: 'rwd',
    Start: startDate,
    Expiry: expiryDate
  },
};

var inputContainerName = 'importcontainer';
var outputContainerName = 'exportcontainer';
var deviceFile = 'devices.txt';

blobSvc.createContainerIfNotExists(inputContainerName, function (error) {
    if(error) {
        console.error('Could not create input container: ' + error.message);
    } else {
        var inputSasToken = blobSvc.generateSharedAccessSignature(inputContainerName, null, inputSharedAccessPolicy);
        var inputSasUrl = blobSvc.getUrl(inputContainerName, null, inputSasToken);
        blobSvc.createBlockBlobFromLocalFile(inputContainerName, deviceFile, deviceFile, function (error) {
            if (error) {
                console.error('Could not create devices.txt: ' + error.message);
            } else {
                blobSvc.createContainerIfNotExists(outputContainerName, function (error) {
                   if (error) {
                       console.error('Could not create output container: ' + error.message);
                   } else {
                       var outputSasToken = blobSvc.generateSharedAccessSignature(outputContainerName, null, outputSharedAccessPolicy);
                       var outputSasUrl = blobSvc.getUrl(outputContainerName, null, outputSasToken);
                    
                    /**
                     * There can only be one active job at a time, therefore, you can uncomment the export section and comment the import 
                     * session or vice versa depending on what part of the code you would like to test.
                     */
                       
                    //    registry.exportDevicesToBlob(outputSasUrl, false, function (error, result) {
                    //        if (error) {
                    //            console.error('Could not create export job: ' + error.message);
                    //        } else {
                    //            console.log('--------------\r\nDevices Export Job Identifier:--------------\r\n' + result);
                    //            var jobId = JSON.parse(result).jobId;
                    //            var interval = setInterval(function () {
                    //                registry.getJob(jobId, function (error, result) {
                    //                   if (error) {
                    //                       console.error('Could not get job status: ' + error.message + ' : ' + error.responseBody);
                    //                   } else {
                    //                       console.log('--------------\r\njob ' + jobId + ' status:\r\n--------------\r\n' + result);
                    //                       var status = JSON.parse(result).status;
                    //                       if (status === "completed") {
                    //                           clearInterval(interval);
                    //                       }
                    //                   }
                    //               });
                    //            }, 500);
                    //        } 
                    //    });
                       
                       
                       registry.importDevicesFromBlob(inputSasUrl, outputSasUrl, function (error, result) {
                          if (error) {
                              console.error('Could not create import devices: ' + error.message + ' : ' + error.responseBody);
                          } else {
                              console.log('--------------\r\nDevices Import Job Identifier:--------------\r\n' + result);
                              var jobId = JSON.parse(result).jobId;
                              var interval = setInterval(function () {
                                  /**
                                   * Uncomment this code to test cancelling a job.
                                   */
                                //   registry.cancelJob(jobId, function (error, result) {
                                //       if (error) {
                                //           console.error('Could not cancel job: ' + error.message + ' : ' + error.responseBody);
                                //       } else {
                                //           console.log('--------------\r\njob ' + jobId + ' cancelled:\r\n--------------\r\n' + result);
                                //           clearInterval(interval);                                          
                                //       }
                                //   });
                                  
                                  registry.getJob(jobId, function (error, result) {
                                      if (error) {
                                          console.error('Could not get job status: ' + error.message + ' : ' + error.responseBody);
                                      } else {
                                          console.log('--------------\r\njob ' + jobId + ' status:\r\n--------------\r\n' + result);
                                          var status = JSON.parse(result).status;
                                          if (status === "completed") {
                                              clearInterval(interval);
                                          }
                                      }
                                  });
                              }, 500);
                          }
                       });
                       
                       registry.listJobs(function(error, result) {
                           if (error) {
                              console.error('Could not list jobs: ' + error.message + ' : ' + error.responseBody);
                          } else {
                              console.log('Job list:\r\n----------\r\n' + result);
                          }
                       });          
                   }
                });
            }
        });
    }
});