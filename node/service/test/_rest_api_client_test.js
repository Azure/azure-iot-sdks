// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var errors = require('azure-iot-common').errors;
var PackageJson = require('../package.json');
var RestApiClient = require('../lib/rest_api_client.js');

var fakeConfig = { host: 'host', sharedAccessSignature: 'sas' };

describe('RestApiClient', function() {
  describe('#constructor', function() {
    [undefined, null, ''].forEach(function(badConfig) {
      it('throws a ReferenceError if \'config\' is \'' + badConfig + '\'', function() {
        assert.throws(function() {
          return new RestApiClient(badConfig);
        }, ReferenceError);
      });
    });

    ['host', 'sharedAccessSignature'].forEach(function(badPropName) {
      [undefined, null, ''].forEach(function(badPropValue) {
        it('throws an ArgumentError if config.' + badPropName + 'is \'' + badPropValue + '\'', function() {
          var badConfig = JSON.parse(JSON.stringify(fakeConfig));
          badConfig[badPropName] = badPropValue;
          assert.throws(function() {
            return new RestApiClient(badConfig);
          }, errors.ArgumentError);
        });
      });
    });


  });

  describe('#executeApiCall', function() {
    [undefined, null, ''].forEach(function(badMethod) {
      it('throws a ReferenceError if \'method\' is \'' + badMethod + '\'', function() {
        var client = new RestApiClient(fakeConfig);
        assert.throws(function() {
          client.executeApiCall(badMethod, '/fake/path', null, null, function() {});
        }, ReferenceError);
      });
    });

    [undefined, null, ''].forEach(function(badPath) {
      it('throws a ReferenceError if \'path\' is \'' + badPath + '\'', function() {
        var client = new RestApiClient(fakeConfig);
        assert.throws(function() {
          client.executeApiCall('GET', badPath, null, null, function() {});
        }, ReferenceError);
      });
    });

    it('adds default headers to every request', function(testCallback) {
      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          assert(headers.Authorization, fakeConfig.sharedAccessSignature);
          assert.isString(headers['Request-Id']);
          assert.equal(headers['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function() {},
            end: function() {
              requestCallback();
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall('GET', '/fake/path', null, null, testCallback);
    });

    it('can use the timeout parameter as callback', function(testCallback) {
      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          return {
            setTimeout: function() {
              assert.fail();
            },
            write: function() {},
            end: function() {
              requestCallback();
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall('GET', '/fake/path', null, null, testCallback);
    });

    it('sets the request timeout if specified', function(testCallback) {
      var testTimeout = 42000;
      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          return {
            setTimeout: function(timeout) {
              assert.equal(timeout, testTimeout);
            },
            write: function() {},
            end: function() {
              requestCallback();
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall('GET', '/fake/path', null, null, testTimeout, testCallback);
    });
  });

  describe('#updateSharedAccessSignature', function() {
    [undefined, null, ''].forEach(function(badSas) {
      it('throws if \'sharedAccessSignature\' is \'' + badSas + '\'', function() {
        var client = new RestApiClient({host: 'host', sharedAccessSignature: 'sas'});
        assert.throws(function () {
          client.updateSharedAccessSignature(badSas);
        }, ReferenceError);
      });

      it('uses the updated sharedAccessSignature', function (done) {
        var newSas = 'newSas';
        var fakeHttpRequestBuilder = {
          buildRequest: function (method, path, headers) {
            assert.equal(headers.Authorization, newSas);
            return {
              write: function() {},
              end: function() {
                done();
              }
            };
          }
        };

        var client = new RestApiClient({host: 'host', sharedAccessSignature: 'sas'}, fakeHttpRequestBuilder);
        client.updateSharedAccessSignature(newSas);
        client.executeApiCall('POST', '/fake/path', null, null, function() {});
      });
    });
  });

  describe('translateError', function() {
    [
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_003: [`translateError` shall return an `ArgumentError` if the HTTP response status code is `400`.]*/
      { statusCode: 400, statusMessage: 'Bad request', expectedErrorType: errors.ArgumentError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_004: [`translateError` shall return an `UnauthorizedError` if the HTTP response status code is `401`.]*/
      { statusCode: 401, statusMessage: 'Unauthorized', expectedErrorType: errors.UnauthorizedError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_005: [`translateError` shall return an `TooManyDevicesError` if the HTTP response status code is `403`.]*/
      { statusCode: 403, statusMessage: 'Forbidden', expectedErrorType: errors.TooManyDevicesError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_006: [`translateError` shall return an `IotHubNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `IotHubNotFound`.*/
      { statusCode: 404, hubErrorCode: 'IotHubNotFound', statusMessage: 'Not found', expectedErrorType: errors.IotHubNotFoundError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_012: [`translateError` shall return an `DeviceNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `DeviceNotFound`.*/
      { statusCode: 404, hubErrorCode: 'DeviceNotFound', statusMessage: 'Not found', expectedErrorType: errors.DeviceNotFoundError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_007: [`translateError` shall return an `DeviceAlreadyExistsError` if the HTTP response status code is `409`.]*/
      { statusCode: 409, hubErrorCode: 'DeviceAlreadyExists', statusMessage: 'Conflict', expectedErrorType: errors.DeviceAlreadyExistsError },
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_013: [`translateError` shall return an `InvalidEtagError` if the HTTP response status code is `412`.]*/
      { statusCode: 412, hubErrorCode: 'InvalidEtag', statusMessage: 'Precondition failed', expectedErrorType: errors.InvalidEtagError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_011: [`translateError` shall return an `ThrottlingError` if the HTTP response status code is `429`.]*/
      { statusCode: 429, statusMessage: 'Too many requests', expectedErrorType: errors.ThrottlingError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_008: [`translateError` shall return an `InternalServerError` if the HTTP response status code is `500`.]*/
      { statusCode: 500, statusMessage: 'Internal Server Error', expectedErrorType: errors.InternalServerError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_009: [`translateError` shall return an `ServiceUnavailableError` if the HTTP response status code is `503`.]*/
      { statusCode: 503, statusMessage: 'Server Unavailable', expectedErrorType: errors.ServiceUnavailableError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_002: [If the HTTP error code is unknown, `translateError` should return a generic Javascript `Error` object.]*/
      { statusCode: 1337, statusMessage: 'unknown', expectedErrorType: Error}
    ].forEach(function(testParams) {
      it('returns a \'' + testParams.expectedErrorType.name + '\' if the response status code is \'' + testParams.statusCode + '\'', function(){
        var fakeReponse = {
          statusCode: testParams.statusCode,
          statusMessage: testParams.statusMessage,
        };

        var fakeResponseBody = '';
        if (testParams.hubErrorCode) {
          fakeResponseBody = "{\"Message\":\"ErrorCode:" + testParams.hubErrorCode + ";\\\"fake test error message\\\"\"}";
        } else {
          fakeResponseBody = testParams.statusCode + ': ' + testParams.statusMessage;
        }

        /* Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_010: [`translateError` shall accept 3 arguments:
        * - A custom error message to give context to the user.
        * - the body of  the HTTP response, containing the explanation of why the request failed
        * - the HTTP response object itself]
        */
        var err = RestApiClient.translateError(fakeResponseBody, fakeReponse);
        assert.instanceOf(err, testParams.expectedErrorType);

        /* Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_001: [** Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 3 properties:
        *- `response` shall contain the `IncomingMessage` object returned by the HTTP layer.
        *- `reponseBody` shall contain the content of the HTTP response.
        *- `message` shall contain a human-readable error message]
        */
        assert.isOk(err.message);
        assert.equal(err.responseBody, fakeResponseBody);
        assert.equal(err.response, fakeReponse);
      });
    });
  });
});