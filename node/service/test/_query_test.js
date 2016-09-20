// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var sinon = require('sinon');
var Query = require('../lib/query.js');
var DeviceTwin = require('../lib/device_twin.js');

describe('Query', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_SERVICE_QUERY_16_001: [The `Query` constructor shall throw a `ReferenceError` if `sqlQuery` is falsy.]*/
    [undefined, null, ''].forEach(function(badSqlQuery) {
      it('throws a ReferenceError when sqlQuery is \'' + badSqlQuery + '\'', function() {
        assert.throws(function() {
          return new Query(badSqlQuery, 100, {});
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_011: [The `Query` constructor shall throw a `TypeError` if `sqlQuery` is not a string.]*/
    [42, {}, function() {}].forEach(function(badSqlQuery) {
      it('throws a TypeError when the type of sqlQuery is \'' + typeof(badSqlQuery) + '\'', function() {
        assert.throws(function() {
          return new Query(badSqlQuery, 100, {});
        }, TypeError);
      });
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_012: [The `Query` constructor shall throw a `TypeError` if `pageSize` is not a number, null or undefined.]*/
    ['foo', {}, function() {}].forEach(function(badPageSize) {
      it('throws a TypeError when the type of pageSize is \'' + typeof(badPageSize) + '\'', function() {
        assert.throws(function() {
          return new Query('SQL', badPageSize, {});
        }, TypeError);
      });
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_002: [The `Query` constructor shall throw a `ReferenceError` if `registry` is falsy.]*/
    [undefined, null].forEach(function(badRegistry) {
      it('throws a ReferenceError when registry is \'' + badRegistry + '\'', function() {
        assert.throws(function() {
          return new Query('SQL', 100, badRegistry);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_003: [The `Query` constructor shall set the Query.sql property to the `sqlQuery` argument value.]*/
    it('sets the sql property to the value of the sqlQuery argument', function() {
      var fakeSql = 'fake sql';
      var query = new Query(fakeSql, 100, {});
      assert.equal(JSON.parse(JSON.stringify(query)).sql, fakeSql);
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_004: [The `Query` constructor shall set the Query.pageSize property to the `pageSize` argument value.]*/
    it('sets the pageSize property to the value of the pageSize argument', function() {
      var pageSize = 1337;
      var query = new Query('fakeSql', pageSize, {});
      assert.equal(JSON.parse(JSON.stringify(query)).pageSize, pageSize);
    });

    [undefined, null].forEach(function(pageSize) {
      it('sets the pageSize property to \'null\' if the pageSize argument is \'' + pageSize + '\'', function() {
      var query = new Query('fakeSql', pageSize, {});
      assert.equal(JSON.parse(JSON.stringify(query)).pageSize, null);
      });
    });
  });

  var nextCommonTests = function(nextName) {
    /*Tests_SRS_NODE_SERVICE_QUERY_16_005: [The `next` method shall call the `Registry.executeQuery` method with a first parameter formatted as follows:
    ```
    {
      sql: <Query.sqlQuery>,
      pageSize: <Query.pageSize>,
      continuationToken: <Query._continuationToken>
    }
    ```]*/
    it('calls Registry.executeQuery with the correct arguments', function() {
      var fakeSql = 'SELECT * FROM devices';
      var fakePageSize = 1337;
      var fakeRegistry = {
        executeQuery: sinon.spy()
      };

      var query = new Query(fakeSql, fakePageSize, fakeRegistry);
      query[nextName](function() {});
      assert.equal(fakeRegistry.executeQuery.args[0][0].sql, fakeSql);
      assert.equal(fakeRegistry.executeQuery.args[0][0].pageSize, fakePageSize);
      assert.equal(fakeRegistry.executeQuery.args[0][0].continuationToken, null);
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_006: [The `next` method shall set the `Query.continuationToken` property to the `continuationToken` value of the query result.]*/
    it('saves the continuationToken', function(testCallback) {
      var fakeContinuationToken = 'continuationToken';
      var fakeRegistry = {
        executeQuery: sinon.stub().callsArgWith(1, null, { Result: [], ContinuationToken: fakeContinuationToken }, { statusCode: 200 })
      };

      var query = new Query('SELECT * FROM devices', 1337, fakeRegistry);
      query[nextName](function() {
        assert.equal(JSON.parse(JSON.stringify(query)).continuationToken, fakeContinuationToken);
        testCallback();
      });
    });

    it('uses the previous continuationToken in the following query', function(testCallback) {
      var fakeContinuationToken = 'fakeContinuationToken';
      var fakeRegistry = {
        executeQuery: sinon.stub().callsArgWith(1, null, { Result: [], ContinuationToken: fakeContinuationToken }, { statusCode: 200 })
      };

      var query = new Query('SELECT * FROM devices', 1337, fakeRegistry);
      query[nextName](function() {
        assert.equal(fakeRegistry.executeQuery.firstCall.args[0].continuationToken, null);
        query[nextName](function() {
          assert.equal(fakeRegistry.executeQuery.secondCall.args[0].continuationToken, fakeContinuationToken);
          testCallback();
        });
      });
    });

    /*Tests_SRS_NODE_SERVICE_QUERY_16_013: [The `next` method shall set the `Query.hasMoreResults` property to `true` if the `continuationToken` property of the result object is not `null`.]*/
    /*Tests_SRS_NODE_SERVICE_QUERY_16_014: [The `next` method shall set the `Query.hasMoreResults` property to `false` if the `continuationToken` property of the result object is `null`.]*/
    [{ token: 'a string', more: true }, { token: null, more: false }].forEach(function(testConfig) {
      it('sets hasMoreResults to \'' + testConfig.more + '\' if the continuationToken is \'' + testConfig.token + '\'', function(testCallback) {
        var fakeRegistry = {
          executeQuery: sinon.stub().callsArgWith(1, null, { Result: [], ContinuationToken: testConfig.token }, { statusCode: 200 })
        };

        var query = new Query('SELECT * FROM devices', 1337, fakeRegistry);
        query[nextName](function() {
          assert.equal(query.hasMoreResults, testConfig.more);
          testCallback();
        });
      });
    });


    /*Tests_SRS_NODE_SERVICE_QUERY_16_008: [The `next` method shall call the `done` callback with a single argument that is an instance of the standard Javascript `Error` object if the request failed.]*/
    it('calls the done callback with an error if the request fails', function(testCallback) {
      var fakeError = new Error('fake');
      var fakeRegistry = {
        executeQuery: sinon.stub().callsArgWith(1, fakeError)
      };

      var query = new Query('SELECT * FROM devices', 1337, fakeRegistry);
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
      var fakeResponse = { statusCode: 200 };
      var fakeRegistry = {
        executeQuery: sinon.stub().callsArgWith(1, null, { Result: fakeResults, ContinuationToken: null }, fakeResponse)
      };

      var query = new Query('SELECT * FROM devices', 1337, fakeRegistry);
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

      var fakeResponse = { statusCode: 200 };
      var fakeRegistry = {
        executeQuery: sinon.stub().callsArgWith(1, null, { Result: fakeResults, ContinuationToken: null }, fakeResponse)
      };

      var query = new Query('SELECT * FROM devices', 1337, fakeRegistry);
      query.nextAsTwin(function(err, result, response) {
        assert.isNull(err);
        assert.equal(response, fakeResponse);
        result.forEach(function(twin, index) {
          assert.instanceOf(twin, DeviceTwin);
          assert.equal(twin.deviceId, 'deviceId' + index);
        });
        testCallback();
      });
    });
  });
});