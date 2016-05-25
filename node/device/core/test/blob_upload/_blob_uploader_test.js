// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var stream = require('stream');
var storage = require('azure-storage');
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var BlobUploader = require('../../lib/blob_upload/blob_uploader.js');
var FakeStorageApi = require('./fake_storage_api.js');

describe('BlobUploader', function() {
  describe('#constructor', function () {
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_16_007: [if `storageApi` is falsy, `BlobUploader` should use the `azure-storage` package to upload data.]*/
    it('creates the \'BlobUploader\' instance with the azure-storage package if \'storageApi\' is falsy', function() {
      var uploader = new BlobUploader();
      assert.deepEqual(uploader.storageApi, storage);
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_16_008: [if `storageApi` is truthy, `BlobUploader` should use the `storageApi` object to upload data.]*/
    it('creates the \'BlobUploader\' instance with the \'storageApi\' argument if \'storageApi\' is truthy', function() {
      var testApi = { foo: 'bar' };
      var uploader = new BlobUploader(testApi);
      assert.deepEqual(uploader.storageApi, testApi);
    });
  });

  describe('#uploadToBlob', function() {
    var fakeBlobInfo = {
      hostName: 'host.name',
      containerName: 'containerName',
      blobName: 'blobName',
      sasToken: 'sasToken'
    };
    
    var fakeStream = new stream.Readable();
    
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_16_001: [`uploadToBlob` shall throw a `ReferenceError` if `blobInfo` is falsy.]*/
    [undefined, '', null, 0].forEach(function (falsyValue) {
      it('throws when \'blobInfo\' is \'' + falsyValue + '\'', function() {
        assert.throws(function() {
          var uploader = new BlobUploader(FakeStorageApi);
          uploader.uploadToBlob(falsyValue, fakeStream, 42, function () {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_16_002: [`uploadToBlob` shall throw a `ReferenceError` if `stream` is falsy.]*/
    [undefined, '', null, 0].forEach(function (falsyValue) {
      it('throws when \'stream\' is \'' + falsyValue + '\'', function() {
        assert.throws(function() {
          var uploader = new BlobUploader(FakeStorageApi);
          uploader.uploadToBlob(fakeBlobInfo, falsyValue, 42, function () {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_16_003: [`uploadToBlob` shall throw a `ReferenceError` if `streamSize` is falsy.]*/
    [undefined, '', null, 0].forEach(function (falsyValue) {
      it('throws when \'streamSize\' is \'' + falsyValue + '\'', function() {
        assert.throws(function() {
          var uploader = new BlobUploader(FakeStorageApi);
          uploader.uploadToBlob(fakeBlobInfo, fakeStream, falsyValue, function () {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_16_004: [`uploadToBlob` shall throw an `ArgumentError` if `blobInfo` is missing one or more of the following properties: `hostName`, `containerName`, `blobName`, `sasToken`).]*/
    [null, undefined, ''].forEach(function (value) {
      it('throws an ArgumentError if \'blobInfo.hostName\' is \'' + value + '\'', function() {
        assert.throws(function() {
          var uploader = new BlobUploader();
          var badBlobInfo = {
            hostName: value,
            containerName: 'container',
            blobName: 'blobName',
            sasToken: 'sasToken'
          };

          uploader.uploadToBlob(badBlobInfo, fakeStream, 42, function() {});
        }, ArgumentError);
      });
    });

    [null, undefined, ''].forEach(function (value) {
      it('throws an ArgumentError if \'blobInfo.containerName\' is \'' + value + '\'', function() {
        assert.throws(function() {
          var uploader = new BlobUploader();
          var badBlobInfo = {
            hostName: 'hostName',
            containerName: value,
            blobName: 'blobName',
            sasToken: 'sasToken'
          };

          uploader.uploadToBlob(badBlobInfo, fakeStream, 42, function() {});
        }, ArgumentError);
      });
    });

    [null, undefined, ''].forEach(function (value) {
      it('throws an ArgumentError if \'blobInfo.blobName\' is \'' + value + '\'', function() {
        assert.throws(function() {
          var uploader = new BlobUploader();
          var badBlobInfo = {
            hostName: 'hostName',
            containerName: 'containerName',
            blobName: value,
            sasToken: 'sasToken'
          };

          uploader.uploadToBlob(badBlobInfo, fakeStream, 42, function() {});
        }, ArgumentError);
      });
    });

    [null, undefined, ''].forEach(function (value) {
      it('throws an ArgumentError if \'blobInfo.sasToken\' is \'' + value + '\'', function() {
        assert.throws(function() {
          var uploader = new BlobUploader();
          var badBlobInfo = {
            hostName: 'hostName',
            containerName: 'containerName',
            blobName: 'blobName',
            sasToken: value
          };

          uploader.uploadToBlob(badBlobInfo, fakeStream, 42, function() {});
        }, ArgumentError);
      });
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_16_005: [`uploadToBlob` shall call the `done` calback with the result of the storage api call.]*/
    it('calls the \'done\' callback with the result of the storage api call', function (done) {
      var uploader = new BlobUploader(FakeStorageApi);

      uploader.uploadToBlob(fakeBlobInfo, fakeStream, 42, function (err, body, response) {
        assert.equal(err, 'fakeError');
        assert.equal(body, 'fakeBody');
        assert.equal(response, 'fakeResponse');
        done();
      });
    });
  });
});
