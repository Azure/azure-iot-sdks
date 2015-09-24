// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

module.exports = {
  checkVersion: require('./lib/version_checker.js').check,
  runTests: require('./lib/mocha_runner.js').runTests,
  jshintConfig: require('./lib/jshint.conf.js')
};