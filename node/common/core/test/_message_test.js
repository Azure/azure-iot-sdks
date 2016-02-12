// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Message = require('../lib/message.js');

var stringTestMsg = "message";

describe('message', function () {

  describe('#getData', function () {

    /*Tests_SRS_NODE_IOTHUB_MESSAGE_07_003: [The getData function shall return a representation of the body of the message as the type that was presented during construction.]*/
    it('returns the underlying data', function () {
      var message = new Message(stringTestMsg);
      assert.equal(message.getData(), stringTestMsg);
    });

  });

  describe('#getBytes', function () {

    /*Tests_SRS_NODE_IOTHUB_MESSAGE_07_001: [If the data message that is store is of type Buffer then the data object will get returned unaltered.]*/
    it('returns a Buffer when the underlying data is a Buffer', function () {
      var original = new Buffer(stringTestMsg);
      var message = new Message(original);
      var buffer = message.getBytes();
      assert.instanceOf(buffer, Buffer);
      assert.equal(buffer, original);
    });

    /*Tests_SRS_NODE_IOTHUB_MESSAGE_07_002: [If the data message is of any other type then the data will be converted to a Buffer object and returned.]*/
    it('returns a Buffer when the underlying data is NOT a Buffer', function () {
      var message = new Message(stringTestMsg);
      var buffer = message.getBytes();
      assert.instanceOf(buffer, Buffer);
      assert.equal(buffer.toString('ascii'), stringTestMsg);
    });

  });

});
