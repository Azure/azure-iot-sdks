// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Twin = require('./twin.js');

/**
 * @class                  module:azure-iothub.Query
 * @classdesc              Constructs a Query object that provides APIs to trigger the execution of a device query.
 * @param {Function}       executeQueryFn  The function that should be called to get a new page.
 * @param {Registry}       registry        [optional] Registry client used to create Twin objects (used in nextAsTwin()).
 */
function Query(executeQueryFn, registry) {
  if(!executeQueryFn) throw new ReferenceError('executeQueryFn cannot be \'' + executeQueryFn + '\'');
  if(typeof executeQueryFn !== 'function') throw new TypeError('executeQueryFn cannot be \'' + typeof executeQueryFn + '\'');

  this._executeQueryFn = executeQueryFn;
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
  self._executeQueryFn(self._continuationToken, function(err, result, response) {
    if (err) {
      /*Codes_SRS_NODE_SERVICE_QUERY_16_008: [The `next` method shall call the `done` callback with a single argument that is an instance of the standard Javascript `Error` object if the request failed.]*/
      done(err);
    } else {
      /*Codes_SRS_NODE_SERVICE_QUERY_16_006: [The `next` method shall set the `Query._continuationToken` property to the `continuationToken` value of the query result.]*/
      self._continuationToken = response.headers['x-ms-continuation'];

      /*Codes_SRS_NODE_SERVICE_QUERY_16_013: [The `next` method shall set the `Query.hasMoreResults` property to `true` if the `continuationToken` property of the result object is not `null`.]*/
      /*Codes_SRS_NODE_SERVICE_QUERY_16_014: [The `next` method shall set the `Query.hasMoreResults` property to `false` if the `continuationToken` property of the result object is `null`.]*/
      self.hasMoreResults = self._continuationToken !== undefined;

      /*Codes_SRS_NODE_SERVICE_QUERY_16_007: [The `next` method shall call the `done` callback with a `null` error object, the results of the query and the response of the underlying transport if the request was successful.]*/
      done(null, result, response);
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
          return new Twin(twinJson, self._registry);
        });
        done(null, twins, response);
      } else {
        /*Codes_SRS_NODE_SERVICE_QUERY_16_007: [The `next` method shall call the `done` callback with a `null` error object, the results of the query and the response of the underlying transport if the request was successful.]*/
        done(null, null, response);
      }
    }
  });
};

module.exports = Query;