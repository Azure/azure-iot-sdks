// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Amqp = require('../lib/amqp.js');

describe('Amqp', function () {
  describe('#connect', function () {
    /* Tests_SRS_NODE_COMMON_AMQP_06_002: [The connect method shall throw a ReferenceError if the uri parameter has not been supplied.] */
    [undefined, null, ''].forEach(function (badUri){
      it('throws if uri is \'' + badUri +'\'', function () {
        var newClient = new Amqp();
        assert.throws(function () {
          newClient.connect(badUri);
        }, ReferenceError, '');
      });
    });
  });
});