// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var endpoint = require('azure-iot-common').endpoint;
var FileUploadApi = require('../../lib/blob_upload/file_upload_api.js');
var packageJson = require('../../package.json');

describe('FileUploadApi', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_002: [`FileUploadApi` shall throw a `ReferenceError` if `deviceId` is falsy.]*/
    [undefined, null, ''].forEach(function (deviceId) {
      it('throws if deviceId is \'' + deviceId + '\'', function(){
        assert.throws(function() {
          return new FileUploadApi(deviceId, 'host.name', function() {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_003: [`FileUploadApi` shall throw a `ReferenceError` if `hostname` is falsy.]*/
    [undefined, null, ''].forEach(function (hostname) {
      it('throws if hostname is \'' + hostname + '\'', function(){
        assert.throws(function() {
          return new FileUploadApi('deviceId', hostname, function() {});
        }, ReferenceError);
      });
    });
    
    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_018: [`FileUploadApi` shall instantiate the default `azure-iot-http-base.Http` transport if `transport` is not specified, otherwise it shall use the specified transport.]*/
    it('uses the transport provided in the arguments', function() {
      var callCount = 0;
      var Transport = function () {
        this.buildRequest = function() {
          callCount++;
          return {
            write: function() {},
            end: function() {}
          };
        };
      };
      var testTransport = new Transport();
      var fileUpload = new FileUploadApi('deviceId', 'host.name', testTransport);
      fileUpload.getBlobSharedAccessSignature('blobName', 'sas', function() {});
      assert.equal(callCount, 1);
      fileUpload.notifyUploadComplete('correlationId', 'sas', { isSuccess: true, statusCode: 200, statusDescription: 'test' }, function() {});
      assert.equal(callCount, 2);
    });
  });
  
  describe('#getBlobSasUri', function() {
    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_004: [`getBlobSharedAccessSignature` shall throw a `ReferenceError` if `blobName` is falsy.]*/
    [undefined, null, ''].forEach(function (blobName) {
      it('throws if blobName is \'' + blobName + '\'', function(){
        var fileUpload = new FileUploadApi('deviceId', 'host.name', function() {});
        assert.throws(function() {
          fileUpload.getBlobSharedAccessSignature(blobName, 'signature', function() {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_005: [`getBlobSharedAccessSignature` shall throw a `ReferenceError` if `auth` is falsy.]*/
    [undefined, null, ''].forEach(function (auth) {
      it('throws if auth is \'' + auth + '\'', function(){
        var fileUpload = new FileUploadApi('deviceId', 'host.name', function() {});
        assert.throws(function() {
          fileUpload.getBlobSharedAccessSignature('blobName', auth, function() {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_006: [`getBlobSharedAccessSignature` shall create a `GET` HTTP request to a path formatted as the following:
    `/devices/<deviceId>/files/<filename>?api-version=<api-version>`]*/
    it('builds a valid path for the HTTP GET request', function() {
      var testFileName = 'testfile.txt';
      var testDeviceId = 'testDevice';
      var FakeHttpTransport = function() {
        this.buildRequest = function (method, path) {
          assert.equal(method, 'GET');
          assert.equal(path, '/devices/' + testDeviceId + '/files/' + testFileName + endpoint.versionQueryString());
          return {
            end: function() {}
          };
        };
      };
      
      var fileUpload = new FileUploadApi(testDeviceId, 'host.name', new FakeHttpTransport());
      fileUpload.getBlobSharedAccessSignature(testFileName, 'sas');
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_007: [The `GET` HTTP request shall have the following headers:
    ```
    Accept: 'application/json'
    Host: <hostname>
    Authorization: <sharedAccessSignature>
    'User-Agent': <sdk name and version>,
    ```]*/
    it('builds a valid set of headers with a shared access signature for the HTTP request', function() {
      var testFileName = 'testfile.txt';
      var testDeviceId = 'testDevice';
      var testHostName = 'host.name';
      var testIoTHubSas = 'hubSas';

      var FakeHttpTransport = function() {
        this.buildRequest = function (method, path, headers) {
          assert.equal(headers.Accept, 'application/json');
          assert.equal(headers.Host, testHostName);
          assert.equal(headers.Authorization, testIoTHubSas);
          assert.equal(headers['User-Agent'], packageJson.name + '/' + packageJson.version);
          
          return {
            end: function() {}
          };
        };
      };

      var fileUpload = new FileUploadApi(testDeviceId, testHostName, new FakeHttpTransport());
      fileUpload.getBlobSharedAccessSignature(testFileName, testIoTHubSas);
    });

    it('builds a valid set of headers with x509 authentication for the HTTP request', function() {
      var testFileName = 'testfile.txt';
      var testDeviceId = 'testDevice';
      var testHostName = 'host.name';
      var x509Auth = {
        cert: 'cert',
        key: 'key',
        passphrase: 'passphrase'
      };

      var FakeHttpTransport = function() {
        this.buildRequest = function (method, path, headers, host, auth) {
          assert.equal(headers.Accept, 'application/json');
          assert.equal(headers.Host, testHostName);
          assert.equal(headers['User-Agent'], packageJson.name + '/' + packageJson.version);
          assert.isUndefined(headers.Authorization);
          assert.equal(auth, x509Auth);
          return {
            end: function() {}
          };
        };
      };

      var fileUpload = new FileUploadApi(testDeviceId, testHostName, new FakeHttpTransport());
      fileUpload.getBlobSharedAccessSignature(testFileName, x509Auth);
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_008: [`getBlobSharedAccessSignature` shall call the `done` callback with an `Error` object if the request fails.]*/

    it('calls the done callback with an error if the request fails', function (done) {
      var testFileName = 'testfile.txt';
      var testDeviceId = 'testDevice';
      var testHostName = 'host.name';
      var testIoTHubSas = 'hubSas';

      var FailingTransport = function() {
        this.buildRequest = function(method, path, headers, hostname, x509auth, callback) {
          return {
            end: function() {
              callback(new Error('fake failure'));
            }
          };
        };
      };

      var fileUpload = new FileUploadApi(testDeviceId, testHostName, new FailingTransport());
      fileUpload.getBlobSharedAccessSignature(testFileName, testIoTHubSas, function (err) {
        assert.instanceOf(err, Error);
        done();
      });
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_009: [`getBlobSharedAccessSignature` shall call the `done` callback with a `null` error object and a result object containing a correlation ID and a SAS parameters if the request succeeds, in the following format:
    {
      correlationId: '<correlationIdString>',
      hostName: '<hostName>',
      blobName: '<blobName>',
      containerName: '<containerName>',
      sasToken: '<sasUriString>'
    }]*/
    it('calls the done callback with a null error object and a properly formatted result object if the request succeeds', function(done) {
      var testFileName = 'testfile.txt';
      var testDeviceId = 'testDevice';
      var testHostName = 'host.name';
      var testIoTHubSas = 'hubSas';
      var testCorrelationId = 'correlationId';
      var testContainerName = 'containerName';
      var testSasToken = 'sasToken';

      var fakeResult = JSON.stringify({
        correlationId: testCorrelationId,
        hostName: testHostName,
        blobName: testFileName,
        containerName: testContainerName,
        sasToken: testSasToken
      });

      var FakeHttpTransport = function() {
        this.buildRequest = function(method, path, headers, hostname, x509auth, callback) {
          return {
            end: function() {
              callback(null, fakeResult);
            }
          };
        };
      };

      var fileUpload = new FileUploadApi(testDeviceId, testHostName, new FakeHttpTransport());
      fileUpload.getBlobSharedAccessSignature(testFileName, testIoTHubSas, function (err, result) {
        if(err) {
          done(err);
        } else {
          assert.equal(result.correlationId, testCorrelationId);
          assert.equal(result.hostName, testHostName);
          assert.equal(result.blobName, testFileName);
          assert.equal(result.containerName, testContainerName);
          assert.equal(result.sasToken, testSasToken);
          done();
        }
      });
    });
  });
  
  describe('#notifyUploadComplete', function() {
    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_010: [`notifyUploadComplete` shall throw a `ReferenceError` if `correlationId` is falsy.]*/
    [undefined, null, ''].forEach(function (correlationId) {
      it('throws if correlationId is \'' + correlationId + '\'', function(){
        var fileUpload = new FileUploadApi('deviceId', 'host.name', function() {});
        assert.throws(function() {
          fileUpload.notifyUploadComplete(correlationId, 'sas', {}, function() {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_011: [`notifyUploadComplete` shall throw a `ReferenceError` if `auth` is falsy.]*/
    [undefined, null, ''].forEach(function (auth) {
      it('throws if auth is \'' + auth + '\'', function(){
        var fileUpload = new FileUploadApi('deviceId', 'host.name', function() {});
        assert.throws(function() {
          fileUpload.notifyUploadComplete('correlationId', auth, {}, function() {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_012: [`notifyUploadComplete` shall throw a `ReferenceError` if `uploadResult` is falsy.]*/
    [undefined, null, ''].forEach(function (uploadResult) {
      it('throws if uploadResult is \'' + uploadResult + '\'', function(){
        var fileUpload = new FileUploadApi('deviceId', 'host.name', function() {});
        assert.throws(function() {
          fileUpload.notifyUploadComplete('correlationId', 'sas', uploadResult, function() {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_013: [`notifyUploadComplete` shall create a `POST` HTTP request to a path formatted as the following: `/devices/<deviceId>/files/<correlationId>?api-version=<api-version>`]*/
    it('builds a valid path for the HTTP POST request', function() {
      var testDeviceId = 'testDevice';
      var testCorrelationId = 'correlationId';
      var testSas = 'testSAS';
      var testUploadResult = { isSuccess: true, statusCode: 200, statusDescription: 'Success' };
      
      var FakeHttpTransport = function() {
        this.buildRequest = function (method, path) {
          assert.equal(method, 'POST');
          assert.equal(path, '/devices/' + testDeviceId + '/files/notifications/' + testCorrelationId + endpoint.versionQueryString());
          return {
            write: function() {},
            end: function() {}
          };
        };
      };
      var fileUpload = new FileUploadApi(testDeviceId, 'host.name', new FakeHttpTransport());
      fileUpload.notifyUploadComplete(testCorrelationId, testSas, testUploadResult, function() {});
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_014: [** The `POST` HTTP request shall have the following headers:
    ```
    Host: <hostname>,
    Authorization: <sharedAccessSignature>,
    'User-Agent': <version>,
    'Content-Type': 'application/json; charset=utf-8',
    'Content-Length': <content length>,
    'iothub-name': <hub name>
    ```]*/
    it('builds a valid set of headers with a shared access signature for the HTTP request', function() {
      var testDeviceId = 'testDevice';
      var testHostName = 'host.name';
      var testIoTHubSas = 'hubSas';
      var testUploadResult = { isSuccess: true, statusCode: 200, statusDescription: 'Success' };

      var FakeHttpTransport = function() {
        this.buildRequest = function (method, path, headers) {
          assert.equal(headers.Host, testHostName);
          assert.equal(headers.Authorization, testIoTHubSas);
          assert.equal(headers['Content-Type'], 'application/json; charset=utf-8');
          assert.equal(headers['Content-Length'], JSON.stringify(testUploadResult).length);
          assert.equal(headers['iothub-name'], testHostName.split('.')[0]);
          
          return {
            write: function() {},
            end: function() {}
          };
        };
      };

      var fileUpload = new FileUploadApi(testDeviceId, testHostName, new FakeHttpTransport());
      fileUpload.notifyUploadComplete('correlationId', testIoTHubSas, testUploadResult, function() {});
    });

    it('builds a valid set of headers and x509 authentication for the HTTP request', function() {
      var testDeviceId = 'testDevice';
      var testHostName = 'host.name';
      var testUploadResult = { isSuccess: true, statusCode: 200, statusDescription: 'Success' };
      var x509Auth = {
        cert: 'cert',
        key: 'key',
        passphrase: 'passphrase'
      };

      var FakeHttpTransport = function() {
        this.buildRequest = function (method, path, headers, host, auth) {
          assert.equal(headers.Host, testHostName);
          assert.isUndefined(headers.Authorization);
          assert.equal(headers['Content-Type'], 'application/json; charset=utf-8');
          assert.equal(headers['Content-Length'], JSON.stringify(testUploadResult).length);
          assert.equal(headers['iothub-name'], testHostName.split('.')[0]);
          assert.equal(auth, x509Auth);

          return {
            write: function() {},
            end: function() {}
          };
        };
      };

      var fileUpload = new FileUploadApi(testDeviceId, testHostName, new FakeHttpTransport());
      fileUpload.notifyUploadComplete('correlationId', x509Auth, testUploadResult, function() {});
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_015: [** The `POST` HTTP request shall have the following body:
    ```
    {
      isSuccess: <true/false>,
      statusCode: <upload status code from the blob upload>,
      statusDescription: <string describing the status code>
    }
    ```]*/
    it('builds a valid body for the HTTP request', function() {
      var testDeviceId = 'testDevice';
      var testHostName = 'host.name';
      var testIoTHubSas = 'hubSas';
      var testUploadResult = { isSuccess: true, statusCode: 200, statusDescription: 'Success' };

      var FakeHttpTransport = function() {
        this.buildRequest = function () {
          return {
            write: function(body) {
              assert.equal(body, JSON.stringify(testUploadResult));
            },
            end: function() {}
          };
        };
      };

      var fileUpload = new FileUploadApi(testDeviceId, testHostName, new FakeHttpTransport());
      fileUpload.notifyUploadComplete('correlationId', testIoTHubSas, testUploadResult, function() {});
    });
    
    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_016: [`notifyUploadComplete` shall call the `done` callback with an `Error` object if the request fails.]*/
    it('calls the done callback with an error if the request fails', function (done) {
      var FailingTransport = function() {
        this.buildRequest = function(method, path, headers, hostname, x509auth, callback) {
          return {
            write: function() {},
            end: function() {
              callback(new Error('fake failure'));
            }
          };
        };
      };

      var fileUpload = new FileUploadApi('testDeviceId', 'testHostName', new FailingTransport());
      fileUpload.notifyUploadComplete('correlationId', 'testIoTHubSas', { isSuccess: true, statusCode: 200, statusDescription: 'Success' }, function (err) {
        assert.instanceOf(err, Error);
        done();
      });
    });

    /*Tests_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_017: [`notifyUploadComplete` shall call the `done` callback with no parameters if the request succeeds.]*/
    it('calls the done callback with no parameters if the request succeeds', function (done) {
      var FakeHttpTransport = function() {
        this.buildRequest = function(method, path, headers, hostname, x509auth, callback) {
          return {
            write: function() {},
            end: function() {
              callback();
            }
          };
        };
      };

      var fileUpload = new FileUploadApi('testDeviceId', 'testHostName', new FakeHttpTransport());
      fileUpload.notifyUploadComplete('correlationId', 'testIoTHubSas', { isSuccess: true, statusCode: 200, statusDescription: 'Success' }, done);
    });
  });
});