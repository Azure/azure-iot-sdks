// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

"use strict";
var assert = require('chai').assert;
var endpoint = require('azure-iot-common').endpoint;
var errors = require('azure-iot-common').errors;
var Registry = require('../lib/registry.js');
var DeviceTwin = require('../lib/device_twin.js');
var PackageJson = require('../package.json');

var fakeDevice = { deviceId: 'deviceId' };
var fakeConfig = { host: 'host', sharedAccessSignature: 'sas' };

function testFalsyArg(methodUnderTest, argName, argValue, ExpectedErrorType) {
  var errorName = ExpectedErrorType ? ExpectedErrorType.name : 'Error';
  it('throws a ' + errorName + ' if \'' + argName + '\' is \'' + JSON.stringify(argValue) + '\'', function() {
    var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
    assert.throws(function() {
      registry[methodUnderTest](argValue, function() {});
    }, ExpectedErrorType);
  });
}

function testErrorCallback(methodUnderTest, arg1, arg2) {
  /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_035: [** When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the `done` callback function with an error translated using the requirements detailed in `registry_http_errors_requirements.md`]*/
  it('calls the done callback with a proper error if an HTTP error occurs', function(testCallback) {
    var FakeHttpErrorHelper = {
      buildRequest: function (method, path, httpHeaders, host, done) {
        return {
          write: function() {},
          end: function() {
            done(new Error('Not found'), "{\"Message\":\"ErrorCode:DeviceNotFound;\\\"fake test error message\\\"\"}", { statusCode: 404 });
          }
        };
      }
    };

    var registry = new Registry(fakeConfig, FakeHttpErrorHelper);
    var callback = function(err, result, response) {
      assert.instanceOf(err, errors.DeviceNotFoundError);
      assert.isUndefined(result);
      assert.isUndefined(response);
      testCallback();
    };

    if (arg1 && arg2) {
      registry[methodUnderTest](arg1, arg2, callback);
    } else if (arg1 && !arg2) {
      registry[methodUnderTest](arg1, callback);
    } else {
      registry[methodUnderTest](callback);
    }
  });

  /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_033: [If any registry operation method encounters an error before it can send the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (err.message).]*/
  it('calls the done callback with a standard error if not an HTTP error', function(testCallback) {
    var FakeGenericErrorHelper = {
      buildRequest: function (method, path, httpHeaders, host, done) {
        return {
          write: function() {},
          end: function() {
            done(new Error('Fake Error'));
          }
        };
      }
    };

    var registry = new Registry(fakeConfig, FakeGenericErrorHelper);
    var callback = function(err, result, response) {
      assert.instanceOf(err, Error);
      assert.isUndefined(result);
      assert.isUndefined(response);
      testCallback();
    };

    if (arg1 && arg2) {
      registry[methodUnderTest](arg1, arg2, callback);
    } else if (arg1 && !arg2) {
      registry[methodUnderTest](arg1, callback);
    } else {
      registry[methodUnderTest](callback);
    }
  });
}


