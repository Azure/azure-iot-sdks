// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var filterQuery = {
  filter: {
    property: {
      name: "CustomerId",
      type: "service"
    },
    value: "123456",
    comparisonOperator: "eq",
    type: "comparison"
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
              name: "QoS",
              type: "service"
          }
      }
  ]
};

var aggregateQuery = {
  filter: null,
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
        operator: "sum",
        property: {
          name: "Weight",
          type: "service"
        },
        columnName: "TotalWeight"
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