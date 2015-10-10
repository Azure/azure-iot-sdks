// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ConnectionString = require('./connection_string.js');

describe('ConnectionString', function () {
  describe('#constructor', function () {
    /*Tests_SRS_NODE_COMMON_CONNSTR_05_001: [The ConnectionString constructor shall accept two arguments:
    value - the connection string to parse
    segments - the name of the property to expect when parsing value, or an array of such names]*/
    /*Tests_SRS_NODE_COMMON_CONNSTR_05_005: [For each element in the segments array, the constructor shall search value for a match of the form: 'element=somevalue', where this match is found either at the beginning of value, or immediately following a semicolon (';').]*/
    /*Tests_SRS_NODE_COMMON_CONNSTR_05_007: [For each element that has a match, the constructor shall create a property on the ConnectionString object instance with the same name as the element value.  The property’s value shall be the sequence of characters in the connection string following the equal sign ('=') up to but not including the next semicolon (';'), or to the end of the string if no semicolon exists.]*/
    it('finds the given segment in the connection string', function () {
      var cn = new ConnectionString('name=value', 'name');
      assert.propertyVal(cn, 'name', 'value');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_007: [For each element that has a match, the constructor shall create a property on the ConnectionString object instance with the same name as the element value.  The property’s value shall be the sequence of characters in the connection string following the equal sign ('=') up to but not including the next semicolon (';'), or to the end of the string if no semicolon exists.]*/
    it('does not find the given segment if it is missing from the connection string', function () {
      var cn = new ConnectionString('name=value', 'othername');
      assert.notProperty('othername');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_007: [For each element that has a match, the constructor shall create a property on the ConnectionString object instance with the same name as the element value.  The property’s value shall be the sequence of characters in the connection string following the equal sign ('=') up to but not including the next semicolon (';'), or to the end of the string if no semicolon exists.]*/
    it('finds more than one segment in the connection string', function () {
      var cn = new ConnectionString('name1=value1;name2=value2', ['name1', 'name2']);
      assert.propertyVal(cn, 'name1', 'value1');
      assert.propertyVal(cn, 'name2', 'value2');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_007: [For each element that has a match, the constructor shall create a property on the ConnectionString object instance with the same name as the element value.  The property’s value shall be the sequence of characters in the connection string following the equal sign ('=') up to but not including the next semicolon (';'), or to the end of the string if no semicolon exists.]*/
    it('exactly matches segment names', function () {
      var cn = new ConnectionString('not-my-name=value', 'name');
      assert.notProperty(cn, 'name');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_007: [For each element that has a match, the constructor shall create a property on the ConnectionString object instance with the same name as the element value.  The property’s value shall be the sequence of characters in the connection string following the equal sign ('=') up to but not including the next semicolon (';'), or to the end of the string if no semicolon exists.]*/
    it('only matches the segments you care about', function () {
      var cn = new ConnectionString('name1=value1;name2=value2', 'name2');
      assert.notProperty(cn, 'name1');
      assert.propertyVal(cn, 'name2', 'value2');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_006: [If there are multiple matches for a given element, only the first shall be used.]*/
    it('finds the first match when duplicate segment names exist in the connection string', function () {
      var cn = new ConnectionString('name=value1;name=value2', 'name');
      assert.propertyVal(cn, 'name', 'value1');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_002: [The constructor shall convert the value argument to type String before using it.]*/
    it('accepts any value argument that is convertible to string', function () {
      var arg = { value: 'name=value', toString: function () { return this.value; } };
      var cn = new ConnectionString(arg, 'name');
      assert.propertyVal(cn, 'name', 'value');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_003: [If the segments argument is not of type Array, the constructor shall convert it to type Array before using it.]*/
    it('accepts any segments argument that is convertible to string', function () {
      var arg = { value: 'name', toString: function () { return this.value; } };
      var cn = new ConnectionString('name=value', arg);
      assert.propertyVal(cn, 'name', 'value');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_004: [The constructor shall convert all elements in the segments argument to type String before using them.]*/
    it('accepts any segments argument that is convertible to array of strings', function () {
      var arg = [
        { value: 'name', toString: function () { return this.value; } }
      ];
      var cn = new ConnectionString('name=value', arg);
      assert.propertyVal(cn, 'name', 'value');
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_008: [If either value or segments arguments are falsy, or if the segments array is empty, the constructor shall not add any properties to itself.]*/
    it('creates no properties when value argument is falsy', function () {
      var cn = new ConnectionString('', 'name');
      assert.deepEqual(cn, {});
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_008: [If either value or segments arguments are falsy, or if the segments array is empty, the constructor shall not add any properties to itself.]*/
    it('creates no properties when segments argument is an empty array', function () {
      var cn = new ConnectionString('name=value', []);
      assert.deepEqual(cn, {});
    });

    /*Tests_SRS_NODE_COMMON_CONNSTR_05_008: [If either value or segments arguments are falsy, or if the segments array is empty, the constructor shall not add any properties to itself.]*/
    it('creates no properties when segments argument is falsy', function () {
      var cn = new ConnectionString('name=value', null);
      assert.deepEqual(cn, {});
    });
  });
});