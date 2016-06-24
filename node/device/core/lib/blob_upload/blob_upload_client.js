// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var BlobUploadResult = require('./blob_upload_result.js');
var DefaultBlobUploader = require('./blob_uploader.js');
var DefaultFileUploadApi = require('./file_upload_api.js');
var errors = require('./blob_upload_errors.js');

function BlobUploadClient (config, fileUploadApi, blobUploader) {
  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_001: [`BlobUploadClient` shall throw a `ReferenceError` if `config` is falsy.]*/
  if(!config) throw new ReferenceError('config cannot be \'' + config + '\'');
  this._config = config;

  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_002: [If specified, `BlobUploadClient` shall use the `fileUploadApi` passed as a parameter instead of the default one.]*/
  this._fileUploadApi = fileUploadApi ? fileUploadApi : new DefaultFileUploadApi(this._config.deviceId, this._config.host); 

  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_003: [If specified, `BlobUploadClient` shall use the `blobUploader` passed as a parameter instead of the default one.]*/
  this._blobUploader = blobUploader ? blobUploader : new DefaultBlobUploader();
}

BlobUploadClient.prototype.updateSharedAccessSignature = function (sharedAccessSignature) {
  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_011: [`updateSharedAccessSignature` shall update the value used by the `BlobUploadClient` instance to the value passed as an argument.]*/
  this._config.sharedAccessSignature = sharedAccessSignature;
};

BlobUploadClient.prototype.uploadToBlob = function(blobName, stream, streamLength, done) {
  var self = this;
  /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_004: [`uploadToBlob` shall obtain a blob SAS token using the IoT Hub service file upload API endpoint.]*/
  var auth = self._config.x509 ? self._config.x509 : self._config.sharedAccessSignature;
  self._fileUploadApi.getBlobSharedAccessSignature(blobName, auth, function (err, uploadParams) {
    if (err) {
      /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_005: [`uploadToBlob` shall call the `done` callback with a `BlobSasError` parameter if retrieving the SAS token fails.]*/
      var error = new errors.BlobSasError('Could not obtain blob shared access signature.');
      error.innerError = err;
      done(error);
    } else {
      /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_006: [`uploadToBlob` shall upload the stream to the specified blob using its BlobUploader instance.]*/
      self._blobUploader.uploadToBlob(uploadParams, stream, streamLength, function (err, body, result) {
        var uploadResult = BlobUploadResult.fromAzureStorageCallbackArgs(err, body, result);
        /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_008: [`uploadToBlob` shall notify the result of a blob upload to the IoT Hub service using the file upload API endpoint.]*/
        self._fileUploadApi.notifyUploadComplete(uploadParams.correlationId, auth, uploadResult, function (err) {
          if(err) {
            /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_009: [`uploadToBlob` shall call the `done` callback with a `BlobUploadNotificationError` if notifying the IoT Hub instance of the transfer outcome fails.]*/
            var error = new errors.BlobUploadNotificationError('Could not notify the IoT Hub of the file upload completion.');
            error.innerError = err;
            done (error);
          } else {
            /*Codes_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_010: [`uploadToBlob` shall call the `done` callback with no arguments if IoT Hub was successfully notified of the blob upload outcome, regardless of the success state of the transfer itself.]*/
            done ();
          }
        });
      });
    }
  });
};

module.exports = BlobUploadClient;