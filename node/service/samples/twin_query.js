// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
'use strict';

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