// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Amqp = require('../lib/amqp.js');
var Client = require('../lib/client.js');
var Message = require('azure-iot-common').Message;
var SimulatedAmqp = require('./amqp_simulated.js');
var transportSpecificTests = require('./_client_common_testrun.js');

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

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_002: [The fromConnectionString method shall throw ReferenceError if the connStr argument is falsy.]*/
    it('throws when value is falsy', function () {
      assert.throws(function () {
        return Client.fromConnectionString();
      }, ReferenceError, 'connStr is \'undefined\'');
    });

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_003: [Otherwise, it shall derive and transform the needed parts from the connection string in order to create a new instance of the default transport (azure-iothub.Transport).]*/
    it('creates an instance of the default transport', function () {
      var client = Client.fromConnectionString(connStr);
      assert.instanceOf(client._transport, Amqp);
    });

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_004: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(transport).]*/
    it('returns an instance of Client', function () {
      var client = Client.fromConnectionString(connStr);
      assert.instanceOf(client, Client);
    });
  });

  describe('#fromSharedAccessSignature', function () {
    var token = 'SharedAccessSignature sr=hubName.azure-devices.net&sig=signature&skn=keyname&se=expiry';

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_005: [The fromSharedAccessSignature method shall throw ReferenceError if the sharedAccessSignature argument is falsy.]*/
    it('throws when value is falsy', function () {
      assert.throws(function () {
        return Client.fromSharedAccessSignature();
      }, ReferenceError, 'sharedAccessSignature is \'undefined\'');
    });

    it('correctly populates the config structure', function() {
      var client = Client.fromSharedAccessSignature(token);
      assert.equal(client._transport._config.hubName, 'hubName');
      assert.equal(client._transport._config.host, 'hubName.azure-devices.net');
      assert.equal(client._transport._config.keyName, 'keyname');
      assert.equal(client._transport._config.sharedAccessSignature, token);
    });

    /*Tests_SRS_NODE_IOTHUB_CLIENT_05_006: [Otherwise, it shall derive and transform the needed parts from the shared access signature in order to create a new instance of the default transport (azure-iothub.Transport).]*/
    it('creates an instance of the default transport', function () {
      var client = Client.fromSharedAccessSignature(token);
      assert.instanceOf(client._transport, Amqp);
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

  describe('#open', function() {
    /*Tests_SRS_NODE_IOTHUB_CLIENT_16_004: [The `disconnect` event shall be emitted when the client is disconnected from the server.]*/
    /*Tests_SRS_NODE_IOTHUB_CLIENT_16_002: [If the transport successfully establishes a connection the `open` method shall subscribe to the `disconnect` event of the transport.]*/
    it('subscribes to the \'disconnect\' event once connected', function(done) {
      var simulatedAmqp = new SimulatedAmqp();
      var client = new Client(simulatedAmqp);
      client.open(function() {
        client.on('disconnect', function() {
          done();
        });

        simulatedAmqp.emit('disconnect');
      });
    });
  });

  describe('#close', function() {
    /*Tests_SRS_NODE_IOTHUB_CLIENT_16_003: [The `close` method shall remove the listener that has been attached to the transport `disconnect` event.]*/
    it('unsubscribes for the \'disconnect\' event when disconnecting', function(done) {
      var simulatedAmqp = new SimulatedAmqp();
      var client = new Client(simulatedAmqp);
      var disconnectReceived = false;
      client.open(function() {
        client.on('disconnect', function() {
          disconnectReceived = true;
        });
        client.close(function() {
          simulatedAmqp.emit('disconnect');
          assert.isFalse(disconnectReceived);
          done();
        });
      });
    });
  });
});

describe('Over simulated AMQP', function () {
  var opts = {
    transport: function () { return new SimulatedAmqp(); },
    connectionString: process.env.IOTHUB_CONNECTION_STRING,
    id: 'id'
  };
  transportSpecificTests(opts);
});