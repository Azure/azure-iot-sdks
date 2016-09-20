// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var DeviceTwin = require('./device_twin.js');

/**
 * @class                  module:azure-iothub.Query
 * @classdesc              Constructs a Query object that provides APIs to trigger the execution of a device query.
 * @param {String}         sqlQuery     A SQL query.
 * @param {Number}         pageSize     The number of results per page 
 * @param {Registry}       registry     The Registry object used to execute API calls.
 */
function Query(sqlQuery, pageSize, registry) {
  /*Codes_SRS_NODE_SERVICE_QUERY_16_001: [The `Query` constructor shall throw a `ReferenceError` if `sqlQuery` is falsy.]*/
  if (!sqlQuery) throw new ReferenceError('sqlQuery cannot be \'' + sqlQuery + '\'');
  /*Codes_SRS_NODE_SERVICE_QUERY_16_011: [The `Query` constructor shall throw a `TypeError` if `sqlQuery` is not a string.]*/
  if (typeof sqlQuery !== 'string') throw new TypeError('sqlQuery must be a string');
  /*Codes_SRS_NODE_SERVICE_QUERY_16_012: [The `Query` constructor shall throw a `TypeError` if `pageSize` is not a number, null or undefined.]*/
  if (pageSize !== null && pageSize !== undefined && typeof pageSize !== 'number') throw new TypeError('pageSize must be null, undefined or a number.');
  /*Codes_SRS_NODE_SERVICE_QUERY_16_002: [The `Query` constructor shall throw a `ReferenceError` if `registry` is falsy.]*/
  if (!registry) throw new ReferenceError('registry cannot be \'' + registry + '\'');

  /*Codes_SRS_NODE_SERVICE_QUERY_16_003: [The `Query` constructor shall set the Query.sql property to the `sqlQuery` argument value.]*/
  this.sql = sqlQuery;

  /*Codes_SRS_NODE_SERVICE_QUERY_16_004: [The `Query` constructor shall set the Query.pageSize property to the `pageSize` argument value.]*/
  this.pageSize = pageSize;

  this._registry = registry;
  this.hasMoreResults = true;
  this._continuationToken = null;
}

/**
 * @method              module:azure-iothub.Query#next
 * @description         Gets the next page of results for this query.
 * @param {Function}    done       The callback that will be called with either an Error object or 
 *                                 the results of the query.
 */
Query.prototype.next = function(done) {
  var self = this;
  /*Codes_SRS_NODE_SERVICE_QUERY_16_005: [The `next` method shall call the `Registry.executeQuery` method with a first parameter formatted as follows:
  ```
  {
    sql: <Query.sqlQuery>,
    pageSize: <Query.pageSize>,
    continuationToken: <Query._continuationToken>
  }
  ```]*/
  self._registry.executeQuery(self.toJSON(), function(err, result, response) {
    if (err) {
      /*Codes_SRS_NODE_SERVICE_QUERY_16_008: [The `next` method shall call the `done` callback with a single argument that is an instance of the standard Javascript `Error` object if the request failed.]*/
      done(err);
    } else {
      /*Codes_SRS_NODE_SERVICE_QUERY_16_006: [The `next` method shall set the `Query.continuationToken` property to the `continuationToken` value of the query result.]*/
      self._continuationToken = result.continuationToken;

      /*Codes_SRS_NODE_SERVICE_QUERY_16_013: [The `next` method shall set the `Query.hasMoreResults` property to `true` if the `continuationToken` property of the result object is not `null`.]*/
      /*Codes_SRS_NODE_SERVICE_QUERY_16_014: [The `next` method shall set the `Query.hasMoreResults` property to `false` if the `continuationToken` property of the result object is `null`.]*/
      self.hasMoreResults = self._continuationToken !== null;

      /*Codes_SRS_NODE_SERVICE_QUERY_16_007: [The `next` method shall call the `done` callback with a `null` error object, the results of the query and the response of the underlying transport if the request was successful.]*/
      done(null, result.results, response);
    }
  });
};

/**
 * @method              module:azure-iothub.Query#nextAsTwin
 * @description         Gets the next page of results for this query and cast them as Twins.
 * @param {Function}    done       The callback that will be called with either an Error object or 
 *                                 the results of the query.
 */
Query.prototype.nextAsTwin = function(done) {
  var self = this;
  this.next(function(err, result, response) {
    if (err) {
      /*Codes_SRS_NODE_SERVICE_QUERY_16_008: [The `next` method shall call the `done` callback with a single argument that is an instance of the standard Javascript `Error` object if the request failed.]*/
      done(err);
    } else {
      if (result) {
        /*SRS_NODE_SERVICE_QUERY_16_009: [The `nextAsTwin` method shall call the `done` callback with a `null` error object and a collection of `Twin` objects created from the results of the query if the request was successful.]*/
        var twins = result.map(function(twinJson) {
          return new DeviceTwin(twinJson, self._registry);
        });
        done(null, twins, response);
      } else {
        /*Codes_SRS_NODE_SERVICE_QUERY_16_007: [The `next` method shall call the `done` callback with a `null` error object, the results of the query and the response of the underlying transport if the request was successful.]*/
        done(null, null, response);
      }
    }
  });
};

Query.prototype.toJSON = function() {
  return {
    sql: this.sql,
    pageSize: this.pageSize,
    continuationToken: this._continuationToken
  };
};

module.exports = Query;