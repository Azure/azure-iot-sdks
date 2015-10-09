// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

function ConnectionString() {
    this.name = 'value';
}

describe('ConnectionString', function () {
  describe('#constructor', function () {
    it('finds the given segment in the connection string', function () {
      var cn = new ConnectionString('name=value', 'name');
      assert.propertyVal(cn, 'name', 'value');
    });

    it('does not find the given segment if it is missing from the connection string', function () {
      var cn = new ConnectionString('name=value', 'othername');
      assert.notProperty('othername');
    });
  });
});