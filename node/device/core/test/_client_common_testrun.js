// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var sinon = require('sinon');
var util = require('util');

var Client = require('../lib/client.js');
var Message = require('azure-iot-common').Message;
var EventEmitter = require('events').EventEmitter;
var results = require('azure-iot-common').results;
var errors = require('azure-iot-common').errors;

function badConfigTests(opName, badConnStrings, Transport, requestFn) {

  function makeRequestWith(connectionString, test, done) {
    var client = Client.fromConnectionString(connectionString, Transport);
    requestFn(client, function (err, res) {
      test(err, res);
      done();
    });
  }

  function expectNotFoundError(err) {
    assert.include(err.message, 'getaddrinfo ENOTFOUND');
  }

  function expect401Response(err) {
    assert.isNotNull(err);
    assert.equal(err.response.statusCode, 401);
  }

  function expect404Response(err) {
    assert.isNotNull(err);
    assert.equal(err.response.statusCode, 404);
  }

  var tests = [
    { name: 'hostname is malformed', expect: expectNotFoundError },
    { name: 'device is not registered', expect: expect404Response },
    { name: 'password is wrong', expect: expect401Response }
  ];

  /*Tests_SRS_NODE_DEVICE_CLIENT_05_016: [When a Client method encounters an error in the transport, the callback function (indicated by the done argument) shall be invoked with the following arguments:
  err - the standard JavaScript Error object, with a response property that points to a transport-specific response object, and a responseBody property that contains the body of the transport response.]*/
  badConnStrings.forEach(function (test, index) {
    it('fails to ' + opName + ' when the ' + tests[index].name, function (done) {
      makeRequestWith(test, tests[index].expect, done);
    });
  });
}

