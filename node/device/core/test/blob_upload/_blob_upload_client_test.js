// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var assert = require('chai').assert;
var sinon = require('sinon');
var stream = require('stream');
var BlobUploadClient = require('../../lib/blob_upload').BlobUploadClient;
var errors = require('../../lib/blob_upload').errors;

var FakeFileUploadApi = function() {
  this.getBlobSharedAccessSignature = sinon.spy();
  this.notifyUploadComplete = sinon.spy();
};

var FakeBlobUploader = function() {
  this.uploadToBlob = sinon.spy();
};

var fakeConfig = {
  host: 'hub.host.com',
  hubName: 'hub',
  sharedAccessSignature: 'sas',
  deviceId: 'deviceId'
};

describe('BlobUploadClient', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_001: [`BlobUploadClient` shall throw a `ReferenceError` if `config` is falsy.]*/
    [undefined, null].forEach(function(config) {
      it('throws a ReferenceError if \'config\' is \'' + config + '\'', function() {
        assert.throws(function() {
          return new BlobUploadClient(config);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_002: [If specified, `BlobUploadClient` shall use the `fileUploadApi` passed as a parameter instead of the default one.]*/
    it('uses the fileUploadApi passed as a argument instead of the default one if specified', function() {
      var fakeFileUpload = new FakeFileUploadApi();
      var fakeBlobUploader = new FakeBlobUploader();

      var client = new BlobUploadClient(fakeConfig, fakeFileUpload, fakeBlobUploader);
      client.uploadToBlob('blobName', new stream.Readable(), 42, function() {});
      assert(fakeFileUpload.getBlobSharedAccessSignature.calledOnce);
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_003: [If specified, `BlobUploadClient` shall use the `blobUploader` passed as a parameter instead of the default one.]*/
  });
  
  describe('#updateSharedAccessSignature', function() {
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_011: [`updateSharedAccessSignature` shall update the value used by the `BlobUploadClient` instance to the value passed as an argument.]*/
    it('updates the shared access signature with the new one', function() {
      var newSas = 'newSas';
      var fakeStream = new stream.Readable();
      var fakeFileUpload = new FakeFileUploadApi();
      var fakeBlobUploader = new FakeBlobUploader();
      var fakeBlobName = 'blobName';

      var client = new BlobUploadClient(fakeConfig, fakeFileUpload, fakeBlobUploader);
      client.uploadToBlob(fakeBlobName, fakeStream, 42, function() {});
      assert(fakeFileUpload.getBlobSharedAccessSignature.calledWith(fakeBlobName, fakeConfig.sharedAccessSignature));
      client.updateSharedAccessSignature(newSas);
      client.uploadToBlob(fakeBlobName, fakeStream, 42, function() {});
      assert(fakeFileUpload.getBlobSharedAccessSignature.calledWith(fakeBlobName, newSas));
    });
  });

  describe('#uploadToBlob', function() {
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_004: [`uploadToBlob` shall obtain a blob SAS token using the IoT Hub service file upload API endpoint.]*/
    it('gets the Blob SAS token from the File Upload API', function() {
      var fakeStream = new stream.Readable();
      var fakeFileUpload = new FakeFileUploadApi();
      var fakeBlobUploader = new FakeBlobUploader();
      var fakeBlobName = 'blobName';
      var client = new BlobUploadClient(fakeConfig, fakeFileUpload, fakeBlobUploader);
      client.uploadToBlob(fakeBlobName, fakeStream, 42, function() {});
      assert(fakeFileUpload.getBlobSharedAccessSignature.calledOnce);
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_005: [`uploadToBlob` shall call the `done` callback with a `BlobSasError` parameter if retrieving the SAS token fails.]*/
    it('calls the done callback with a BlobSasError if getting the SAS token fails', function(done) {
      var fakeStream = new stream.Readable();
      var fakeFileUpload = new FakeFileUploadApi();
      var fakeBlobUploader = new FakeBlobUploader();
      var fakeBlobName = 'blobName';
      fakeFileUpload.getBlobSharedAccessSignature = function(blobName, sas, callback) {
        var error = new Error('fake error');
        callback(error);
      };

      var client = new BlobUploadClient(fakeConfig, fakeFileUpload, fakeBlobUploader);
      client.uploadToBlob(fakeBlobName, fakeStream, 42, function(err) {
        assert.instanceOf(err, errors.BlobSasError);
        done();
      });
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_006: [`uploadToBlob` shall upload the stream to the specified blob using its BlobUploader instance.]*/
    it('uploads the stream to the blob', function(done) {
      var fakeStream = new stream.Readable();
      var fakeFileUpload = new FakeFileUploadApi();
      var fakeBlobUploader = new FakeBlobUploader();
      var fakeBlobName = 'blobName';
      var fakeStreamLength = 42;
      var fakeBlobInfo = {
        correlationId: 'correlationId',
        blobName: fakeBlobName,
        containerName: 'container',
        hostName: 'host.name',
        sasToken: 'sasToken'
      };

      fakeFileUpload.getBlobSharedAccessSignature = function(blobName, sas, callback) {
        callback(null, fakeBlobInfo);
      };

      fakeBlobUploader.uploadToBlob = function(blobInfo, stream, streamLength) {
        assert.equal(blobInfo, fakeBlobInfo);
        assert.equal(stream, fakeStream);
        assert.equal(streamLength, fakeStreamLength);
        done();
      };

      var client = new BlobUploadClient(fakeConfig, fakeFileUpload, fakeBlobUploader);
      client.uploadToBlob(fakeBlobName, fakeStream, fakeStreamLength, function() {});
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_008: [`uploadToBlob` shall notify the result of a blob upload to the IoT Hub service using the file upload API endpoint.]*/
    it('sends the file uploaded notification once done with the upload', function() {
      var fakeStream = new stream.Readable();
      var fakeFileUpload = new FakeFileUploadApi();
      var fakeBlobUploader = new FakeBlobUploader();
      var fakeBlobName = 'blobName';
      var fakeStreamLength = 42;
      var fakeBlobInfo = {
        correlationId: 'correlationId',
        blobName: fakeBlobName,
        containerName: 'container',
        hostName: 'host.name',
        sasToken: 'sasToken'
      };

      fakeFileUpload.getBlobSharedAccessSignature = function(blobName, sas, callback) {
        callback(null, fakeBlobInfo);
      };

      fakeBlobUploader.uploadToBlob = function(blobInfo, stream, streamLength, callback) {
        callback(null, 'deviceId/' + fakeBlobName,  { statusCode: 200, body: 'Success' });
      };

      var client = new BlobUploadClient(fakeConfig, fakeFileUpload, fakeBlobUploader);
      client.uploadToBlob(fakeBlobName, fakeStream, fakeStreamLength, function() {});
      assert(fakeFileUpload.notifyUploadComplete.calledWith(fakeBlobInfo.correlationId));
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_009: [`uploadToBlob` shall call the `done` callback with a `BlobUploadNotificationError` if notifying the IoT Hub instance of the transfer outcome fails.]*/
    it('sends the file uploaded notification once done with the upload', function(done) {
      var fakeStream = new stream.Readable();
      var fakeFileUpload = new FakeFileUploadApi();
      var fakeBlobUploader = new FakeBlobUploader();
      var fakeBlobName = 'blobName';
      var fakeStreamLength = 42;
      var fakeBlobInfo = {
        correlationId: 'correlationId',
        blobName: fakeBlobName,
        containerName: 'container',
        hostName: 'host.name',
        sasToken: 'sasToken'
      };

      fakeFileUpload.getBlobSharedAccessSignature = function(blobName, sas, callback) {
        callback(null, fakeBlobInfo);
      };
      
      fakeFileUpload.notifyUploadComplete = function(correlationId, result, sharedAccessSignature, callback) {
        callback(new Error('could not notify hub'));
      };

      fakeBlobUploader.uploadToBlob = function(blobInfo, stream, streamLength, callback) {
        callback(null, 'deviceId/' + fakeBlobName,  { statusCode: 200, body: 'Success' });
      };

      var client = new BlobUploadClient(fakeConfig, fakeFileUpload, fakeBlobUploader);
      client.uploadToBlob(fakeBlobName, fakeStream, fakeStreamLength, function(err) {
        assert.instanceOf(err, errors.BlobUploadNotificationError);
        done();
      });
    });
    
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_010: [`uploadToBlob` shall call the `done` callback with no arguments if IoT Hub was successfully notified of the blob upload outcome, regardless of the success state of the transfer itself.]*/
    it('calls the done callback with no arguments if the upload as completed successfully and IoT Hub has been notified.', function(done) {
      var fakeStream = new stream.Readable();
      var fakeFileUpload = new FakeFileUploadApi();
      var fakeBlobUploader = new FakeBlobUploader();
      var fakeBlobName = 'blobName';
      var fakeStreamLength = 42;
      var fakeBlobInfo = {
        correlationId: 'correlationId',
        blobName: fakeBlobName,
        containerName: 'container',
        hostName: 'host.name',
        sasToken: 'sasToken'
      };

      fakeFileUpload.getBlobSharedAccessSignature = function(blobName, sas, callback) {
        callback(null, fakeBlobInfo);
      };
      
      fakeFileUpload.notifyUploadComplete = function(correlationId, result, sharedAccessSignature, callback) {
        callback();
      };

      fakeBlobUploader.uploadToBlob = function(blobInfo, stream, streamLength, callback) {
        callback(null, 'deviceId/' + fakeBlobName,  { statusCode: 200, body: 'Success' });
      };

      var client = new BlobUploadClient(fakeConfig, fakeFileUpload, fakeBlobUploader);
      client.uploadToBlob(fakeBlobName, fakeStream, fakeStreamLength, function(err) {
        assert.isUndefined(err);
        done();
      });
    });
  });
});