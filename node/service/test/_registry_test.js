// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var Registry = require('../lib/registry.js');
var SimulatedHttp = require('./registry_http_simulated.js');
var runTests = require('./_registry_common_testrun.js');
var ArgumentError = require('azure-iot-common').errors.ArgumentError;

function bulkTests(Transport, goodConnectionString) {
  describe('Registry', function () {
    var testJobId = "";
    describe('#importDevicesFromBlob', function (done) {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_001: [A ReferenceError shall be thrown if inputBlobContainerUri is falsy]*/
      [undefined, null, "", 0].forEach(function (inputBlobUri) {
        it('throws when inputBlobContainerUri is ' + inputBlobUri, function () {
          var registry = Registry.fromConnectionString(goodConnectionString, Transport);
          assert.throws(function () {
            registry.importDevicesFromBlob(inputBlobUri, "foo", done);
          }, ReferenceError);
        });
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_002: [A ReferenceError shall be thrown if outputBlobContainerUri is falsy]*/
      [undefined, null, "", 0].forEach(function (outputBlobUri) {
        it('throws when outputBlobContainerUri is ' + outputBlobUri, function () {
          var registry = Registry.fromConnectionString(goodConnectionString, Transport);
          assert.throws(function () {
            registry.importDevicesFromBlob("foo", outputBlobUri, done);
          }, ReferenceError);
        });
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_003: [The ‘done’ callback shall be called with the job identifier as a parameter when the import job has been created]*/
      it('calls the done callback with a job identifier', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        registry.importDevicesFromBlob("foo", "bar", function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.isOk(res.jobId);
            done();
          }
        });
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_008: [The importDevicesFromBlob method should create a bulk import job using the transport associated with the Registry instance by giving it the correct URI path and an import request object as such:
      {
          'type': 'import',
          'inputBlobContainerUri': <input container Uri given as parameter>,
          'outputBlobContainerUri': <output container Uri given as parameter>
      }] */
      it('creates a correct import request', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        var inputUri = "foo";
        var outputUri = "bar";
        registry.importDevicesFromBlob(inputUri, outputUri, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.isOk(res.jobId);
            testJobId = res.jobId;
            assert.equal(res.type, 'import');
            assert.equal(res.inputBlobContainerUri, inputUri);
            assert.equal(res.outputBlobContainerUri, outputUri);
            done();
          }
        });
      });
    });

    describe('#exportDevicesToBlob', function (done) {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_004: [A ReferenceError shall be thrown if outputBlobContainerUri is falsy] */
      [undefined, null, "", 0].forEach(function (outputBlobUri) {
        it('throws when outputBlobContainerUri is ' + outputBlobUri, function () {
          var registry = Registry.fromConnectionString(goodConnectionString, Transport);
          assert.throws(function () {
            registry.exportDevicesToBlob(outputBlobUri, true, done);
          }, ReferenceError);
        });
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_005: [The ‘done’ callback shall be called with a null error parameter and the job status as a second parameter when the export job has been created] */
      it('calls the done callback with a job identifier', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);
        registry.exportDevicesToBlob("foo", true, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.isOk(res.jobId);
            done();
          }
        });
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_009: [The exportDevicesFromBlob method should create a bulk export job using the transport associated with the Registry instance by giving it the correct path URI and export request object as such:
      {
          'type': 'export',
          'outputBlobContainerUri': <output container Uri given as parameter>,
          'excludeKeysInExport': <excludeKeys Boolean given as parameter>
      }]*/
      [true, false].forEach(function (excludeKeys) {
        it('creates a correct export request when excludeKeys is ' + excludeKeys, function (done) {
          var registry = Registry.fromConnectionString(goodConnectionString, Transport);
          var outputUri = "bar";
          registry.exportDevicesToBlob(outputUri, excludeKeys, function (err, res) {
            if (err) {
              done(err);
            } else {
              assert.isOk(res.jobId);
              assert.equal(res.type, 'export');
              assert.equal(res.outputBlobContainerUri, outputUri);
              assert.equal(res.excludeKeysInExport, excludeKeys);
              done();
            }
          });
        });
      });
    });

    describe('#listJobs', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_016: [The listJobs method should request a list of active and recent bulk import/export jobs using the transport associated with the Registry instance and give it the correct path URI.] */
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_010: [The ‘done’ callback shall be called with a null error parameter and list of recent jobs as a second parameter if the request is successful.]*/
      it('calls the transport to request a list of jobs and succeeds', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);

        registry.listJobs(function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.isOk(res);
            done();
          }
        });
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_011: [The ‘done’ callback shall be called with only an error object if the request fails.]*/
      it('calls the transport to request a list of jobs and fails', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);

        if (registry._transport instanceof SimulatedHttp) {
          registry._transport.fakeFailure = true;

          registry.listJobs(function (err, res) {
            assert.isOk(err);
            assert.isUndefined(res);
            done();
          });
        } else {
          done();
        }
      });
    });

    describe('#getJob', function (done) {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_006: [A ReferenceError shall be thrown if jobId is falsy] */
      [undefined, null, "", 0].forEach(function (jobId) {
        it('throws when jobId is ' + jobId, function () {
          var registry = Registry.fromConnectionString(goodConnectionString, Transport);
          assert.throws(function () {
            registry.getJob(jobId, done);
          }, ReferenceError);
        });
      });

      /* Tests_SRS_NODE_IOTHUB_REGISTRY_16_017: [The getJob method should request status information of the bulk import/export job identified by the jobId parameter using the transport associated with the Registry instance and give it the correct path URI.]*/
      /* Tests_SRS_NODE_IOTHUB_REGISTRY_16_015: [The ‘done’ callback shall be called with only an error object if the request fails.]*/
      it('calls the transport to request a job status and succeeds', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);

        registry.getJob(testJobId, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.isOk(res);
            done();
          }
        });
      });

      /* Tests_SRS_NODE_IOTHUB_REGISTRY_16_007: [The ‘done’ callback shall be called with a null error parameter and the job status as second parameter if the request is successful.]*/
      it('calls the transport to request a job status and fails', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);

        registry.getJob("000-000", function (err, res) {
          assert.isOk(err);
          assert.isUndefined(res);
          done();
        });
      });
    });

    describe('#cancelJob', function (done) {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_012: [A ReferenceError shall be thrown if the jobId is falsy] */
      [undefined, null, "", 0].forEach(function (jobId) {
        it('throws when jobId is ' + jobId, function () {
          var registry = Registry.fromConnectionString(goodConnectionString, Transport);
          assert.throws(function () {
            registry.cancelJob(jobId, done);
          }, ReferenceError);
        });
      });

      /* Tests_SRS_NODE_IOTHUB_REGISTRY_16_018: [The cancelJob method should request cancel the job identified by the jobId parameter using the transport associated with the Registry instance by giving it the correct path URI.]*/
      /* Tests_SRS_NODE_IOTHUB_REGISTRY_16_013: [The ‘done’ callback shall be called with only an error object if the request fails.]*/
      it('calls the transport to cancel a job and succeeds', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);

        registry.cancelJob(testJobId, function (err) {
          assert.isUndefined(err);
          done();
        });
      });

      /* Tests_SRS_NODE_IOTHUB_REGISTRY_16_014: [The ‘done’ callback shall be called with no parameters if the request is successful.]*/
      it('calls the transport to cancel a job and fails', function (done) {
        var registry = Registry.fromConnectionString(goodConnectionString, Transport);

        registry.cancelJob("000-000", function (err) {
          assert.isOk(err);
          done();
        });
      });
    });

    describe('#queryDevicesByTags', function () {
        /* Tests_SRS_NODE_IOTHUB_REGISTRY_07_019: [A ReferenceError shall be thrown if the tags array is empty.] */
        [undefined, null, "", 0].forEach(function (tagList) {
            it('throws if the tagList is \'' + tagList + '\'', function () {
                var registry = Registry.fromConnectionString(goodConnectionString, Transport);
                assert.throws(function() {
                    registry.queryDevicesByTags(tagList, 10, function () {});    
                }, ArgumentError);
            });
        });

        /* Tests_SRS_NODE_IOTHUB_REGISTRY_07_023: [A ReferenceError shall be thrown if the maxCount is less than or equal to zero.] */
        [0, -1].forEach(function (maxCount) {
            it('throws if the maxCount is less then or equal to zero', function () {
                var tagList = ['tag1', 'tag2'];
                var registry = Registry.fromConnectionString(goodConnectionString, Transport);
                assert.throws(function () {
                    registry.queryDevicesByTags(tagList, maxCount, function () {});
                }, RangeError);
            });
        });

        /* Tests_SRS_NODE_IOTHUB_REGISTRY_07_020: [The queryDevicesByTags method shall call the server for the device that contain the tags] */
        /* Tests_SRS_NODE_IOTHUB_REGISTRY_07_022: [The JSON array returned from the service shall be converted to a list of Device objects.] */
        it('calls the transport to query a device with a specified tag', function (done) {
            var registry = Registry.fromConnectionString(goodConnectionString, Transport);
            var tagList = ['tag1', 'tag2'];
            registry.queryDevicesByTags(tagList, 10, function (err, deviceList, res) {
                if (err) {
                    done(err);
                } else {
                    assert.isOk(res);
                    assert.isArray(deviceList);
                    done();
                }
            });
        }); 
    });

    describe('#queryDevices', function () {
      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_019: [** A `ReferenceError` shall be thrown if the query object is falsy or empty.]*/
      [undefined, null, "", 0].forEach(function (testParam) {
          it('throws if the query is \'' + testParam + '\'', function () {
              var registry = Registry.fromConnectionString(goodConnectionString, Transport);
              assert.throws(function() {
                  registry.queryDevices(testParam, function () {});    
              }, ReferenceError);
          });
      });

      /*Tests_SRS_NODE_IOTHUB_REGISTRY_16_020: [** The `done` callback shall be called with an `Error` object if the request fails.]*/
      it('calls the done callback with an error object if the request fails', function(done) {
        var FailingTransport = function () { };
        FailingTransport.prototype.queryDevices = function (versionString, query, callback) {
          var error = new Error('fake query couldn\'t be completed');
          callback(error);
        };

        var registry = Registry.fromConnectionString(goodConnectionString, FailingTransport);
        var query = {
          project: null,
          aggregate: null,
          sort: null,
          filter: null
        };

        registry.queryDevices(query, function(err, result, response) {
          assert.instanceOf(err, Error);
          assert.isUndefined(result);
          assert.isUndefined(response);
          done();
        });
      });
    });
  });
}

function makeConnectionString(host, policy, key) {
  return 'HostName=' + host + ';SharedAccessKeyName=' + policy + ';SharedAccessKey=' + key;
}

var connectionString = makeConnectionString('host', 'policy', 'key');
var badConnStrings = [
  makeConnectionString('bad', 'policy', 'key'),
  makeConnectionString('host', 'bad', 'key'),
  makeConnectionString('host', 'policy', 'bad')
];
var deviceId = 'testDevice';

describe('Over simulated HTTPS', function () {
  runTests(SimulatedHttp, connectionString, badConnStrings, connectionString, badConnStrings, deviceId);
  bulkTests(SimulatedHttp, connectionString);
});
