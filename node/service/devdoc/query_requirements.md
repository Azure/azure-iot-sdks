# Query requirements

## Overview
The `Query` class is used to run SQL queries on the device twin store in order to retrieve devices with specific properties or job statuses.

## Usage
```js
var Registry = require('azure-iothub').Registry;

var connectionString = '<IOTHUB CONNECTION STRING>';
var registry = Registry.fromConnectionString(connectionString);

var query = registry.createQuery('SELECT * FROM devices', 100);
var onResults = function(err, results) {
  if (err) {
    console.error('Failed to fetch the results: ' + err.message);
  } else {
    // Do something with the results
    results.forEach(function(twin) {
      console.log(twin.deviceId);
    });

    if (query.hasMoreResults) {
        query.nextAsTwin(onResults);
    }
  }
};

query.nextAsTwin(onResults);
```

## Public Interface

### Query(sqlQuery, pageSize, registry) [constructor]
The `Query` constructor initializes a new instance of the `Query` class.

**SRS_NODE_SERVICE_QUERY_16_001: [** The `Query` constructor shall throw a `ReferenceError` if `sqlQuery` is falsy. **]**

**SRS_NODE_SERVICE_QUERY_16_002: [** The `Query` constructor shall throw a `ReferenceError` if `registry` is falsy. **]**

**SRS_NODE_SERVICE_QUERY_16_011: [** The `Query` constructor shall throw a `TypeError` if `sqlQuery` is not a string. **]**

**SRS_NODE_SERVICE_QUERY_16_012: [** The `Query` constructor shall throw a `TypeError` if `pageSize` is not a number, null or undefined. **]**

**SRS_NODE_SERVICE_QUERY_16_003: [** The `Query` constructor shall set the Query.sql property to the `sqlQuery` argument value. **]**

**SRS_NODE_SERVICE_QUERY_16_004: [** The `Query` constructor shall set the Query.pageSize property to the `pageSize` argument value. **]**

### next(done)
The `next` method runs the query and calls the `done` callback with a new page of results.

**SRS_NODE_SERVICE_QUERY_16_008: [** The `next` method shall call the `done` callback with a single argument that is an instance of the standard Javascript `Error` object if the request failed. **]**

### nextAsTwin(done)
The `nextAsTwin` method runs the query and call the `done` callback with an error a new page of results casted as device twins.

**SRS_NODE_SERVICE_QUERY_16_009: [** The `nextAsTwin` method shall call the `done` callback with a `null` error object and a collection of `Twin` objects created from the results of the query if the request was successful. **]**

### Common requirement for all next*() methods
**SRS_NODE_SERVICE_QUERY_16_005: [** The `next` method shall call the `Registry.executeQuery` method with a first parameter formatted as follows:
```
{
  sql: <Query.sqlQuery>,
  pageSize: <Query.pageSize>,
  continuationToken: <Query._continuationToken>
}
```
**]**

**SRS_NODE_SERVICE_QUERY_16_006: [** The `next` method shall set the `Query.continuationToken` property to the `continuationToken` value of the query result. **]**

**SRS_NODE_SERVICE_QUERY_16_013: [** The `next` method shall set the `Query.hasMoreResults` property to `true` if the `continuationToken` property of the result object is not `null`. **]**

**SRS_NODE_SERVICE_QUERY_16_014: [** The `next` method shall set the `Query.hasMoreResults` property to `false` if the `continuationToken` property of the result object is `null`. **]**

**SRS_NODE_SERVICE_QUERY_16_007: [** The `next` method shall call the `done` callback with a `null` error object, the results of the query and the response of the underlying transport if the request was successful. **]**
