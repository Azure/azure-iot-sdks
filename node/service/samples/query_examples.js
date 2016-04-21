// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var filterQuery = {
  filter: {
    logicalOperator: "and",
    filters: [
      {
        property: {
          name: "QosLevel",
          type: "service"
        },
        value: 1,
        comparisonOperator: "gt",
        type: "comparison"
      }
    ],
    type: "logical"
  },
  project: null,
  aggregate: null,
  sort: []
};

var sortQuery = {
  filter: null,
  project: null,
  aggregate: null,
  sort: [
      {
          order: "asc",
          property: {
              name: "CustomerId",
              type: "service"
          }
      }
  ]
};

var aggregateQuery = {
  filter: {
    logicalOperator: "and",
    filters: [
      {
        property: {
          name: "CustomerId",
          type: "service"
        },
        value: null,
        comparisonOperator: "ne",
        type: "comparison"
      }
    ],
    type: "logical"
  },
   project: null,
   aggregate: {
    keys: [
      {
        name: "CustomerId",
        type: "service"
      }
    ],
    properties: [
      {
        operator: "avg",
        property: {
          name: "QosLevel",
          type: "service"
        },
        columnName: "AvgQosLevel"
      }
    ]
  },
  sort: []
};

module.exports = {
  aggregateQuery: aggregateQuery,
  filterQuery: filterQuery,
  sortQuery: sortQuery
};