// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

function ConnectionString() {
    this.segment = 'abc';
}

describe('ConnectionString', function () {
  describe('#constructor', function () {
    it('finds the given segment in the connection string', function () {
      var cn = new ConnectionString('segment=abc', 'segment');
      assert.propertyVal(cn, 'segment', 'abc');
    });
  });
});