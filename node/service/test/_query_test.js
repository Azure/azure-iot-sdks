// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var sinon = require('sinon');
var Query = require('../lib/query.js');
var Twin = require('../lib/twin.js');

describe('Query', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_SERVICE_QUERY_16_001: [The `Query`  shall throw a `ReferenceError` if `executeQueryFn` is falsy.]*/
    [undefined, null, ''].forEach(function (badFn) {
      it('throws a ReferenceError if executeQueryFn is \'' + badFn + '\'', function() {
        assert.throws(function(){
          return new Query(badFn);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_011: [The `Query` constructor shall throw a `TypeError` if `executeQueryFn` is not a function.]*/
    ['foo', 42, {}].forEach(function (badFn) {
      it('throws a TypeError if executeQueryFn is \'' + badFn + '\'', function() {
        assert.throws(function(){
          return new Query(badFn);
        }, TypeError);
      });
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_015: [The `Query` constructor shall initialize the `hasMoreResults` property to `true.]*/
    it('initializes Query.hasMoreResults to true', function() {
      var query = new Query(function() {});
      assert.isTrue(query.hasMoreResults);
    });
  });

  var nextCommonTests = function(nextName) {
    it('uses the previous continuationToken in the following query', function(testCallback) {
      var fakeContinuationToken = 'continuationToken';
      var fakeResponse = { 
        statusCode: 200,
        headers: { 
          'x-ms-continuation': fakeContinuationToken 
        }
      };
      var fakeExecuteFn = sinon.stub().callsArgWith(1, null, [], fakeResponse);

      var query = new Query(fakeExecuteFn);

      query[nextName](function() {
        assert(fakeExecuteFn.calledWith(null));
        query[nextName](function() {
          assert(fakeExecuteFn.calledWith(fakeContinuationToken));
          testCallback();
        });
      });
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_013: [The `next` method shall set the `Query.hasMoreResults` property to `true` if the `continuationToken` property of the result object is not `null`.]*/
    /*Tests_SRS_NODE_SERVICE_QUERY_16_014: [The `next` method shall set the `Query.hasMoreResults` property to `false` if the `continuationToken` property of the result object is `null`.]*/
    [{ token: 'a string', more: true }, { token: undefined, more: false }].forEach(function(testConfig) {
      it('sets hasMoreResults to \'' + testConfig.more + '\' if the continuationToken is \'' + testConfig.token + '\'', function(testCallback) {
        var fakeExecuteFn = sinon.stub().callsArgWith(1, null, [], { statusCode: 200, headers: { 'x-ms-continuation': testConfig.token } });

        var query = new Query(fakeExecuteFn);
        query[nextName](function() {
          assert.equal(query.hasMoreResults, testConfig.more);
          testCallback();
        });
      });
    });


    /*Tests_SRS_NODE_SERVICE_QUERY_16_008: [The `next` method shall call the `done` callback with a single argument that is an instance of the standard Javascript `Error` object if the request failed.]*/
    it('calls the done callback with an error if the request fails', function(testCallback) {
      var fakeError = new Error('fake');
      var fakeExecuteFn = sinon.stub().callsArgWith(1, fakeError);

      var query = new Query(fakeExecuteFn);
      query[nextName](function(err) {
        assert.equal(err, fakeError);
        testCallback();
      });
    });

  };

  describe('#next', function() {
    nextCommonTests('next');

    /*Tests_SRS_NODE_SERVICE_QUERY_16_007: [The `next` method shall call the `done` callback with a `null` error object, the results of the query and the response of the underlying transport if the request was successful.]*/
    it('calls the done callback with the results', function(testCallback) {
      var fakeResults = [
        { deviceId: 'deviceId1' },
        { deviceId: 'deviceId2' },
        { deviceId: 'deviceId3' }
      ];
      var fakeResponse = { 
        statusCode: 200,
        headers: { 
          'x-ms-continuation': null 
        }
      };
      var fakeExecuteFn = sinon.stub().callsArgWith(1, null, fakeResults, fakeResponse);

      var query = new Query(fakeExecuteFn);
      query.next(function(err, result, response) {
        assert.isNull(err);
        assert.equal(result, fakeResults);
        assert.equal(response, fakeResponse);
        testCallback();
      });
    });
  });

  describe('#nextAsTwin', function() {
    nextCommonTests('nextAsTwin');

    /*Tests_SRS_NODE_SERVICE_QUERY_16_009: [The `nextAsTwin` method shall call the `done` callback with a `null` error object and a collection of `Twin` objects created from the results of the query if the request was successful.]*/
    it('calls the done callback with the results converted to Twin', function(testCallback) {
      var fakeResults = [];
      for(var i = 0; i < 3; i++) {
        fakeResults.push({ deviceId: 'deviceId' + i });
      }

      var fakeResponse = { 
        statusCode: 200,
        headers: { 
          'x-ms-continuation': null 
        }
      };

      var fakeExecuteFn = sinon.stub().callsArgWith(1, null, fakeResults, fakeResponse);

      var query = new Query(fakeExecuteFn, {});
      query.nextAsTwin(function(err, result, response) {
        assert.isNull(err);
        assert.equal(response, fakeResponse);
        result.forEach(function(twin, index) {
          assert.instanceOf(twin, Twin);
          assert.equal(twin.deviceId, 'deviceId' + index);
        });
        testCallback();
      });
    });

    it('calls the done callback with null if the result of the query was null', function(testCallback) {
      var fakeResults = [];
      for(var i = 0; i < 3; i++) {
        fakeResults.push({ deviceId: 'deviceId' + i });
      }

      var fakeResponse = { 
        statusCode: 200,
        headers: { 
          'x-ms-continuation': null 
        }
      };

      var fakeExecuteFn = sinon.stub().callsArgWith(1, null, null, fakeResponse);

      var query = new Query(fakeExecuteFn, {});
      query.nextAsTwin(function(err, result, response) {
        assert.isNull(err);
        assert.equal(response, fakeResponse);
        assert.isNull(result);
        testCallback();
      });
    });
  });
});