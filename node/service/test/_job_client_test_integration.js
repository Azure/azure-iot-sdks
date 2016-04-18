// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var RealHttp = require('../lib/job_client_http.js');
var commonTests = require('./_job_client_common_testrun.js');

describe('Over Real HTTPS', function() {
  this.timeout(60000);
  commonTests(RealHttp, process.env.IOTHUB_DM_CONNECTION_STRING);
});