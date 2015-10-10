// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ConnectionString = require('./connection_string.js');

describe('ConnectionString', function () {
  describe('#constructor', function () {
    /*Tests_SRS_NODE_COMMON_CONNSTR_05_001: [The ConnectionString constructor shall accept as an argument the connection string to parse.]*/
    /*Tests_SRS_NODE_COMMON_CONNSTR_05_003: [The constructor shall search value for name/value pairs of the form: 'element=somevalue', where the pair is found either at the beginning of value, or immediately following a semicolon (';').]*/
    /*Tests_SRS_NODE_COMMON_CONNSTR_05_005: [For each pair, the constructor shall create a property on the ConnectionString object instance with the same name as the pair.  The property’s value shall be the pair value.]*/
    it('finds a name/value pair in the connection string', function () {
      var cn = new ConnectionString('name=value');
      assert.propertyVal(cn, 'name', 'value');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_005: [For each pair, the constructor shall create a property on the ConnectionString object instance with the same name as the pair.  The property’s value shall be the pair value.]*/
    it('finds more than one name/value pair in the connection string', function () {
      var cn = new ConnectionString('name1=value1;name2=value2;name3=value3');
      assert.propertyVal(cn, 'name1', 'value1');
      assert.propertyVal(cn, 'name2', 'value2');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_004: [If there are multiple pairs with the same name, the last value shall be used.]*/
    it('uses the last value when duplicate names exist in the connection string', function () {
      var cn = new ConnectionString('name=value1;name=value2');
      assert.propertyVal(cn, 'name', 'value2');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_002: [The constructor shall convert the value argument to type String before using it.]*/
    it('accepts any value argument that is convertible to string', function () {
      var value = { value: 'name=value', toString: function () { return this.value; } };
      var cn = new ConnectionString(value);
      assert.propertyVal(cn, 'name', 'value');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_006: [If the value argument is falsy the constructor shall not add any properties to itself.]*/
    it('creates no properties when the value argument is falsy', function () {
      assert.deepEqual(new ConnectionString(), {});
      assert.deepEqual(new ConnectionString(0), {});
      assert.deepEqual(new ConnectionString(''), {});
      assert.deepEqual(new ConnectionString(null), {});
      assert.deepEqual(new ConnectionString(false), {});
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_005: [For each pair, the constructor shall create a property on the ConnectionString object instance with the same name as the pair.  The property’s value shall be the pair value.]*/
    it('ignores non-pair segments', function () {
      var cn = new ConnectionString('name=value;name-without:a/value');
      assert.deepEqual(cn, { name: 'value' });
    });
    
    /*Tests_SRS_NODE_COMMON_CONNSTR_05_005: [For each pair, the constructor shall create a property on the ConnectionString object instance with the same name as the pair.  The property’s value shall be the pair value.]*/
    it('finds a value with a trailing equal (=) sign', function () {
      var cn = new ConnectionString('name=value=;');
      assert.propertyVal(cn, 'name', 'value=');
    });
    
    /*Tests_SRS_NODE_COMMON_CONNSTR_05_005: [For each pair, the constructor shall create a property on the ConnectionString object instance with the same name as the pair.  The property’s value shall be the pair value.]*/
    it('ignores name=value=value...', function () {
      var cn = new ConnectionString('name=value1=value2');
      assert.deepEqual(cn, {});
    });
  });
});