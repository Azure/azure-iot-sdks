// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

"use strict";

var assert = require('chai').assert;
var sinon = require('sinon');
var endpoint = require('azure-iot-common').endpoint;
var errors = require('azure-iot-common').errors;
var Registry = require('../lib/registry.js');
var Twin = require('../lib/twin.js');
var Query = require('../lib/query.js');

var fakeDevice = { deviceId: 'deviceId' };
var fakeConfig = { host: 'host', sharedAccessSignature: 'sas' };

function testFalsyArg(methodUnderTest, argName, argValue, ExpectedErrorType) {
  var errorName = ExpectedErrorType ? ExpectedErrorType.name : 'Error';
  it('throws a ' + errorName + ' if \'' + argName + '\' is \'' + JSON.stringify(argValue) + '\' (type:' + typeof(argValue) + ')', function() {
    var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
    assert.throws(function() {
      registry[methodUnderTest](argValue, function() {});
    }, ExpectedErrorType);
  });
}

function testErrorCallback(methodUnderTest, arg1, arg2, arg3) {
  /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_035: [** When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the `done` callback function with an error translated using the requirements detailed in `registry_http_errors_requirements.md`]*/
  it('calls the done callback with a proper error if an HTTP error occurs', function(testCallback) {
    var FakeHttpErrorHelper = {
      executeApiCall: function (method, path, httpHeaders, body, done) {
        done(new errors.DeviceNotFoundError('Not found'));
      }
    };

    var registry = new Registry(fakeConfig, FakeHttpErrorHelper);
    var callback = function(err, result, response) {
      assert.instanceOf(err, errors.DeviceNotFoundError);
      assert.isUndefined(result);
      assert.isUndefined(response);
      testCallback();
    };

    if (arg1 && arg2 && arg3) {
      registry[methodUnderTest](arg1, arg2, arg3, callback);
    } else if (arg1 && arg2) {
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
      executeApiCall: function (method, path, httpHeaders, body, done) {
        done(new Error('Fake Error'));
      }
    };

    var registry = new Registry(fakeConfig, FakeGenericErrorHelper);
    var callback = function(err, result, response) {
      assert.instanceOf(err, Error);
      assert.isUndefined(result);
      assert.isUndefined(response);
      testCallback();
    };

    if (arg1 && arg2 && arg3) {
      registry[methodUnderTest](arg1, arg2, arg3, callback);
    } else if (arg1 && arg2) {
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
      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'PUT');
          assert.equal(path, '/devices/' + fakeDevice.deviceId + endpoint.versionQueryString());
          assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
          assert.equal(body, fakeDevice);

          done();
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.create(fakeDevice, testCallback);
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
      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'PUT');
          assert.equal(path, '/devices/' + fakeDevice.deviceId + endpoint.versionQueryString());
          assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
          assert.equal(body, fakeDevice);
          done();
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.update(fakeDevice, testCallback);
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
      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'GET');
          assert.equal(path, '/devices/' + fakeDevice.deviceId + endpoint.versionQueryString());
          done();
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.get(fakeDevice.deviceId, testCallback);
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
      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'GET');
          assert.equal(path, '/devices/' + endpoint.versionQueryString());
          done();
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.list(testCallback);
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
      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'DELETE');
          assert.equal(path, '/devices/' + fakeDevice.deviceId + endpoint.versionQueryString());
          assert.equal(httpHeaders['If-Match'], '*');
          done();
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.delete(fakeDevice.deviceId, testCallback);
    });
  });

  describe('#getTwin', function () {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_019: [The `getTwin` method shall throw a `ReferenceError` if the `deviceId` parameter is falsy.]*/
    [undefined, null, ''].forEach(function(badDeviceId) {
      testFalsyArg('getTwin', 'deviceId', badDeviceId, ReferenceError);
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_020: [The `getTwin` method shall throw a `ReferenceError` if the `done` parameter is falsy.]*/
    [undefined, null].forEach(function(badCallback) {
      it('throws a ReferenceError if \'done\' is \'' + badCallback + '\'', function() {
        var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
        assert.throws(function(){
          registry.getTwin('deviceId', badCallback);
        }, ReferenceError);
      });
    });

    testErrorCallback('getTwin', fakeDevice.deviceId);

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_036: [The `getTwin` method shall call the `done` callback with a `Twin` object updated with the latest property values stored in the IoT Hub servce.]*/
    it('calls the \'done\' callback with a \'Twin\' object', function(testCallback) {
      var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' }, {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          done(null, { deviceId: 'fakeTwin' }, { status: 200 });
        }
      });

      registry.getTwin('deviceId', function(err, twin) {
        assert.instanceOf(twin, Twin);
        testCallback();
      });
    });
  });

  describe('updateTwin', function() {
    /*Codes_SRS_NODE_IOTHUB_REGISTRY_16_044: [The `updateTwin` method shall throw a `ReferenceError` if the `deviceId` argument is `undefined`, `null` or an empty string.]*/
    [undefined, null, ''].forEach(function(badDeviceId) {
      it('throws a \'ReferenceError\' if \'deviceId\' is \'' + badDeviceId + '\'', function() {
        var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
        assert.throws(function() {
          registry.updateTwin(badDeviceId, {}, 'etag==', function() {});
        }, ReferenceError);
      });
    });

    /*Codes_SRS_NODE_IOTHUB_REGISTRY_16_045: [The `updateTwin` method shall throw a `ReferenceError` if the `patch` argument is falsy.]*/
    [undefined, null].forEach(function(badPatch) {
      it('throws a \'ReferenceError\' if \'patch\' is \'' + badPatch + '\'', function() {
        var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
        assert.throws(function() {
          registry.updateTwin('deviceId', badPatch, 'etag==', function() {});
        }, ReferenceError);
      });
    });

    /*Codes_SRS_NODE_IOTHUB_REGISTRY_16_046: [The `updateTwin` method shall throw a `ReferenceError` if the `etag` argument is falsy.]*/
    [undefined, null, ''].forEach(function(badEtag) {
      it('throws a \'ReferenceError\' if \'etag\' is \'' + badEtag + '\'', function() {
        var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' });
        assert.throws(function() {
          registry.updateTwin('deviceId', {}, badEtag, function() {});
        }, ReferenceError);
      });
    });

    /*Codes_SRS_NODE_IOTHUB_REGISTRY_16_048: [The `updateTwin` method shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    PATCH /twins/<deviceId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Content-Type: application/json; charset=utf-8
    Request-Id: <guid>
    If-Match: <etag>

    <patch>
    ```]*/
    it('creates a valid HTTP request', function(testCallback) {
      var fakeDeviceId = 'deviceId';
      var fakeTwinPatch = {
        tags: {
          foo: 'bar'
        }
      };
      var fakeHttpResponse = { statusCode: 200 };
      var fakeEtag = 'etag==';

      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'PATCH');
          assert.equal(path, '/twins/' + fakeDeviceId + endpoint.versionQueryString());
          assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
          assert.equal(httpHeaders['If-Match'], fakeEtag);
          assert.equal(body, fakeTwinPatch);
          done(null, new Twin(fakeDeviceId, {}), fakeHttpResponse);
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.updateTwin(fakeDeviceId, fakeTwinPatch, fakeEtag, testCallback);
    });

    /*Codes_SRS_NODE_IOTHUB_REGISTRY_16_050: [The `updateTwin` method shall call the `done` callback with a `Twin` object updated with the latest property values stored in the IoT Hub service.]*/
    it('calls the \'done\' a \'Twin\' object', function(testCallback) {
      var registry = new Registry({ host: 'host', sharedAccessSignature: 'sas' }, {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          done(null, JSON.stringify({ deviceId: 'fakeTwin' }), { status: 200 });
        }
      });

      registry.updateTwin('deviceId', {}, 'etag==', function(err, twin) {
        assert.instanceOf(twin, Twin);
        testCallback();
      });
    });

    testErrorCallback('updateTwin', 'deviceId', {}, 'etag==');
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
      var fakeInputBlob = "input";
      var fakeOutputBlob = "output";
      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'POST');
          assert.equal(path, '/jobs/create' + endpoint.versionQueryString());
          assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
          assert.equal(body.type, 'import');
          assert.equal(body.inputBlobContainerUri, fakeInputBlob);
          assert.equal(body.outputBlobContainerUri, fakeOutputBlob);
          done();
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.importDevicesFromBlob(fakeInputBlob, fakeOutputBlob, testCallback);
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
        var fakeOutputBlob = "output";
        var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
            assert.equal(method, 'POST');
            assert.equal(path, '/jobs/create' + endpoint.versionQueryString());
            assert.equal(httpHeaders['Content-Type'], 'application/json; charset=utf-8');
            assert.equal(body.type, 'export');
            assert.equal(body.outputBlobContainerUri, fakeOutputBlob);
            assert.equal(body.excludeKeysInExport, fakeExcludeKeys);
            done();
          }
        };

        var registry = new Registry(fakeConfig, fakeHttpHelper);
        registry.exportDevicesToBlob(fakeOutputBlob, fakeExcludeKeys, testCallback);
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
      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'GET');
          assert.equal(path, '/jobs' + endpoint.versionQueryString());
          done();
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.listJobs(testCallback);
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
      var fakeJob = {
        jobId: '42'
      };
      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'GET');
          assert.equal(path, '/jobs/' + fakeJob.jobId + endpoint.versionQueryString());
          done();
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.getJob(fakeJob.jobId, testCallback);
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
      var fakeJobId = '42';
      var fakeHttpHelper = {
        executeApiCall: function (method, path, httpHeaders, body, done) {
          assert.equal(method, 'DELETE');
          assert.equal(path, '/jobs/' + fakeJobId + endpoint.versionQueryString());
          done();
        }
      };

      var registry = new Registry(fakeConfig, fakeHttpHelper);
      registry.cancelJob(fakeJobId, testCallback);
    });
  });

  describe('#createQuery', function() {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_051: [The `createQuery` method shall throw a `ReferenceError` if the `sqlQuery` argument is falsy.]*/
    [undefined, null, ''].forEach(function(badSql) {
      testFalsyArg('createQuery', 'sqlQuery', badSql, ReferenceError);
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_052: [The `createQuery` method shall throw a `TypeError` if the `sqlQuery` argument is not a string.]*/
    [42, {}, function() {}].forEach(function(badSql) {
      testFalsyArg('createQuery', 'sqlQuery', badSql, TypeError);
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_053: [The `createQuery` method shall throw a `TypeError` if the `pageSize` argument is not `null`, `undefined` or a number.]*/
    ['foo', {}, function() {}].forEach(function(badPageSize) {
      it('throws a TypeError if pageSize is of type \'' + typeof(badPageSize) + '\'', function() {
        var registry = new Registry(fakeConfig, {});
        assert.throws(function() {
          registry.createQuery('foo', badPageSize);
        }, TypeError);
      });
    });

    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_054: [The `createQuery` method shall return a new `Query` instance initialized with the `sqlQuery` and the `pageSize` argument if specified.]*/
    it('returns a Query instance', function() {
      var registry = new Registry(fakeConfig, {});
      var query = registry.createQuery('SELECT * FROM devices', 42);
      assert.instanceOf(query, Query);
    });
  });

  describe('#_executeQueryFunc', function() {
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_057: [The `executeQuery` method shall construct an HTTP request as follows:
    ```
    POST /devices/query?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Content-Type: application/json; charset=utf-8
    x-ms-continuation: <continuationToken>
    x-ms-max-item-count: <pageSize>
    Request-Id: <guid>

    <query>
    ```]*/
    [{
      fakePageSize: 42,
      fakeContinuationToken: null
    },{
      fakePageSize: undefined,
      fakeContinuationToken: null
    },{
      fakePageSize: undefined,
      fakeContinuationToken: 'continuation'
    },{
      fakePageSize: 42,
      fakeContinuationToken: 'continuation'
    }].forEach(function(testConfig){
      it('constructs a valid HTTP request with pageSize: \'' + testConfig.fakePageSize + '\' and continuation token: \'' + testConfig.fakeContinuationToken + '\'', function() {
        var fakeSql = 'sql';
        var fakePageSize = testConfig.fakePageSize;
        var fakeQuery = {
          query: fakeSql
        };

        var fakeHttpHelper = { executeApiCall: sinon.stub().callsArgWith(4, null, [], { statusCode: 200, headers: { 'x-ms-continuation': testConfig.fakeContinuationToken }}) };

        var registry = new Registry(fakeConfig, fakeHttpHelper);
        var query = registry.createQuery(fakeSql, fakePageSize);
        query.next(function() {
          query.next(function() {});
        });
        assert.strictEqual(fakeHttpHelper.executeApiCall.args[0][0], 'POST');
        assert.strictEqual(fakeHttpHelper.executeApiCall.args[0][1], '/devices/query' + endpoint.versionQueryString());
        assert.strictEqual(fakeHttpHelper.executeApiCall.args[0][2]['Content-Type'], 'application/json; charset=utf-8');

        if (testConfig.fakePageSize) {
          assert.strictEqual(fakeHttpHelper.executeApiCall.args[0][2]['x-ms-max-item-count'], testConfig.fakePageSize);
        } else {
          assert.isUndefined(fakeHttpHelper.executeApiCall.args[0][2]['x-ms-max-item-count']);
        }


        if (testConfig.fakeContinuationToken) {
          assert.strictEqual(fakeHttpHelper.executeApiCall.args[1][2]['x-ms-continuation'], testConfig.fakeContinuationToken);
        } else {
          assert.isUndefined(fakeHttpHelper.executeApiCall.args[1][2]['x-ms-continuation']);
        }

        assert.deepEqual(fakeHttpHelper.executeApiCall.args[0][3], fakeQuery);
      });
    });
  });
});