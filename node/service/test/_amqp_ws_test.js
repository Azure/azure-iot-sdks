// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var Amqp = require('../lib/amqp.js');
var AmqpWs = require('../lib/amqp_ws.js');
var assert = require('chai').assert;

describe('AmqpWs', function() {
  describe('#constructor', function() {
    /*Tests_SRS_NODE_IOTHUB_SERVICE_AMQP_WS_16_002: [`AmqpWs` should inherit from `Amqp`.]*/
    it('inherits from `Amqp`', function() {
      var amqpWs = new AmqpWs({
        host: 'host',
        hubName: 'hubName',
        keyName: 'keyName',
        sharedAccessSignature: 'sas'
      });
      
      assert.instanceOf(amqpWs, Amqp);
    });
  });
});
