// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var errors = require('azure-iot-common').errors;
var HttpBase = require('azure-iot-http-base').Http;
var PackageJson = require('../package.json');
var RestApiClient = require('../lib/rest_api_client.js');

var fakeConfig = { host: 'host', sharedAccessSignature: 'sas' };

describe('RestApiClient', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_001: [The `RestApiClient` constructor shall throw a `ReferenceError` if config is falsy.]*/
    [undefined, null, ''].forEach(function(badConfig) {
      it('throws a ReferenceError if \'config\' is \'' + badConfig + '\'', function() {
        assert.throws(function() {
          return new RestApiClient(badConfig);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_002: [The `RestApiClient` constructor shall throw an `ArgumentError` if config is missing a `host` or `sharedAccessSignature` property.]*/
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

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_003: [The `RestApiClient` constructor shall use `azure-iot-common.Http` as the internal HTTP client if the `httpBase` argument is `undefined`.]*/
    it('uses \'azure-iot-common.Http\' if the \'httpBase\' argument is undefined', function() {
      var client = new RestApiClient(fakeConfig);
      assert.instanceOf(client._http, HttpBase);
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_004: [The `RestApiClient` constructor shall use the value of the `httpBase` argument as the internal HTTP client if present.]*/
    it('uses the \'httpBase\' argument if specified', function(testCallback) {
      var fakeHttpBase = {
        buildRequest: function() {
          return {
            end: function() {
              testCallback();
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpBase);
      client.executeApiCall('method', 'path', null, null, function() {});
    });
  });

  describe('#executeApiCall', function() {

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_005: [The `executeApiCall` method shall throw a `ReferenceError` if the `method` argument is falsy.]*/
    [undefined, null, ''].forEach(function(badMethod) {
      it('throws a ReferenceError if \'method\' is \'' + badMethod + '\'', function() {
        var client = new RestApiClient(fakeConfig);
        assert.throws(function() {
          client.executeApiCall(badMethod, '/fake/path', null, null, function() {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_006: [The `executeApiCall` method shall throw a `ReferenceError` if the `path` argument is falsy.]*/
    [undefined, null, ''].forEach(function(badPath) {
      it('throws a ReferenceError if \'path\' is \'' + badPath + '\'', function() {
        var client = new RestApiClient(fakeConfig);
        assert.throws(function() {
          client.executeApiCall('GET', badPath, null, null, function() {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_007: [The `executeApiCall` method shall add the following headers to the request:
    - Authorization: <this.sharedAccessSignature>
    - Request-Id: <guid>
    - User-Agent: <version string>]*/
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
    
    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_029: [If `done` is `undefined` and the `timeout` argument is a function, `timeout` should be used as the callback.]*/
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

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_030: [If `timeout` is defined and is not a function, the HTTP request timeout shall be adjusted to match the value of the argument.]*/
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

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_008: [The `executeApiCall` method shall build the HTTP request using the arguments passed by the caller.]*/
    it('builds the HTTP request with the passed arguments', function(testCallback) {
      var testMethod = 'GET';
      var testPath = '/test/path';
      var testHeaderKey = 'Content-Type';
      var testHeaderValue = 'text/plain; charset=utf-8';
      var testHeaders = {};
      testHeaders[testHeaderKey] = testHeaderValue;
      var testRequestBody = 'foo';
      var testTimeout = 42;

      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          assert.equal(method, testMethod);
          assert.equal(path, testPath);
          assert.equal(headers[testHeaderKey], testHeaderValue);
          return {
            setTimeout: function(timeout) {
              assert.equal(timeout, testTimeout);
            },
            write: function(body) {
              assert.equal(body, testRequestBody);
            },
            end: function() {
              requestCallback();
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall(testMethod, testPath, testHeaders, testRequestBody, testTimeout, testCallback);
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_009: [If the HTTP request is successful the `executeApiCall` method shall parse the JSON response received and call the `done` callback with a `null` first argument, the parsed result as a second argument and the HTTP response object itself as a third argument.]*/
    it('calls the done callback with null, a result and a response if the request succeeds', function (testCallback) {
      var fakeResponse = { statusCode: 200 };
      var fakeResponseBody = {
        foo: 'bar'
      };

      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          return {
            setTimeout: function() {},
            write: function() {},
            end: function() {
              requestCallback(null, JSON.stringify(fakeResponseBody), fakeResponse);
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall('GET', '/test/path', null, null, function(err, result, response) {
        assert.isNull(err);
        assert.deepEqual(result, fakeResponseBody);
        assert.equal(response, fakeResponse);
        testCallback();
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_010: [If the HTTP request fails with an error code >= 300 the `executeApiCall` method shall translate the HTTP error into a transport-agnostic error using the `translateError` method and call the `done` callback with the resulting error as the only argument.]*/
    it('calls the done callback with a translated error if the request fails with an HTTP error', function (testCallback) {
      var fakeResponse = { statusCode: 401 };

      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          return {
            setTimeout: function() {},
            write: function() {},
            end: function() {
              requestCallback(new Error('Unauthorized'), '', fakeResponse);
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall('GET', '/test/path', null, null, function(err) {
        assert.instanceOf(err, errors.UnauthorizedError);
        testCallback();
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_011: [If the HTTP request fails without an HTTP error code the `executeApiCall` shall call the `done` callback with the error itself as the only argument.]*/
    it('calls the done callback with a translated error if the request fails with an HTTP error', function (testCallback) {
      var fakeError = new Error('Non-HTTP error');
      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          return {
            setTimeout: function() {},
            write: function() {
            },
            end: function() {
              requestCallback(fakeError);
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall('GET', '/test/path', null, null, function(err) {
        assert.equal(err, fakeError);
        testCallback();
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_031: [If there's is a `Content-Type` header and its value is `application/json; charset=utf-8` and the `body` argument is not a `string`, the body of the request shall be stringified using `JSON.stringify()`.]*/
    it('calls JSON.stringify on the request body if the Content-Type header is \'application/json; charset=utf-8\'', function(testCallback) {
      var testHeaders = {
        'Content-Type': 'application/json; charset=utf-8'
      };
      var testRequestBody = {
        key: 'value'
      };

      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          return {
            write: function(body) {
              assert.equal(body, JSON.stringify(testRequestBody));
            },
            end: function() {
              requestCallback();
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall('GET', '/test/path', testHeaders, testRequestBody, testCallback);
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_035: [If there's is a `Content-Type` header and its value is `application/json; charset=utf-8` and the `body` argument is a `string` it shall be used as is as the body of the request.]*/
    it('uses the body argument as is if it is a string and the Content-Type header is \'application/json; charset=utf-8\'', function(testCallback) {
      var testHeaders = {
        'Content-Type': 'application/json; charset=utf-8'
      };
      var testRequestBody = JSON.stringify({
        key: 'value'
      });

      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          return {
            write: function(body) {
              assert.equal(body, testRequestBody);
            },
            end: function() {
              requestCallback();
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall('GET', '/test/path', testHeaders, testRequestBody, testCallback);
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_032: [If there's is a `Content-Type` header and its value is `text/plain; charset=utf-8`, the `body` argument shall be used.]*/
    it('sends the request body directly if the Content-Type header is \'text/plain; charset=utf-8\'', function(testCallback) {
      var testHeaders = {
        'Content-Type': 'text/plain; charset=utf-8'
      };
      var testRequestBody = 'foo';

      var fakeHttpHelper = {
        buildRequest: function(method, path, headers, host, requestCallback) {
          return {
            write: function(body) {
              assert.equal(body, testRequestBody);
            },
            end: function() {
              requestCallback();
            }
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      client.executeApiCall('GET', '/test/path', testHeaders, testRequestBody, testCallback);
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_033: [The `executeApiCall` shall throw a `TypeError` if there's is a `Content-Type` header and its value is `text/plain; charset=utf-8` and the `body` argument is not a string.]*/
    it('throws a TypeError if the Content-Type header is \'text/plain; charset=utf-8\' but the requestBody argument is not a string', function() {
      var testHeaders = {
        'Content-Type': 'text/plain; charset=utf-8'
      };
      var testRequestBody = {
        key: 'value'
      };

      var fakeHttpHelper = {
        buildRequest: function() {
          return {
            write: function() {},
            end: function() {}
          };
        }
      };

      var client = new RestApiClient(fakeConfig, fakeHttpHelper);
      assert.throws(function() {
        client.executeApiCall('GET', '/test/path', testHeaders, testRequestBody, function() {});
      }, TypeError);
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_036: [The `executeApiCall` shall set the `Content-Length` header to the length of the serialized value of `requestBody` if it is truthy.]*/
    [
      { contentType: 'text/plain; charset=utf-8', content: 'foo' },
      { contentType: 'application/json; charset=utf-8', content: 'foo' },
      { contentType: 'application/json; charset=utf-8', content: { key: 'value' }}
    ].forEach(function(testConfig) {
      it('sets the Content-Length header when Content-Type is \'' + testConfig.contentType + '\' and content is ' + JSON.stringify(testConfig.content), function(testCallback) {
        var testHeaders = {
          'Content-Type': testConfig.contentType
        };

        var testRequestBody = testConfig.content;
        var expectedContentLength = (typeof testRequestBody === 'string') ? testRequestBody.length : JSON.stringify(testRequestBody).length;

        var fakeHttpHelper = {
          buildRequest: function(method, path, headers, host, requestCallback) {
            assert.equal(headers['Content-Length'], expectedContentLength);
            return {
              write: function() { },
              end: function() {
                requestCallback();
              }
            };
          }
        };

        var client = new RestApiClient(fakeConfig, fakeHttpHelper);
        client.executeApiCall('GET', '/test/path', testHeaders, testRequestBody, testCallback);
      });
    });
  });

  describe('#updateSharedAccessSignature', function() {
    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_034: [The `updateSharedAccessSignature` method shall throw a `ReferenceError` if the `sharedAccessSignature` argument is falsy.]*/
    [undefined, null, ''].forEach(function(badSas) {
      it('throws if \'sharedAccessSignature\' is \'' + badSas + '\'', function() {
        var client = new RestApiClient({host: 'host', sharedAccessSignature: 'sas'});
        assert.throws(function () {
          client.updateSharedAccessSignature(badSas);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_028: [The `updateSharedAccessSignature` method shall update the `sharedAccessSignature` configuration parameter that is used in the `Authorization` header of all HTTP requests.]*/
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
      /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_019: [`translateError` shall return a `DeviceTimeoutError` if the HTTP response status code is `408`.]*/
      { statusCode: 408, statusMessage: 'Device Timeout', expectedErrorType: errors.DeviceTimeoutError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_007: [`translateError` shall return an `DeviceAlreadyExistsError` if the HTTP response status code is `409`.]*/
      { statusCode: 409, hubErrorCode: 'DeviceAlreadyExists', statusMessage: 'Conflict', expectedErrorType: errors.DeviceAlreadyExistsError },
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_013: [`translateError` shall return an `InvalidEtagError` if the HTTP response status code is `412`.]*/
      { statusCode: 412, hubErrorCode: 'InvalidEtag', statusMessage: 'Precondition failed', expectedErrorType: errors.InvalidEtagError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_011: [`translateError` shall return an `ThrottlingError` if the HTTP response status code is `429`.]*/
      { statusCode: 429, statusMessage: 'Too many requests', expectedErrorType: errors.ThrottlingError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_008: [`translateError` shall return an `InternalServerError` if the HTTP response status code is `500`.]*/
      { statusCode: 500, statusMessage: 'Internal Server Error', expectedErrorType: errors.InternalServerError },
      /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_024: [`translateError` shall return a `BadDeviceResponseError` if the HTTP response status code is `502`.]*/
      { statusCode: 502, statusMessage: 'Bad Gateway', expectedErrorType: errors.BadDeviceResponseError },
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_009: [`translateError` shall return an `ServiceUnavailableError` if the HTTP response status code is `503`.]*/
      { statusCode: 503, statusMessage: 'Server Unavailable', expectedErrorType: errors.ServiceUnavailableError },
      /*Tests_SRS_NODE_IOTHUB_REST_API_CLIENT_16_026: [`translateError` shall return a `GatewayTimeoutError` if the HTTP response status code is `504`.]*/
      { statusCode: 504, statusMessage: 'Gateway Timeout', expectedErrorType: errors.GatewayTimeoutError },
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