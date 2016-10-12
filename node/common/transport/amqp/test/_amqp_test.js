// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var sinon = require('sinon');
require('sinon-as-promised');
var Amqp = require('../lib/amqp.js');
var AmqpReceiver = require('../lib/amqp_receiver.js');
var results = require('azure-iot-common').results;
var Message = require('azure-iot-common').Message;
var EventEmitter = require('events').EventEmitter;


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

    /*Tests_SRS_NODE_COMMON_AMQP_16_002: [The connect method shall establish a connection with the IoT hub instance and if given as argument call the `done` callback with a null error object in the case of success and a `results.Connected` object.]*/
    it('Calls the done callback when successfully connected', function(testCallback) {
      var amqp = new Amqp();
      sinon.stub(amqp._amqp, 'connect').resolves('connected');
      amqp.connect('uri', null, function(err, res) {
        if (err) testCallback(err);
        else {
          assert.instanceOf(res, results.Connected);
          testCallback();
        }
      });
    });

    it('Calls the done callback immediately when already connected', function(testCallback) {
      var amqp = new Amqp();
      sinon.stub(amqp._amqp, 'connect').rejects('amqp10.client.connect should not be called');
      amqp._connected = true;
      amqp.connect('uri', null, function(err, res) {
        if (err) testCallback(err);
        else {
          assert.instanceOf(res, results.Connected);
          testCallback();
        }
      });
    });

    /*Tests_SRS_NODE_COMMON_AMQP_16_003: [If given as an argument, the connect method shall call the `done` callback with a standard `Error` object if the connection fails.]*/
    it('Calls the done callback with an error if connecting fails', function(testCallback) {
      var amqp = new Amqp();
      sinon.stub(amqp._amqp, 'connect').rejects('connection failed');
      amqp.connect('uri', null, function(err) {
        assert.instanceOf(err, Error);
        testCallback();
      });
    });
  });

  describe('#setDisconnectHandler', function() {
    it('disconnect callback is called when the \'connection:closed\' event is emitted', function(testCallback) {
      var amqp = new Amqp();
      amqp.setDisconnectHandler(function() {
        testCallback();
      });

      amqp._amqp.emit('connection:closed');
    });
  });

  describe('#disconnect', function() {
    /*Tests_SRS_NODE_COMMON_AMQP_16_004: [The disconnect method shall call the `done` callback when the application/service has been successfully disconnected from the service]*/
    it('calls the done callback if disconnected successfully', function(testCallback) {
      var amqp = new Amqp();
      sinon.stub(amqp._amqp, 'disconnect').resolves();
      amqp.disconnect(function(err) {
        if (err) testCallback(err);
        else {
          testCallback();
        }
      });
    });

    /*Tests_SRS_NODE_COMMON_AMQP_16_005: [The disconnect method shall call the `done` callback and pass the error as a parameter if the disconnection is unsuccessful]*/
    it('calls the done callback with an error if there\'s an error while disconnecting', function(testCallback) {
      var amqp = new Amqp();
      sinon.stub(amqp._amqp, 'disconnect').rejects('disconnection error');
      amqp.disconnect(function(err) {
        if (err) {
          assert.instanceOf(err, Error);
          testCallback();
        } else {
          testCallback(new Error('disconnect callback was called without an error'));
        }
      });
    });
  });

  describe('#send', function() {
    it('calls the done callback with an error if not connected', function(testCallback) {
      var amqp = new Amqp();
      amqp.send(new Message('message'), 'endpoint', 'deviceId', function(err) {
        assert.instanceOf(err, Error);
        testCallback();
      });
    });

    it('calls the done callback with a MessageEnqueued result if it successful', function(testCallback) {
      var amqp = new Amqp();
      var sender = new EventEmitter();
      sender.send = sinon.stub().resolves('message enqueued');
      
      sinon.stub(amqp._amqp, 'connect').resolves('connected');
      sinon.stub(amqp._amqp, 'createSender').resolves(sender);

      amqp.connect('uri', null, function() {
        amqp.send(new Message('message'), 'endpoint', 'deviceId', function(err, result) {
          if(!err) {
            assert.instanceOf(result, results.MessageEnqueued);
          }
          testCallback(err);
        
        });
      });
    });

    /*Tests_SRS_NODE_COMMON_AMQP_16_007: [If send encounters an error before it can send the request, it shall invoke the `done` callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
    it('calls the done callback with an Error if creating a sender fails', function(testCallback) {
      var amqp = new Amqp();
      var sender = new EventEmitter();
      sender.send = sinon.stub().resolves('message enqueued');
      
      sinon.stub(amqp._amqp, 'connect').resolves('connected');
      sinon.stub(amqp._amqp, 'createSender').rejects('failed to create sender');

      amqp.connect('uri', null, function() {
        amqp.send(new Message('message'), 'endpoint', 'deviceId', function(err) {
          assert.instanceOf(err, Error);
          testCallback();
        });
      });
    });

    it('calls the done callback with an Error if the sender fails to send the message', function(testCallback) {
      var amqp = new Amqp();
      var sender = new EventEmitter();
      sender.send = sinon.stub().rejects('could not send');
      
      sinon.stub(amqp._amqp, 'connect').resolves('connected');
      sinon.stub(amqp._amqp, 'createSender').resolves(sender);

      amqp.connect('uri', null, function() {
        amqp.send(new Message('message'), 'endpoint', 'deviceId', function(err) {
          assert.instanceOf(err, Error);
          testCallback();
        });
      });
    });

    it('Reuses the same sender link if already created', function(testCallback) {
      var amqp = new Amqp();
      var sender = new EventEmitter();
      sender.send = sinon.stub().resolves('message enqueued');
      amqp._sender = sender;

      sinon.stub(amqp._amqp, 'connect').resolves('connected');
      sinon.stub(amqp._amqp, 'createSender').rejects('createSender should not be called');

      amqp.connect('uri', null, function() {
        amqp.send(new Message('message'), 'endpoint', 'deviceId', function(err, result) {
          if (!err) {
            assert.instanceOf(result, results.MessageEnqueued);
          }
          testCallback(err);
        });
      });
    });
  });

  describe('#getReceiver', function() {
    /*Tests_SRS_NODE_COMMON_AMQP_16_010: [If a receiver for this endpoint doesn't exist, the getReceiver method should create a new AmqpReceiver object and then call the `done` method with the object that was just created as an argument.]*/
    it('calls the done callback with a null Error and a receiver if successful', function(testCallback) {
      var amqp = new Amqp();
      sinon.stub(amqp._amqp, 'connect').resolves('connected');
      sinon.stub(amqp._amqp, 'createReceiver').resolves('receiver');

      amqp.connect('uri', null, function() {
        amqp.getReceiver('endpoint', function(err, receiver) {
          assert.instanceOf(receiver, AmqpReceiver);
          testCallback(err);
        });
      });
    });

    /*Tests_SRS_NODE_COMMON_AMQP_16_009: [If a receiver for this endpoint has already been created, the getReceiver method should call the `done` method with the existing instance as an argument.]*/
    it('gets the existing receiver for an endpoint if it was previously created', function(testCallback) {
      var amqp = new Amqp();
      sinon.stub(amqp._amqp, 'connect').resolves('connected');
      sinon.stub(amqp._amqp, 'createReceiver').resolves(new AmqpReceiver('fake'));

      amqp.connect('uri', null, function() {
        amqp.getReceiver('endpoint', function(err, recv1) {
          if (err) {
            testCallback(err);
          } else {
            amqp.getReceiver('endpoint', function(err, recv2) {
              if (err) {
                testCallback(err);
              } else {
                assert.equal(recv1, recv2);
                testCallback();
              }
            });
          }
        });
      });

    });

    it('calls the done callback with an Error if it fails', function(testCallback) {
      var amqp = new Amqp();
      sinon.stub(amqp._amqp, 'connect').resolves('connected');
      sinon.stub(amqp._amqp, 'createReceiver').rejects(new Error('cannt create receiver'));

      amqp.connect('uri', null, function() {
        amqp.getReceiver('endpoint', function(err) {
          assert.instanceOf(err, Error);
          testCallback();
        });
      });
    });
  });
});