// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var assert = require('chai').assert;
var BlobUploadResult = require('../../lib/blob_upload/blob_upload_result.js');

describe('BlobUploadResult', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_001: [The `isSuccess` parameter shall be assigned to the the `isSuccess` property of the newly created `BlobUploadResult` instance.]*/
    it('Assigns the \'isSuccess\' parameter to the \'isSuccess\' property', function() {
      [true, false].forEach(function(successValue) {
        var result = new BlobUploadResult(successValue);
        assert.equal(result.isSuccess, successValue);
      });
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_002: [The `statusCode` parameter shall be assigned to the the `statusCode` property of the newly created `BlobUploadResult` instance.]*/
    it('Assigns the \'statusCode\' parameter to the \'statusCode\' property', function() {
      var testStatusCode = 42;
      var result = new BlobUploadResult(true, testStatusCode);
      assert.equal(result.statusCode, testStatusCode);
    });

    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_003: [The `statusDescription` parameter shall be assigned to the the `statusDescription` property of the newly created `BlobUploadResult` instance.]*/
    it('Assigns the \'statusDescription\' parameter to the \'statusDescription\' property', function() {
      var testStatusDescription = 'test description';
      var result = new BlobUploadResult(true, 42, testStatusDescription);
      assert.equal(result.statusDescription, testStatusDescription);
    });
  });
  
  describe('#fromAzureStorageCallbackArgs', function(){
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_004: [If the `err` argument is not `null`, the `BlobUploadResult` error shall have the `isSuccess` property set to `false`.]*/
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_005: [If `err`, `body` and `response` are not `null` (HTTP error), the `BlobUploadResult` error shall have the `statusCode` property set to the HTTP error code of the blob upload response.]*/
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_006: [If `err`, `body` and `response` are not `null` (HTTP error), the `BlobUploadResult` error shall have the `statusDescription` property set to the HTTP error body of the blob upload response.]*/
    it('Creates a correct BlobUploadResult instance when the error in an HTTP error', function() {
      var testBody = 'test body';
      var testCode = 400;
      var result = BlobUploadResult.fromAzureStorageCallbackArgs(new Error(), 'foo', { statusCode: testCode, body: testBody });
      assert.isFalse(result.isSuccess);
      assert.equal(result.statusCode, testCode);
      assert.equal(result.statusDescription, testBody);
    });
    
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_007: [If the `err` argument is not `null` but body and response are `undefined` (non HTTP error), the `BlobUploadResult` error shall have the `statusCode` property set to -1.]*/
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_008: [If the `err` argument is not `null` but body and response are `undefined` (non HTTP error), the `BlobUploadResult` error shall have the `statusDescription` property set to the error message.]*/
    it('Creates a correct BlobUploadResult instance when the error in not an HTTP error', function() {
      var testErrorMessage = 'ECONNRESET';
      var result = BlobUploadResult.fromAzureStorageCallbackArgs(new Error(testErrorMessage));
      assert.isFalse(result.isSuccess);
      assert.equal(result.statusCode, -1);
      assert.equal(result.statusDescription, testErrorMessage);
    });
    
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_009: [If `err` is null but `body` and `reponse` are provided, the `BlobUploadResult` error shall have the `isSuccess` property set to `false`.]*/
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_010: [If `err` is null but `body` and `reponse` are provided, the `BlobUploadResult` error shall have the `statusCode` property set to the HTTP status code of the blob upload response.]*/
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_011: [If `err` is null but `body` and `reponse` are provided, the `BlobUploadResult` error shall have the `statusDescription` property set to the HTTP response body of the blob upload response.]*/
    it('Creates a correct BlobUploadResult instance when the error is null', function() {
      var testBody = 'test body';
      var testCode = 200;
      var result = BlobUploadResult.fromAzureStorageCallbackArgs(null, 'foo', { statusCode: testCode, body: testBody });
      assert.isTrue(result.isSuccess);
      assert.equal(result.statusCode, testCode);
      assert.equal(result.statusDescription, testBody);
    });
    
    /*Tests_SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_012: [If `err` is null and `body` and/or `response` is/are falsy, `fromAzureStorageCallbackArgs` shall throw a `ReferenceError`.]*/
    it('Throws a \'ReferenceError\' if err is null but body is not provided', function() {
      assert.throws(function() {
        BlobUploadResult.fromAzureStorageCallbackArgs(null, undefined, { statusCode: 400 });
      }, ReferenceError);
    });

    it('Throws a \'ReferenceError\' if err is null but response is not provided', function() {
      assert.throws(function() {
        BlobUploadResult.fromAzureStorageCallbackArgs(null, 'foo');
      }, ReferenceError);
    });
  });
});