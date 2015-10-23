// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Client = require('./client.js');
var Message = require('azure-iot-common').Message;
var errors = require('azure-iot-common').errors;

describe('Client', function () {
  describe('#constructor', function () {
    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_001: [The Client constructor shall throw ReferenceError if the transport argument is falsy.]*/
    it('throws when transport is falsy', function () {
      assert.throws(function () {
        return new Client();
      }, ReferenceError, 'transport is \'undefined\'');
    });
  });

  describe('#fromConnectionString', function () {
    var connStr = 'HostName=a.b.c;SharedAccessKeyName=name;SharedAccessKey=key';

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_002: [The fromConnectionString method shall throw ReferenceError if the value argument is falsy.]*/
    it('throws when value is falsy', function () {
      assert.throws(function () {
        return Client.fromConnectionString();
      }, ReferenceError, 'value is \'undefined\'');
    });

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_003: [Otherwise, it shall derive and transform the needed parts from the connection string in order to create a new instance of the default transport (azure-iothub.Transport).]*/
    it('creates an instance of the default transport', function () {
      var client = Client.fromConnectionString(connStr);
      assert.instanceOf(client._transport, require('./transport.js'));
    });

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_004: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(transport).]*/
    it('returns an instance of Client', function () {
      var client = Client.fromConnectionString(connStr);
      assert.instanceOf(client, Client);
    });
  });

  describe('#fromSharedAccessSignature', function () {
    var token = 'SharedAccessSignature sr=audience&sig=signature&se=expiry&skn=keyname';

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_005: [The fromSharedAccessSignature method shall throw ReferenceError if the value argument is falsy.]*/
    it('throws when value is falsy', function () {
      assert.throws(function () {
        return Client.fromSharedAccessSignature();
      }, ReferenceError, 'value is \'undefined\'');
    });

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_006: [Otherwise, it shall derive and transform the needed parts from the shared access signature in order to create a new instance of the default transport (azure-iothub.Transport).]*/
    it('creates an instance of the default transport', function () {
      var client = Client.fromSharedAccessSignature(token);
      assert.instanceOf(client._transport, require('./transport.js'));
    });

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_007: [The fromSharedAccessSignature method shall return a new instance of the Client object, as by a call to new Client(transport).]*/
    it('returns an instance of Client', function () {
      var client = Client.fromSharedAccessSignature(token);
      assert.instanceOf(client, Client);
    });
  });

  describe('#send', function () {
    var testSubject;

    beforeEach('prepare test subject', function () {
      testSubject = new Client({});
    });

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_013: [The send method shall throw ReferenceError if the deviceId or message arguments are falsy.]*/
    it('throws if deviceId is falsy', function () {
      assert.throws(function () {
        testSubject.send();
      }, ReferenceError, 'deviceId is \'undefined\'');
    });

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_013: [The send method shall throw ReferenceError if the deviceId or message arguments are falsy.]*/
    it('throws if message is falsy', function () {
      assert.throws(function () {
        testSubject.send('id');
      }, ReferenceError, 'message is \'undefined\'');
    });

    it('does not throw if done is falsy', function () {
      assert.doesNotThrow(function () {
        testSubject.send('id', new Message('msg'));
      }, ReferenceError);
    });
  });
});

function transportSpecificTests(opts) {
  describe('Client', function () {
    var testSubject;
    var deviceId;

    beforeEach('prepare test subject', function () {
      if (opts.transport && opts.connectionString) assert.fail('test setup error');
      if (opts.transport) testSubject = new Client(opts.transport);
      else testSubject = Client.fromConnectionString(opts.connectionString);
      deviceId = opts.id;
    });

    afterEach('close the connection', function (done) {
      testSubject.close(function (err) { done(err); });
    });

    describe('#open', function () {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_008: [The open method shall open a connection to the IoT Hub that was identified when the Client object was created (e.g., in Client.fromConnectionString).]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_009: [When the open method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - standard JavaScript Error object (or subclass)]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_010: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
      it('opens a connection to the IoT Hub', function (done) {
        testSubject.open(function (err) {
          done(err);
        });
      });
    });

    describe('#close', function () {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_021: [The close method shall close the connection.]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_022: [When the close method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - standard JavaScript Error object (or subclass)]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_023: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
      it('closes the connection to the IoT Hub', function (done) {
        testSubject.open(function (err) {
          if (err) done(err);
          else {
            testSubject.close(function (err) {
              done(err);
            });
          }
        });
      });
    });

    describe('#send', function () {
      function createTestMessage() {
        var msg = new Message('msg');
        msg.expiryTimeUtc = Date.now() + 5000; // Expire 5s from now, to reduce the chance of us hitting the 50-message limit on the IoT Hub
        return msg;
      }

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_016: [When the send method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - standard JavaScript Error object (or subclass)
      response - an implementation-specific response object returned by the underlying protocol, useful for logging and troubleshooting]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_017: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
      it('sends the message', function (done) {
        testSubject.open(function (err) {
          if (err) done(err);
          else {
            testSubject.send(deviceId, createTestMessage(), function (err, state) {
              if (!err) {
                assert.equal(state.descriptor.toString(), 0x24); // 0x24 == AMQP's 'accepted' disposition frame
              }
              done(err);
            });
          }
        });
      });

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_015: [If the connection has not already been opened (e.g., by a call to open), the send method shall open the connection before attempting to send the message.]*/
      it('opens if necessary', function (done) {
        testSubject.send(deviceId, createTestMessage(), function (err, state) {
          if (!err) {
            assert.equal(state.descriptor.toString(), 0x24); // 0x24 == AMQP's 'accepted' disposition frame
          }
          done(err);
        });
      });

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_015: [If the connection has not already been opened (e.g., by a call to open), the send method shall open the connection before attempting to send the message.]*/
      it('reopens if necessary', function (done) {
        testSubject.open(function (err) {
          if (err) done(err);
          else {
            testSubject.close(function (err) {
              if (err) done(err);
              else {
                testSubject.send(deviceId, createTestMessage(), function (err, state) {
                  if (!err) {
                    assert.equal(state.descriptor.toString(), 0x24); // 0x24 == AMQP's 'accepted' disposition frame
                  }
                  done(err);
                });
              }
            });
          }
        });
      });

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_014: [The send method shall convert the message object to type azure-iot-common.Message if necessary.]*/
      it('accepts any message that is convertible to type Message', function (done) {
        var message = 'msg';
        testSubject.send(deviceId, message, function (err, state) {
          if (!err) {
            assert.equal(state.descriptor.toString(), 0x24); // 0x24 == AMQP's 'accepted' disposition frame
          }
          done(err);
        });
      });

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_019: [If the deviceId has not been registered with the IoT Hub, send shall return an instance of DeviceNotFoundError.]*/
      it('returns DeviceNotFoundError when sending to an unregistered deviceId', function (done) {
        var unregisteredDeviceId = 'no-device' + Math.random();
        testSubject.send(unregisteredDeviceId, new Message('msg'), function (err) {
          assert.instanceOf(err, errors.DeviceNotFoundError);
          done();
        });
      });
    });
  });
}

module.exports = transportSpecificTests;

describe('Over simulated AMQP', function () {
  var SimulatedTransport = require('./transport_simulated.js');
  var opts = {
    transport: new SimulatedTransport(),
    connectionString: null,
    id: 'id'
  };
  transportSpecificTests(opts);
});