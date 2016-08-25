// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Message = require('azure-iot-common').Message;
var AmqpMessage = require('../lib/amqp_message.js');

describe('AmqpMessage', function () {
  describe('#fromMessage', function () {
    it('throws if message is falsy', function () {
      assert.throws(function () {
        AmqpMessage.fromMessage(null);
      }, ReferenceError, 'message is \'null\'');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_001: [The fromMessage method shall create a new instance of AmqpMessage.]*/
    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_006: [The generated AmqpMessage object shall be returned to the caller.]*/
    it('creates an AmqpMessage object', function () {
      var amqpMessage = AmqpMessage.fromMessage(new Message());
      assert.instanceOf(amqpMessage, AmqpMessage);
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_002: [The created AmqpMessage object shall have a property of type Object named properties.]*/
    it('always creates a properties object on the AmqpMessage object', function () {
      var amqpMessage = AmqpMessage.fromMessage(new Message());
      assert.property(amqpMessage, 'properties');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_003: [If the message argument has a to property, the properties property of the AmqpMessage object shall have a property named to with the same value.]*/
    it('maps message.to to amqpMessage.properties.to', function () {
      var to = 'destination';
      var message = new Message();
      message.to = to;
      var amqpMessage = AmqpMessage.fromMessage(message);
      assert.equal(amqpMessage.properties.to, to);
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_003: [If the message argument has a to property, the properties property of the AmqpMessage object shall have a property named to with the same value.]*/
    it('does not set amqpMessage.properties.to if message.to isn\'t set', function () {
      var amqpMessage = AmqpMessage.fromMessage(new Message());
      assert.notDeepProperty(amqpMessage, 'properties.to');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_004: [If the message argument has an expiryTimeUtc property, the properties property of the AmqpMessage object shall have a property named absoluteExpiryTime with the same value.]*/
    it('maps message.expiryTimeUtc to amqpMessage.properties.absoluteExpiryTime', function () {
      var ts = Date.now() + 60000; // one minute from now
      var message = new Message();
      message.expiryTimeUtc = ts;
      var amqpMessage = AmqpMessage.fromMessage(message);
      assert.equal(amqpMessage.properties.absoluteExpiryTime, ts);
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_004: [If the message argument has an expiryTimeUtc property, the properties property of the AmqpMessage object shall have a property named absoluteExpiryTime with the same value.]*/
    it('does not set amqpMessage.properties.absoluteExpiryTime if message.expiryTimeUtc isn\'t set', function () {
      var amqpMessage = AmqpMessage.fromMessage(new Message());
      assert.notDeepProperty(amqpMessage, 'properties.absoluteExpiryTime');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_007: [If the message argument has a messageId property, the properties property of the AmqpMessage object shall have a property named messageId with the same value.]*/
    it('maps message.messageId to amqpMessage.properties.messageId', function () {
      var messageId = '123';
      var message = new Message();
      message.messageId = messageId;
      var amqpMessage = AmqpMessage.fromMessage(message);
      assert.equal(amqpMessage.properties.messageId, messageId);
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_007: [If the message argument has a messageId property, the properties property of the AmqpMessage object shall have a property named messageId with the same value.]*/
    it('does not set amqpMessage.properties.messageId if message.messageId isn\'t set', function () {
      var amqpMessage = AmqpMessage.fromMessage(new Message());
      assert.notDeepProperty(amqpMessage, 'properties.messageId');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_008: [If needed, the created AmqpMessage object shall have a property of type Object named applicationProperties.]*/
    it('does not create amqpMessage.applicationProperties object if the are no application properties', function () {
      var amqpMessage = AmqpMessage.fromMessage(new Message());
      assert.notProperty(amqpMessage, 'applicationProperties');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_008: [If needed, the created AmqpMessage object shall have a property of type Object named applicationProperties.]*/
    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_009: [If the message argument has an ack property, the applicationProperties property of the AmqpMessage object shall have a property named iothub-ack with the same value.]*/
    it('maps message.ack to amqpMessage.applicationProperties[\'iothub-ack\']', function () {
      var ack = 'full';
      var message = new Message();
      message.ack = ack;
      var amqpMessage = AmqpMessage.fromMessage(message);
      assert.equal(amqpMessage.applicationProperties['iothub-ack'], ack);
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_13_001: [ If message.properties is truthy, then all the properties in it shall be copied to the applicationProperties property of the AmqpMessage object. ]*/
    it('copies message.properties to amqpMessage.applicationProperties', function () {
      var message = new Message();
      var keyItem = '';
      var valueItem = '';
      var numProps = 10;
      var i;

      for (i = 0; i < numProps; i++) {
        keyItem = "itemKey" + (i + 1);
        valueItem = "itemValue" + (i + 1);
        message.properties.add(keyItem, valueItem);
      }

      var amqpMessage = AmqpMessage.fromMessage(message);
      assert.isOk(amqpMessage.applicationProperties);

      for (i = 0; i < numProps; i++) {
        keyItem = "itemKey" + (i + 1);
        valueItem = "itemValue" + (i + 1);
        assert.property(amqpMessage.applicationProperties, keyItem);
        assert.strictEqual(amqpMessage.applicationProperties[keyItem], valueItem);
      }
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_13_001: [ If message.properties is truthy, then all the properties in it shall be copied to the applicationProperties property of the AmqpMessage object. ]*/
    it('does not create amqpMessage.applicationProperties when there are no properties', function () {
      var message = new Message();
      var amqpMessage = AmqpMessage.fromMessage(message);
      assert.isNotOk(amqpMessage.applicationProperties);
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_009: [If the message argument has an ack property, the applicationProperties property of the AmqpMessage object shall have a property named iothub-ack with the same value.]*/
    it('does not set amqpMessage.applicationProperties[\'iothub-ack\'] if message.ack isn\'t set', function () {
      var amqpMessage = AmqpMessage.fromMessage(new Message());
      assert.notDeepProperty(amqpMessage, 'applicationProperties.iothub-ack');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_005: [If message.getData() is truthy, the AmqpMessage object shall have a property named body with the value returned from message.getData().]*/
    it('copies Message body', function () {
      var body = 'hello';
      var message = new Message(body);
      var amqpMessage = AmqpMessage.fromMessage(message);
      assert.equal(amqpMessage.body, body);
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPMSG_05_005: [If message.getData() is truthy, the AmqpMessage object shall have a property named body with the value returned from message.getData().]*/
    it('does not set amqpMessage.body if message does not have a body', function () {
      var amqpMessage = AmqpMessage.fromMessage(new Message());
      assert.notProperty(amqpMessage, 'body');
    });
  });
});