describe('Registry', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_023: [The `Registry` constructor shall throw a `ReferenceError` if the config object is falsy.]*/
    [undefined, null].forEach(function(badConfig) {
      it('throws if \'config\' is \'' + badConfig + '\'', function() {
        assert.throws(function() {
          return new Registry(badConfig);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_001: [The `Registry` constructor shall throw an `ArgumentException` if the config object is missing one or more of the following properties:
    - `host`: the IoT Hub hostname
    - `sharedAccessSignature`: shared access signature with the permissions for the desired operations.]*/
    [undefined, null, ''].forEach(function(badConfigProperty) {
      it('throws if \'config.host\' is \'' + badConfigProperty + '\'', function() {
        assert.throws(function() {
          return new Registry({host: badConfigProperty, sharedAccessSignature: 'sharedAccessSignature'});
        }, errors.ArgumentError);
      });

      it('throws if \'config.sharedAccessSignature\' is \'' + badConfigProperty + '\'', function() {
        assert.throws(function() {
          return new Registry({host: 'host', sharedAccessSignature: badConfigProperty});
        }, errors.ArgumentError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_024: [The `Registry` constructor shall use the `httpHelper` provided as a second argument if it is provided.]*/
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_025: [The `Registry` constructor shall use `azure-iot-http-base.Http` if no `httpHelper` argument is provided.]*/
  });

  describe('#fromConnectionString', function() {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_008: [The `fromConnectionString` method shall throw `ReferenceError` if the value argument is falsy.]*/
    [undefined, null, ''].forEach(function(falsyConnectionString) {
      it('throws if \'value\' is \'' + falsyConnectionString + '\'', function() {
        assert.throws(function() {
          return Registry.fromConnectionString(falsyConnectionString);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_009: [The `fromConnectionString` method shall derive and transform the needed parts from the connection string in order to create a `config` object for the constructor (see `SRS_NODE_IOTHUB_REGISTRY_05_001`).]*/
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_010: [The `fromConnectionString` method shall return a new instance of the `Registry` object.]*/
    it('returns a new instance of the Registry object', function() {
      var registry = Registry.fromConnectionString('HostName=a.b.c;SharedAccessKeyName=name;SharedAccessKey=key');
      assert.instanceOf(registry, Registry);
    });
  });

  describe('#fromSharedAccessSignature', function() {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_011: [The `fromSharedAccessSignature` method shall throw `ReferenceError` if the value argument is falsy.]*/
    [undefined, null, ''].forEach(function(falsySas) {
      it('throws if \'value\' is \'' + falsySas + '\'', function() {
        assert.throws(function() {
          return Registry.fromSharedAccessSignature(falsySas);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_012: [The `fromSharedAccessSignature` method shall derive and transform the needed parts from the shared access signature in order to create a `config` object for the constructor (see `SRS_NODE_IOTHUB_REGISTRY_05_001`).]*/
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_013: [The `fromSharedAccessSignature` method shall return a new instance of the `Registry` object.]*/
    it('returns a new instance of the Registry object', function() {
      var registry = Registry.fromSharedAccessSignature('SharedAccessSignature sr=audience&sig=signature&se=expiry&skn=keyname');
      assert.instanceOf(registry, Registry);
    });
  });

  describe('#create', function(){
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_001: [The `create` method shall throw `ReferenceError` if the `deviceInfo` argument is falsy.]*/
    [undefined, null].forEach(function(badDeviceInfo) {
      testFalsyArg('create', 'deviceInfo', badDeviceInfo, ReferenceError);
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_001: [The `create` method shall throw `ArgumentError` if the `deviceInfo` argument does not contain a `deviceId` property.]*/
    [undefined, null, ''].forEach(function(badDeviceId) {
      testFalsyArg('create', 'deviceInfo', { deviceId: badDeviceId }, errors.ArgumentError);
    });

    testErrorCallback('create', fakeDevice);

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_026: [** The `create` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    PUT /devices/<deviceInfo.deviceId>?api-version=<version> HTTP/1.1
    Authorization: <sharedAccessSignature>
    Content-Type: application/json; charset=utf-8
    Request-Id: <guid>

    <deviceInfo>
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeHttpResponse = { statusCode: 200 };
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'PUT');
          assert.equal(path, '/devices/' + fakeDevice.deviceId + endpoint.versionQueryString());
          assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows tracing of requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function(body) {
              assert.equal(body, JSON.stringify(fakeDevice));
            },
            end: function() {
              done(null, JSON.stringify(fakeDevice), fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.create(fakeDevice, function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.deepEqual(result, fakeDevice);
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });

  describe('#update', function(){
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_043: [The `update` method shall throw `ReferenceError` if the `deviceInfo` argument is falsy.]*/
    [undefined, null].forEach(function(badDeviceInfo) {
      testFalsyArg('update', 'deviceInfo', badDeviceInfo, ReferenceError);
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_003: [The `update` method shall throw ArgumentError if the first argument does not contain a deviceId property.]*/
    [undefined, null, ''].forEach(function(badDeviceId) {
      testFalsyArg('update', 'deviceInfo', { deviceId: badDeviceId }, errors.ArgumentError);
    });

    testErrorCallback('update', fakeDevice);

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_027: [** The `update` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    PUT /devices/<deviceInfo.deviceId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Content-Type: application/json; charset=utf-8
    Request-Id: <guid>

    <deviceInfo>
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeHttpResponse = { statusCode: 200 };
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'PUT');
          assert.equal(path, '/devices/' + fakeDevice.deviceId + endpoint.versionQueryString());
          assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function(body) {
              assert.equal(body, JSON.stringify(fakeDevice));
            },
            end: function() {
              done(null, JSON.stringify(fakeDevice), fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.update(fakeDevice, function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.deepEqual(result, fakeDevice);
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });

  describe('#get', function(){
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_05_006: [The `get` method shall throw `ReferenceError` if the supplied deviceId is falsy.]*/
    [undefined, null, ''].forEach(function(badDeviceId) {
      testFalsyArg('get', 'deviceId', badDeviceId, ReferenceError);
    });

    testErrorCallback('get', fakeDevice.deviceId);

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_028: [** The `get` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    GET /devices/<deviceInfo.deviceId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Request-Id: <guid>
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeHttpResponse = { statusCode: 200 };
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'GET');
          assert.equal(path, '/devices/' + fakeDevice.deviceId + endpoint.versionQueryString());
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function() {
              assert.fail();
            },
            end: function() {
              done(null, JSON.stringify(fakeDevice), fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.get(fakeDevice.deviceId, function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.deepEqual(result, fakeDevice);
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });

  describe('#list', function() {
    testErrorCallback('list');

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_029: [** The `list` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    GET /devices?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Request-Id: <guid>
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeHttpResponseBody = [
        { deviceId: 'device1' },
        { deviceId: 'device2' }
      ];
      var fakeHttpResponse = { statusCode: 204 };
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'GET');
          assert.equal(path, '/devices/' + endpoint.versionQueryString());
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function() {
              assert.fail();
            },
            end: function() {
              done(null, JSON.stringify(fakeHttpResponseBody), fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.list(function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.deepEqual(result, fakeHttpResponseBody);
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });

  describe('#delete', function() {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_07_007: [The `delete` method shall throw `ReferenceError` if the supplied deviceId is falsy.]*/
    [undefined, null, ''].forEach(function(badDeviceId) {
      testFalsyArg('delete', 'deviceId', badDeviceId, ReferenceError);
    });

    testErrorCallback('delete', fakeDevice.deviceId);

    /**SRS_NODE_IOTHUB_REGISTRY_16_030: [** The `delete` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    DELETE /devices/<deviceInfo.deviceId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    If-Match: *
    Request-Id: <guid>
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeHttpResponse = { statusCode: 204 };
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'DELETE');
          assert.equal(path, '/devices/' + fakeDevice.deviceId + endpoint.versionQueryString());
          assert.equal(httpHeaders['If-Match'], '*');
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function() {
              assert.fail();
            },
            end: function() {
              done(null, '', fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.delete(fakeDevice.deviceId, function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.equal(result, '');
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });

  describe('#getDeviceTwin', function () {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_019: [The `getDeviceTwin` method shall throw a `ReferenceError` if the `deviceId` parameter is falsy.]*/
    [undefined, null, ''].forEach(function(badDeviceId) {
      testFalsyArg('getDeviceTwin', 'deviceId', badDeviceId, ReferenceError);
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_020: [The `getDeviceTwin` method shall throw a `ReferenceError` if the `done` parameter is falsy.]*/
    [undefined, null].forEach(function(badCallback) {
      it('throws a ReferenceError if \'done\' is \'' + badCallback + '\'', function() {
        var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
        assert.throws(function(){
          registry.getDeviceTwin('deviceId', badCallback);
        }, ReferenceError);
      });
    });

    testErrorCallback('getDeviceTwin', fakeDevice.deviceId);

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_036: [The `getDeviceTwin` method shall call the `done` callback with a `DeviceTwin` object updated with the latest property values stored in the IoT Hub servce.]*/
    it('calls the \'done\' a \'DeviceTwin\' object', function(testCallback) {
      var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' }, {
        buildRequest: function(method, path, headers, body, done) {
          return {
            write: function() {},
            end: function() {
              done(null, JSON.stringify({ deviceId: 'fakeTwin' }), { status: 200 });
            }
          };
        }
      });

      registry.getDeviceTwin('deviceId', function(err, twin) {
        assert.instanceOf(twin, DeviceTwin);
        testCallback();
      });
    });
  });

  describe('importDevicesFromBlob', function() {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_001: [A `ReferenceError` shall be thrown if `inputBlobContainerUri` is falsy]*/
    [undefined, null, ''].forEach(function(badUri) {
      it('throws a ReferenceError if inputBlobContainerUri is \'' + badUri + '\'', function() {
        var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
        assert.throws(function(){
          registry.importDevicesFromBlob(badUri, 'outputContainerUri', function() {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_002: [A `ReferenceError` shall be thrown if `outputBlobContainerUri` is falsy]*/
    [undefined, null, ''].forEach(function(badUri) {
      it('throws a ReferenceError if outputBlobContainerUri is \'' + badUri + '\'', function() {
        var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
        assert.throws(function(){
          registry.importDevicesFromBlob('inputContainerUri', badUri, function() {});
        }, ReferenceError);
      });
    });

    testErrorCallback('importDevicesFromBlob', 'input', 'output');

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_031: [The `importDeviceFromBlob` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    POST /jobs/create?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Content-Type: application/json; charset=utf-8 
    Request-Id: <guid>

    {
      "type": "import",
      "inputBlobContainerUri": "<input container Uri given as parameter>",
      "outputBlobContainerUri": "<output container Uri given as parameter>"
    }
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeHttpResponse = { statusCode: 200 };
      var fakeInputBlob = "input";
      var fakeOutputBlob = "output";
      var fakeResponseBody = {
        jobId: 42
      };
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'POST');
          assert.equal(path, '/jobs/create' + endpoint.versionQueryString());
          assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function(body) {
              var fakeRequestBody = {
                type: 'import',
                inputBlobContainerUri: fakeInputBlob,
                outputBlobContainerUri: fakeOutputBlob
              };
              assert.equal(body, JSON.stringify(fakeRequestBody));
            },
            end: function() {
              done(null, JSON.stringify(fakeResponseBody), fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.importDevicesFromBlob(fakeInputBlob, fakeOutputBlob, function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.deepEqual(result, fakeResponseBody);
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });

  describe('exportDevicesToBlob', function() {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_004: [A `ReferenceError` shall be thrown if outputBlobContainerUri is falsy]*/
    [undefined, null, ''].forEach(function(badUri) {
      it('throws a ReferenceError if outputBlobContainerUri is \'' + badUri + '\'', function() {
        var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
        assert.throws(function(){
          registry.exportDevicesToBlob(badUri, false, function() {});
        }, ReferenceError);
        assert.throws(function(){
          registry.exportDevicesToBlob(badUri, true, function() {});
        }, ReferenceError);
      });
    });

    testErrorCallback('exportDevicesToBlob', 'input', true);

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_032: [The `exportDeviceToBlob` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    POST /jobs/create?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Content-Type: application/json; charset=utf-8 
    Request-Id: <guid>

    {
      "type": "export",
      "outputBlobContainerUri": "<output container Uri given as parameter>",
      "excludeKeysInExport": "<excludeKeys Boolean given as parameter>"
    }
    ```]*/
    [true, false].forEach(function(fakeExcludeKeys) {
      it('constructs a valid HTTP request when excludeKeys is \'' + fakeExcludeKeys + '\'', function(testCallback) {
        var fakeHttpResponse = { statusCode: 200 };
        var fakeOutputBlob = "output";
        var fakeResponseBody = {
          jobId: 42
        };
        var fakeHttpHelper = {
          buildRequest: function (method, path, httpHeaders, host, done) {
            assert.equal(host, fakeConfig.host);
            assert.equal(method, 'POST');
            assert.equal(path, '/jobs/create' + endpoint.versionQueryString());
            assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
            /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
            assert.isString(httpHeaders['Request-Id']);
            /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
            assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
            /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
            assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
            return {
              write: function(body) {
                var fakeRequestBody = {
                  type: 'export',
                  outputBlobContainerUri: fakeOutputBlob,
                  excludeKeysInExport: fakeExcludeKeys
                };
                assert.equal(body, JSON.stringify(fakeRequestBody));
              },
              end: function() {
                done(null, JSON.stringify(fakeResponseBody), fakeHttpResponse);
              }
            };
          }
        };

        var registry = new Registry(fakeConfig, fakeHttpHelper);
        registry.exportDevicesToBlob(fakeOutputBlob, fakeExcludeKeys, function(err, result, response) {
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
          - `err`: `null`
          - `result`: A javascript object parsed from the body of the HTTP response
          - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
          assert.isNull(err);
          assert.deepEqual(result, fakeResponseBody);
          assert.equal(response, fakeHttpResponse);
          testCallback();
        });
      });
    });
  });

  describe('listJobs', function() {
    testErrorCallback('listJobs');

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_037: [The `listJobs` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    GET /jobs?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature> 
    Request-Id: <guid>
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeHttpResponseBody = [
        { jobId: 42 },
        { jobId: 43 }
      ];
      var fakeHttpResponse = { statusCode: 204 };
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'GET');
          assert.equal(path, '/jobs' + endpoint.versionQueryString());
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function() {
              assert.fail();
            },
            end: function() {
              done(null, JSON.stringify(fakeHttpResponseBody), fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.listJobs(function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.deepEqual(result, fakeHttpResponseBody);
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });

  describe('#getJob', function() {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_006: [A `ReferenceError` shall be thrown if jobId is falsy]*/
    [undefined, null, ''].forEach(function(badJobId) {
      testFalsyArg('getJob', 'jobId', badJobId, ReferenceError);
    });

    testErrorCallback('getJob', 'fakeJobId');  

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_038: [The `getJob` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    GET /jobs/<jobId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature> 
    Request-Id: <guid>
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeHttpResponse = { statusCode: 200 };
      var fakeJob = {
        jobId: '42'
      };
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'GET');
          assert.equal(path, '/jobs/' + fakeJob.jobId + endpoint.versionQueryString());
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function() {
              assert.fail();
            },
            end: function() {
              done(null, JSON.stringify(fakeJob), fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.getJob(fakeJob.jobId, function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.deepEqual(result, fakeJob);
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });

  describe('#cancelJob', function(){
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_012: [A `ReferenceError` shall be thrown if the jobId is falsy]*/
    [undefined, null, ''].forEach(function(badJobId) {
      testFalsyArg('cancelJob', 'jobId', badJobId, ReferenceError);
    });

    testErrorCallback('cancelJob', 'fakeJobId');

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_039: [The `cancelJob` method shall construct an HTTP request using information supplied by the caller as follows:
    ```
    DELETE /jobs/<jobId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Request-Id: <guid>
    ```]*/
    it('constructs a valid HTTP request', function(testCallback) {
      var fakeHttpResponse = { statusCode: 204 };
      var fakeJobId = '42';
      var fakeHttpHelper = {
        buildRequest: function (method, path, httpHeaders, host, done) {
          assert.equal(host, fakeConfig.host);
          assert.equal(method, 'DELETE');
          assert.equal(path, '/jobs/' + fakeJobId + endpoint.versionQueryString());
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_041: [All requests shall contain a `Request-Id` header that uniquely identifies the request and allows to trace requests/responses in the logs.]*/
          assert.isString(httpHeaders['Request-Id']);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_042: [All requests shall contain a `Authorization` header that contains a valid shared access key.]*/
          assert.equal(httpHeaders.Authorization, fakeConfig.sharedAccessSignature);
          /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_040: [All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used.]*/
          assert.equal(httpHeaders['User-Agent'], PackageJson.name + '/' + PackageJson.version);
          return {
            write: function() {
              assert.fail();
            },
            end: function() {
              done(null, '', fakeHttpResponse);
            }
          };
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.cancelJob(fakeJobId, function(err, result, response) {
        /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_034: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
        - `err`: `null`
        - `result`: A javascript object parsed from the body of the HTTP response
        - `response`: the Node.js `http.ServerResponse` object returned by the transport]*/
        assert.isNull(err);
        assert.equal(result, '');
        assert.equal(response, fakeHttpResponse);
        testCallback();
      });
    });
  });
});