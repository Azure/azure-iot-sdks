// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Registry = require('azure-iothub').Registry;

var connectionString = "[IoT Hub Connection String]";
var sqlQuery = 'SELECT * FROM devices';

var registry = Registry.fromConnectionString(connectionString);

registry.queryTwins(sqlQuery, function(err, result) {
  if (err) {
    console.error('Cannot query twins: ' + err.message);
  } else {
    console.log(JSON.stringify(result, null, 2));
  }
});