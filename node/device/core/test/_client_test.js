// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var sinon = require('sinon');
var stream = require('stream');
var util = require('util');
var EventEmitter = require('events').EventEmitter;

var Client = require('../lib/client.js');
var SimulatedHttp = require('./http_simulated.js');
var runTests = require('./_client_common_testrun.js');
var results = require('azure-iot-common').results;
var errors = require('azure-iot-common').errors;
var Message = require('azure-iot-common').Message;

describe('Client', function () {
  describe('#constructor', function () {
    /*Tests_SRS_NODE_DEVICE_CLIENT_05_001: [The Client constructor shall throw ReferenceError if the transport argument is falsy.]*/
    it('throws if transport arg is falsy', function () {
      [null, undefined, '', 0].forEach(function (transport) {
        assert.throws(function () {
          return new Client(transport);
        }, ReferenceError, 'transport is \'' + transport + '\'');
      });
    });
  });

  describe('#fromConnectionString', function () {
    var connectionString = 'HostName=host;DeviceId=id;SharedAccessKey=key';

    /*Tests_SRS_NODE_DEVICE_CLIENT_05_003: [The fromConnectionString method shall throw ReferenceError if the connStr argument is falsy.]*/
    it('throws if connStr arg is falsy', function () {
      [null, undefined, '', 0].forEach(function (value) {
        assert.throws(function () {
          return Client.fromConnectionString(value);
        }, ReferenceError, 'connStr is \'' + value + '\'');
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_05_006: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(new Transport(...)).]*/
    it('returns an instance of Client', function () {
      var DummyTransport = function () { };
      var client = Client.fromConnectionString(connectionString, DummyTransport);
      assert.instanceOf(client, Client);
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_027: [If a connection string argument is provided and is using SharedAccessKey authentication, the Client shall automatically generate and renew SAS tokens.] */
    it('automatically renews the SAS token before it expires', function (done) {
      this.clock = sinon.useFakeTimers();
      var clock = this.clock;
      var tick = 0;
      var firstTick = 1800000;
      var secondTick = 1200000;
      var sasUpdated = false;
      var DummyTransport = function () {
        this.updateSharedAccessSignature = function () {
          sasUpdated = true;
          assert.equal(tick, secondTick);
          clock.restore();
          done();
        };
      };

      var client = Client.fromConnectionString(connectionString, DummyTransport);
      assert.instanceOf(client, Client);
      tick = firstTick;
      this.clock.tick(tick); // 30 minutes. shouldn't have updated yet.
      assert.isFalse(sasUpdated);
      tick = secondTick;
      this.clock.tick(tick); // +20 => 50 minutes. should've updated so the callback will be called and the test will terminate.
    });

    it('doesn\'t try to renew the SAS token when using x509', function (done) {
      this.clock = sinon.useFakeTimers();
      var clock = this.clock;
      var DummyTransport = function () {
        this.updateSharedAccessSignature = function () {
          clock.restore();
          done(new Error('updateSharedAccessSignature should not have been called'));
        };
      };

      var x509ConnectionString = 'HostName=host;DeviceId=id;x509=true';
      var client = Client.fromConnectionString(x509ConnectionString, DummyTransport);
      assert.instanceOf(client, Client);
      this.clock.tick(3600000); // 1 hour: this should trigger the call to renew the SAS token.
      clock.restore();
      done();
    });
  });

  describe('#fromSharedAccessSignature', function () {
    /*Tests_SRS_NODE_DEVICE_CLIENT_16_029: [The fromSharedAccessSignature method shall throw a ReferenceError if the sharedAccessSignature argument is falsy.] */
    it('throws if sharedAccessSignature arg is falsy', function () {
      [null, undefined, '', 0].forEach(function (value) {
        assert.throws(function () {
          return Client.fromSharedAccessSignature(value);
        }, ReferenceError, 'sharedAccessSignature is \'' + value + '\'');
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_030: [The fromSharedAccessSignature method shall return a new instance of the Client object] */
    it('returns an instance of Client', function () {
      var DummyTransport = function () { };
      var sharedAccessSignature = '"SharedAccessSignature sr=hubName.azure-devices.net/devices/deviceId&sig=s1gn4tur3&se=1454204843"';
      var client = Client.fromSharedAccessSignature(sharedAccessSignature, DummyTransport);
      assert.instanceOf(client, Client);
    });

    it('create a correct config when sr is not URI-encoded', function () {
      var sharedAccessSignature = '"SharedAccessSignature sr=hubName.azure-devices.net/devices/deviceId&sig=s1gn4tur3&se=1454204843"';
      var DummyTransport = function (config) {
        assert.strictEqual(config.host, 'hubName.azure-devices.net');
        assert.strictEqual(config.deviceId, 'deviceId');
        assert.strictEqual(config.hubName, 'hubName');
        assert.strictEqual(config.sharedAccessSignature, sharedAccessSignature);
      };
      Client.fromSharedAccessSignature(sharedAccessSignature, DummyTransport);
    });

    it('create a correct config when sr is URI-encoded', function () {
      var sharedAccessSignature = '"SharedAccessSignature sr=hubName.azure-devices.net%2Fdevices%2FdeviceId&sig=s1gn4tur3&se=1454204843"';
      var DummyTransport = function (config) {
        assert.strictEqual(config.host, 'hubName.azure-devices.net');
        assert.strictEqual(config.deviceId, 'deviceId');
        assert.strictEqual(config.hubName, 'hubName');
        assert.strictEqual(config.sharedAccessSignature, sharedAccessSignature);
      };
      Client.fromSharedAccessSignature(sharedAccessSignature, DummyTransport);
    });
  });

  describe('setTransportOptions', function () {
    /*Tests_SRS_NODE_DEVICE_CLIENT_16_021: [The ‘setTransportOptions’ method shall call the ‘setOptions’ method on the transport object.]*/
    /*Tests_SRS_NODE_DEVICE_CLIENT_16_022: [The ‘done’ callback shall be invoked with a null error object and a ‘TransportConfigured’ object nce the transport has been configured.]*/
    it('calls the setOptions method on the transport object and gives it the options parameter', function (done) {
      var testOptions = { foo: 42 };
      var DummyTransport = function () {
        this.setOptions = function (options, callback) {
          assert.equal(options.http.receivePolicy, testOptions);
          callback(null, new results.TransportConfigured());
        };
      };
      var client = new Client(new DummyTransport());
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
      var client = new Client(new DummyTransport());
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
        var client = new Client(new DummyTransport());
        assert.throws(function () {
          client.setTransportOptions(option, function () { });
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_025: [The ‘setTransportOptions’ method shall throw a ‘NotImplementedError’ if the transport doesn’t implement a ‘setOption’ method.]*/
    it('throws a NotImplementedError if the setOptions method is not implemented on the transport', function () {
      var DummyTransport = function () { };
      var client = new Client(new DummyTransport());
      assert.throws(function () {
        client.setTransportOptions({ foo: 42 }, function () { });
      }, errors.NotImplementedError);
    });
  });
  
  describe('setOptions', function() {
    /*Tests_SRS_NODE_DEVICE_CLIENT_16_042: [The `setOptions` method shall throw a `ReferenceError` if the options object is falsy.]*/
    [null, undefined].forEach(function(options) {
      it('throws is options is ' + options, function() {
        var client = new Client({});
        assert.throws(function () {
          client.setOptions(options, function () { });
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_043: [The `done` callback shall be invoked no parameters when it has successfully finished setting the client and/or transport options.]*/
    it('calls the done callback with no parameters when it has successfully configured the client', function(done) {
      var DummyTransport = function () {
        this.setOptions = function (options, done) {
          done();
        };
      };

      var client = new Client(new DummyTransport());
      client.setOptions({}, done);
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_044: [The `done` callback shall be invoked with a standard javascript `Error` object and no result object if the client could not be configured as requested.]*/
    it('calls the done callback with no parameters when it has successfully configured the client', function(done) {
      var FailingTransport = function () {
        this.setOptions = function (options, done) {
          done(new Error('dummy error'));
        };
      };

      var client = new Client(new FailingTransport());
      client.setOptions({}, function(err) {
        assert.instanceOf(err, Error);
        done();
      });
    });
  });

  describe('uploadToBlob', function() {
    /*Tests_SRS_NODE_DEVICE_CLIENT_16_037: [The `uploadToBlob` method shall throw a `ReferenceError` if `blobName` is falsy.]*/
    [undefined, null, ''].forEach(function (blobName) {
      it('throws a ReferenceError if \'blobName\' is ' + blobName + '\'', function() {
        var client = new Client({}, null, {});
        assert.throws(function() {
          client.uploadToBlob(blobName, new stream.Readable(), 42, function() {});
        });
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_038: [The `uploadToBlob` method shall throw a `ReferenceError` if `stream` is falsy.]*/
    [undefined, null, ''].forEach(function (stream) {
      it('throws a ReferenceError if \'stream\' is ' + stream + '\'', function() {
        var client = new Client({}, null, {});
        assert.throws(function() {
          client.uploadToBlob('blobName', stream, 42, function() {});
        });
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_039: [The `uploadToBlob` method shall throw a `ReferenceError` if `streamLength` is falsy.]*/
    [undefined, null, '', 0].forEach(function (streamLength) {
      it('throws a ReferenceError if \'streamLength\' is ' + streamLength + '\'', function() {
        var client = new Client({}, null, {});
        assert.throws(function() {
          client.uploadToBlob('blobName', new stream.Readable(), streamLength, function() {});
        });
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_040: [The `uploadToBlob` method shall call the `done` callback with an `Error` object if the upload fails.]*/
    it('calls the done callback with an Error object if the upload fails', function(done) {
      var DummyBlobUploader = function () {
        this.uploadToBlob = function(blobName, stream, streamLength, callback) {
          callback(new Error('fake error'));
        };
      };

      var client = new Client({}, null, new DummyBlobUploader());
      client.uploadToBlob('blobName', new stream.Readable(), 42, function(err) {
        assert.instanceOf(err, Error);
        done();
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_041: [The `uploadToBlob` method shall call the `done` callback no parameters if the upload succeeds.]*/
    it('calls the done callback with no parameters if the upload succeeded', function (done) {
      var DummyBlobUploader = function () {
        this.uploadToBlob = function(blobName, stream, streamLength, callback) {
          callback();
        };
      };

      var client = new Client({}, null, new DummyBlobUploader());
      client.uploadToBlob('blobName', new stream.Readable(), 42, done);
    });
  });

  describe('#open', function () {
    var TransportCanOpen = function (config) {
      EventEmitter.call(this);
      this.config = config;
    };
    util.inherits(TransportCanOpen, EventEmitter);

    TransportCanOpen.prototype.connect = function (callback) {
      callback(null, new results.Connected());
    };

    /* Tests_SRS_NODE_DEVICE_CLIENT_12_001: [The open function shall call the transport’s connect function, if it exists.] */
    it('calls connect on the transport if the method exists', function (done) {
      var client = new Client(new TransportCanOpen());
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
      var client = new Client(new TransportWithoutConnect());
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

      var client = new Client(new DummyTransport());
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

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_045: [If the transport successfully establishes a connection the `open` method shall subscribe to the `disconnect` event of the transport.]*/
    it('subscribes to the \'disconnect\' event once connected', function(done) {
      var transport = new TransportCanOpen();
      var client = new Client(transport);
      client.open(function() {
        client.on('disconnect', function() {
          done();
        });

        transport.emit('disconnect');
      });
    });
  });

  describe('#close', function () {
    var TransportCanClose = function (config) {
      EventEmitter.call(this);
      this.config = config;
    };
    util.inherits(TransportCanClose, EventEmitter);

    TransportCanClose.prototype.connect = function (callback) {
      callback(null, new results.Connected());
    };

    TransportCanClose.prototype.disconnect = function (callback) {
      callback(null, new results.Disconnected(null, 'Disconnected by the client'));
    };

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_001: [The close function shall call the transport’s disconnect function if it exists.] */
    it('calls disconnect on the transport if the method exists', function (done) {
      var client = new Client(new TransportCanClose());
      client.close(function () {
        done();
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_046: [** The `disconnect` method shall remove the listener that has been attached to the transport `disconnect` event.]*/
    it('unsubscribes for the \'disconnect\' event when disconnecting', function(done) {
      var transport = new TransportCanClose();
      var client = new Client(transport);
      var disconnectReceived = false;
      client.open(function() {
        client.on('disconnect', function() {
          disconnectReceived = true;
        });
        client.close(function() {
          transport.emit('disconnect');
          assert.isFalse(disconnectReceived);
          done();
        });
      });
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

      var client = new Client(new DummyTransport());
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

      var client = new Client(new DummyTransport());

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

      var client = new Client(new DummyTransport());
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
        var client = new Client(new DummyTransport());
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
      var client = new Client(new DummyTransport());
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

      var client = new Client(new DummyTransport());
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

      var client = new Client(new DummyTransport());
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
        var client = new Client(new DummyTransport());
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
      var client = new Client(new DummyTransport());
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

      var client = new Client(new DummyTransport());
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

      var client = new Client(new DummyTransport());
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
      var client = new Client(new DummyTransport());
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
      var client = new Client(new DummyTransport());
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

      var client = new Client(new DummyTransport());
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

      var client = new Client(new DummyTransport());
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
    var DummyBlobUploadClient = function() {};
    DummyBlobUploadClient.prototype.updateSharedAccessSignature = function() {};

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_031: [The updateSharedAccessSignature method shall throw a ReferenceError if the sharedAccessSignature parameter is falsy.]*/
    [undefined, null, '', 0].forEach(function (sas) {
      it('throws a ReferenceError if sharedAccessSignature is \'' + sas + '\'', function () {
        var DummyTransport = function () { };
        var client = new Client(new DummyTransport());
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
      var client = new Client(new DummyTransport(), null, new DummyBlobUploadClient());
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
        EventEmitter.call(this);
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
      util.inherits(DummyTransport, EventEmitter);

      var client = new Client(new DummyTransport(), null, new DummyBlobUploadClient());
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

      var client = new Client(new DummyTransport(), null, new DummyBlobUploadClient());
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

      var client = new Client(new DummyTransport(), null, new DummyBlobUploadClient());
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

      var client = new Client(new DummyTransport(), null, new DummyBlobUploadClient());
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

function makeConnectionString(host, device, key) {
  return 'HostName=' + host + ';DeviceId=' + device + ';SharedAccessKey=' + key;
}

var connectionString = makeConnectionString('host', 'device', 'key');
var badConnStrings = [
  makeConnectionString('bad', 'device', 'key'),
  makeConnectionString('host', 'bad', 'key'),
  makeConnectionString('host', 'device', 'bad')
];

describe('Over simulated HTTPS', function () {
  runTests(SimulatedHttp, connectionString, badConnStrings);
});