function runTests(Transport, goodConnectionString, badConnectionStrings) {

  describe('Client', function () {
    describe('#sendEvent', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_007: [The sendEvent method shall send the event indicated by the message argument via the transport associated with the Client instance.]*/
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_017: [With the exception of receive, when a Client method completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - null
      response - a transport-specific response object]*/
      it('sends the event', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var message = new Message('hello');
        client.sendEvent(message, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'MessageEnqueued');
            done();
          }
        });
      });

      badConfigTests('send an event', badConnectionStrings, Transport, function (client, done) {
        client.sendEvent(new Message(''), done);
      });

    });

    describe('#sendEventBatch', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_008: [The sendEventBatch method shall send the list of events (indicated by the messages argument) via the transport associated with the Client instance.]*/
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_017: [With the exception of receive, when a Client method completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - null
      response - a transport-specific response object]*/
      it('sends the event batch message', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var messages = [];
        for (var i = 0; i < 5; i++) {
          messages[i] = new Message('Event Msg ' + i);
        }
        client.sendEventBatch(messages, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'MessageEnqueued');
            done();
          }
        });
      });

      badConfigTests('send an event batch', badConnectionStrings, Transport, function (client, done) {
        var messages = [];
        for (var i = 0; i < 5; i++) {
          messages[i] = new Message('Event Msg ' + i);
        }
        client.sendEventBatch(messages, done);
      });
    });

    describe('#open', function () {
      /* Tests_SRS_NODE_DEVICE_CLIENT_12_001: [The open function shall call the transport’s connect function, if it exists.] */
      it('calls connect on the transport if the method exists', function (done) {
        var TransportCanOpen = function (config) {
          this.config = config;
        };

        TransportCanOpen.prototype.connect = function (callback) {
          callback(null, new results.Connected());
        };

        var client = Client.fromConnectionString(goodConnectionString, TransportCanOpen);
        client.open(function (err, result) {
          if (err) {
            done(err);
          } else {
            assert.equal(result.constructor.name, 'Connected');
            done();
          }
        });
      });

      it('doesn\'t fail if the connect method doesn\'t exist on the transport', function (done) {
        var TransportWithoutConnect = function () { };

        var client = Client.fromConnectionString(goodConnectionString, TransportWithoutConnect);
        client.open(function (err, result) {
          assert.isNull(err);
          assert.equal(result.constructor.name, 'Connected');
          done();
        });
      });

      it('connects the receiver if there\'s a listener on the message event', function (done) {
        var receiver = new EventEmitter();
        receiver.on = sinon.spy();
        var DummyTransport = function () {
          this.open = function (callback) {
            callback(null, new results.Connected());
          };
          this.getReceiver = function (callback) {
            callback(null, receiver);
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        client.on('message', function () { });
        client.open(function (err, result) {
          if (err) {
            done(err);
          } else {
            assert.equal(result.constructor.name, 'Connected');
            assert(receiver.on.calledWith('message'));
            done();
          }
        });
      });
    });

    describe('#close', function () {
      var TransportCanClose = function (config) {
        this.config = config;
      };

      TransportCanClose.prototype.disconnect = function (callback) {
        callback(null, new results.Disconnected(null, 'Disconnected by the client'));
      };

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_001: [The close function shall call the transport’s disconnect function if it exists.] */
      it('calls disconnect on the transport if the method exists', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, TransportCanClose);
        client.close(function () {
          done();
        });
      });
    });

    describe('#setTransportOptions', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_16_021: [The ‘setTransportOptions’ method shall call the ‘setOptions’ method on the transport object.]*/
      /*Tests_SRS_NODE_DEVICE_CLIENT_16_022: [The ‘done’ callback shall be invoked with a null error object and a ‘TransportConfigured’ object nce the transport has been configured.]*/
      it('calls the setOptions method on the transport object and gives it the options parameter', function (done) {
        var testOptions = { foo: 42 };
        var DummyTransport = function () {
          this.setOptions = function (options, callback) {
            assert.equal(options, testOptions);
            callback(null, new results.TransportConfigured());
          };
        };
        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        client.setTransportOptions(testOptions, function (err, result) {
          if (err) {
            done(err);
          } else {
            assert.equal(result.constructor.name, 'TransportConfigured');
            done();
          }
        });
      });

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_023: [The ‘done’ callback shall be invoked with a standard javascript Error object and no result object if the transport could not be configued as requested.]*/
      it('calls the \'done\' callback with an error object if setOptions failed', function (done) {
        var DummyTransport = function () {
          this.setOptions = function (options, callback) {
            var err = new Error('fail');
            callback(err);
          };
        };
        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        client.setTransportOptions({ foo: 42 }, function (err) {
          assert.isNotNull(err);
          done();
        });
      });

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_024: [The ‘setTransportOptions’ method shall throw a ‘ReferenceError’ if the options object is falsy] */
      [null, undefined, '', 0].forEach(function (option) {
        it('throws a ReferenceError if options is ' + option, function () {
          var DummyTransport = function () {
            this.setOptions = function () { };
          };
          var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
          assert.throws(function () {
            client.setTransportOptions(option, function () { });
          }, ReferenceError);
        });
      });

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_025: [The ‘setTransportOptions’ method shall throw a ‘NotImplementedError’ if the transport doesn’t implement a ‘setOption’ method.]*/
      it('throws a NotImplementedError if the setOptions method is not implemented on the transport', function () {
        var DummyTransport = function () { };
        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        assert.throws(function () {
          client.setTransportOptions({ foo: 42 }, function () { });
        }, errors.NotImplementedError);
      });
    });

    describe('#events', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_16_002: [The ‘message’ event shall be emitted when a cloud-to-device message is received from the IoT Hub service.]*/
      it('emits a message event when a message is received', function (done) {
        var DummyReceiver = function () {
          EventEmitter.call(this);
          this.emitMessage = function (msg) {
            this.emit('message', msg);
          };
        };
        util.inherits(DummyReceiver, EventEmitter);

        var receiver = new DummyReceiver();
        var DummyTransport = function () {
          EventEmitter.call(this);
          this.getReceiver = function (callback) {
            callback(null, receiver);
          };
        };
        util.inherits(DummyTransport, EventEmitter);

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        client.on('message', function (msg) {
          /*Tests_SRS_NODE_DEVICE_CLIENT_16_003: [The ‘message’ event parameter shall be a ‘Message’ object.]*/
          assert.equal(msg.constructor.name, 'Message');
          done();
        });

        receiver.emitMessage(new Message());
      });

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_004: [The client shall start listening for messages from the service whenever there is a listener subscribed to the ‘message’ event.]*/
      it('starts listening for messages when a listener subscribes to the message event', function () {
        var DummyReceiver = function () {
          EventEmitter.call(this);
        };
        util.inherits(DummyReceiver, EventEmitter);

        var receiver = new DummyReceiver();
        var DummyTransport = function () {
          EventEmitter.call(this);
          this.getReceiver = function (callback) {
            callback(null, receiver);
          };
        };
        util.inherits(DummyTransport, EventEmitter);

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);

        client.on('message', function () { });
        client.on('message', function () { });
        assert.equal(receiver.listeners('message').length, 1, 'receiver.on was not called once.');
      });

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_005: [The client shall stop listening for messages from the service whenever the last listener unsubscribes from the ‘message’ event.]*/
      it('stops listening for messages when the last listener has unsubscribed', function () {
        var DummyReceiver = function () {
          EventEmitter.call(this);
          this.emitMessage = function (msg) {
            this.emit('message', msg);
          };
          this.removeAllListeners = sinon.spy();
        };
        util.inherits(DummyReceiver, EventEmitter);

        var receiver = new DummyReceiver();
        var DummyTransport = function () {
          EventEmitter.call(this);
          this.getReceiver = function (callback) {
            callback(null, receiver);
          };
        };
        util.inherits(DummyTransport, EventEmitter);

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var listener1 = function () { };
        var listener2 = function () { };
        client.on('message', listener1);
        client.on('message', listener2);

        client.removeListener('message', listener1);
        assert.isFalse(receiver.removeAllListeners.calledOnce);
        client.removeListener('message', listener2);
        assert(receiver.removeAllListeners.calledOnce);
      });
    });

    describe('#complete', function () {
      /*Codes_SRS_NODE_DEVICE_CLIENT_16_016: [The ‘complete’ method shall throw a ReferenceError if the ‘message’ parameter is falsy.] */
      [undefined, null, '', 0].forEach(function (message) {
        it('throws if message is \'' + message + '\'', function () {
          var DummyTransport = function () { };
          var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
          assert.throws(function () {
            client.complete(message, function () { });
          }, ReferenceError);
        });
      });

      /*Codes_SRS_NODE_DEVICE_CLIENT_16_007: [The ‘complete’ method shall call the ‘complete’ method of the transport with the message as an argument]*/
      it('calls the complete method on the transport with the message as an argument', function () {
        var DummyTransport = function () {
          this.complete = sinon.spy();
        };
        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var message = new Message();
        client.complete(message, function () { });
        assert(client._transport.complete.calledOnce);
        assert(client._transport.complete.calledWith(message));
      });

      /*Codes_SRS_NODE_DEVICE_CLIENT_16_008: [The ‘done’ callback shall be called with a null error object and a ‘MessageCompleted’ result once the transport has completed the message.]*/
      it('calls the done callback with a null error object and a result', function (done) {
        var DummyTransport = function () {
          this.complete = function (message, callback) {
            callback(null, new results.MessageCompleted());
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var message = new Message();
        client.complete(message, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'MessageCompleted');
            done();
          }
        });
      });

      /*Codes_SRS_NODE_DEVICE_CLIENT_16_009: [The ‘done’ callback shall be called with a standard javascript Error object and no result object if the transport could not complete the message.]*/
      it('calls the done callback with a null error object and a result', function (done) {
        var DummyTransport = function () {
          this.complete = function (message, callback) {
            callback(null, new results.MessageCompleted());
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var message = new Message();
        client.complete(message, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'MessageCompleted');
            done();
          }
        });
      });
    });

    describe('#reject', function () {
      /*Codes_SRS_NODE_DEVICE_CLIENT_16_018: [The ‘reject’ method shall throw a ReferenceError if the ‘message’ parameter is falsy.] */
      [undefined, null, '', 0].forEach(function (message) {
        it('throws if message is \'' + message + '\'', function () {
          var DummyTransport = function () { };
          var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
          assert.throws(function () {
            client.reject(message, function () { });
          }, ReferenceError);
        });
      });

      /*Codes_SRS_NODE_DEVICE_CLIENT_16_010: [The ‘reject’ method shall call the ‘reject’ method of the transport with the message as an argument]*/
      it('calls the reject method on the transport with the message as an argument', function () {
        var DummyTransport = function () {
          this.reject = sinon.spy();
        };
        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var message = new Message();
        client.reject(message, function () { });
        assert(client._transport.reject.calledOnce);
        assert(client._transport.reject.calledWith(message));
      });

      /*Codes_SRS_NODE_DEVICE_CLIENT_16_011: [The ‘done’ callback shall be called with a null error object and a ‘MessageRejected’ result once the transport has rejected the message.]*/
      it('calls the done callback with a null error object and a result', function (done) {
        var DummyTransport = function () {
          this.reject = function (message, callback) {
            callback(null, new results.MessageRejected());
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var message = new Message();
        client.reject(message, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'MessageRejected');
            done();
          }
        });
      });

      /*Codes_SRS_NODE_DEVICE_CLIENT_16_012: [The ‘done’ callback shall be called with a standard javascript Error object and no result object if the transport could not reject the message.]*/
      it('calls the done callback with a null error object and a result', function (done) {
        var DummyTransport = function () {
          this.reject = function (message, callback) {
            callback(null, new results.MessageRejected());
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var message = new Message();
        client.reject(message, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'MessageRejected');
            done();
          }
        });
      });
    });

    describe('#abandon', function () {
      /*Codes_SRS_NODE_DEVICE_CLIENT_16_018: [The ‘abandon’ method shall throw a ReferenceError if the ‘message’ parameter is falsy.] */
      [undefined, null, '', 0].forEach(function (message) {
        it('throws if message is \'' + message + '\'', function () {
          var DummyTransport = function () { };
          var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
          assert.throws(function () {
            client.abandon(message, function () { });
          }, ReferenceError);
        });
      });

      /*Codes_SRS_NODE_DEVICE_CLIENT_16_010: [The ‘abandon’ method shall call the ‘abandon’ method of the transport with the message as an argument]*/
      it('calls the abandon method on the transport with the message as an argument', function () {
        var DummyTransport = function () {
          this.abandon = sinon.spy();
        };
        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var message = new Message();
        client.abandon(message, function () { });
        assert(client._transport.abandon.calledOnce);
        assert(client._transport.abandon.calledWith(message));
      });

      /*Codes_SRS_NODE_DEVICE_CLIENT_16_011: [The ‘done’ callback shall be called with a null error object and a ‘Messageabandoned’ result once the transport has abandoned the message.]*/
      it('calls the done callback with a null error object and a result', function (done) {
        var DummyTransport = function () {
          this.abandon = function (message, callback) {
            callback(null, new results.MessageAbandoned());
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var message = new Message();
        client.abandon(message, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'MessageAbandoned');
            done();
          }
        });
      });

      /*Codes_SRS_NODE_DEVICE_CLIENT_16_012: [The ‘done’ callback shall be called with a standard javascript Error object and no result object if the transport could not abandon the message.]*/
      it('calls the done callback with a null error object and a result', function (done) {
        var DummyTransport = function () {
          this.abandon = function (message, callback) {
            callback(null, new results.MessageAbandoned());
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var message = new Message();
        client.abandon(message, function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'MessageAbandoned');
            done();
          }
        });
      });
    });

    describe('#updateSharedAccessSignature', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_16_031: [The updateSharedAccessSignature method shall throw a ReferenceError if the sharedAccessSignature parameter is falsy.]*/
      [undefined, null, '', 0].forEach(function (sas) {
        it('throws a ReferenceError if sharedAccessSignature is \'' + sas + '\'', function () {
          var DummyTransport = function () { };
          var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
          assert.throws(function () {
            client.updateSharedAccessSignature(sas, function () { });
          }, ReferenceError);
        });
      });

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_032: [The updateSharedAccessSignature method shall call the updateSharedAccessSignature method of the transport currently in use with the sharedAccessSignature parameter.]*/
      it('calls the transport `updateSharedAccessSignature` method with the sharedAccessSignature parameter', function () {
        var DummyTransport = function () {
          this.updateSharedAccessSignature = sinon.spy();
        };
        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        var sas = 'sas';
        client.updateSharedAccessSignature(sas, function () { });
        assert(client._transport.updateSharedAccessSignature.calledOnce);
        assert(client._transport.updateSharedAccessSignature.calledWith(sas));
      });

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_033: [The updateSharedAccessSignature method shall reconnect the transport to the IoTHub service if it was connected before before the method is called.]*/
      it('Reconnects the transport and receiver if necessary', function (done) {
        var connectCalled = false;
        var getReceiverCalled = false;
        var DummyTransport = function () {
          this.connect = function (callback) {
            connectCalled = true;
            callback(null);
          };
          this.updateSharedAccessSignature = function (sas, callback) {
            callback(null, new results.SharedAccessSignatureUpdated(true));
          };
          this.getReceiver = function (callback) {
            getReceiverCalled = true;
            callback(null, new EventEmitter());
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        client.on('message', function () { });
        client.updateSharedAccessSignature('sas', function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.isFalse(res.needToReconnect);
            assert(connectCalled);
            assert(getReceiverCalled);
            done();
          }
        });
      });

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_034: [The updateSharedAccessSignature method shall not reconnect the transport if the transport was disconnected to begin with.]*/
      it('Doesn\'t reconnect the transport if it\'s not necessary', function (done) {
        var DummyTransport = function () {
          this.connect = function () { assert.fail(); };
          this.updateSharedAccessSignature = function (sas, callback) {
            callback(null, new results.SharedAccessSignatureUpdated(false));
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        client.updateSharedAccessSignature('sas', function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.isFalse(res.needToReconnect);
            done();
          }
        });
      });


      /*Tests_SRS_NODE_DEVICE_CLIENT_16_035: [The updateSharedAccessSignature method shall call the `done` callback with an error object if an error happened while renewing the token.]*/
      it('Calls the `done` callback with an error object if an error happened', function (done) {
        var DummyTransport = function () {
          this.updateSharedAccessSignature = function (sas, callback) {
            callback(new Error('foo'));
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        client.updateSharedAccessSignature('sas', function (err) {
          assert.isOk(err);
          done();
        });
      });

      /*Tests_SRS_NODE_DEVICE_CLIENT_16_036: [The updateSharedAccessSignature method shall call the `done` callback with a null error object and a result of type SharedAccessSignatureUpdated if the token was updated successfully.]*/
      it('Calls the `done` callback with a null error object and a SharedAccessSignatureUpdated result', function (done) {
        var DummyTransport = function () {
          this.updateSharedAccessSignature = function (sas, callback) {
            callback(null, new results.SharedAccessSignatureUpdated());
          };
        };

        var client = Client.fromConnectionString(goodConnectionString, DummyTransport);
        client.updateSharedAccessSignature('sas', function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'SharedAccessSignatureUpdated');
            done();
          }
        });
      });
    });
  });
}

module.exports = runTests;