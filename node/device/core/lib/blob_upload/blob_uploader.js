// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ArgumentError = require('azure-iot-common').errors.ArgumentError;

function BlobUploader(storageApi) {
  if (storageApi) {
    /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_16_008: [`BlobUploader` should use the `storageApi` object to upload data if `storageApi` is truthy.]*/
    this.storageApi = storageApi;
  } else {
    /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_16_007: [`BlobUploader` should use the `azure-storage` package to upload data if `storageApi` is falsy.]*/
    this.storageApi = require('azure-storage');
  }
}

BlobUploader.prototype.uploadToBlob = function uploadToBlob (blobInfo, stream, streamLength, done) {
  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_16_001: [`uploadToBlob` shall throw a `ReferenceError` if `blobInfo` is falsy.]*/
  if (!blobInfo) throw new ReferenceError('blobInfo cannot be \'' + blobInfo + '\'');
  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_16_002: [`uploadToBlob` shall throw a `ReferenceError` if `stream` is falsy.]*/
  if (!stream) throw new ReferenceError('stream cannot be \'' + stream + '\'');
  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_16_003: [`uploadToBlob` shall throw a `ReferenceError` if `streamSize` is falsy.]*/
  if (!streamLength) throw new ReferenceError('streamLength cannot be \'' + streamLength + '\'');
  
  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_16_004: [`uploadToBlob` shall throw an `ArgumentError` if `blobInfo` is missing one or more of the following properties: `hostName`, `containerName`, `blobName`, `sasToken`).]*/
  if (!blobInfo.hostName || !blobInfo.containerName || !blobInfo.blobName || !blobInfo.sasToken) {
    throw new ArgumentError('Invalid upload parameters');
  }

  var blobService = this.storageApi.createBlobServiceWithSas(blobInfo.hostName, blobInfo.sasToken);
  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_16_005: [`uploadToBlob` shall call the `done` calback with the result of the storage api call.]*/
  blobService.createBlockBlobFromStream(blobInfo.containerName, blobInfo.blobName, stream, streamLength, done);
};

module.exports = BlobUploader;