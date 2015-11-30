// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var Client = require('./client.js');
var Message = require('azure-iot-common').Message;
var SimulatedHttp = require('./http_simulated.js');

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

    /*Tests_SRS_NODE_DEVICE_CLIENT_05_003: [The fromConnectionString method shall throw ReferenceError if the value argument is falsy.]*/
    it('throws if value arg is falsy', function () {
      [null, undefined, '', 0].forEach(function (value) {
        assert.throws(function () {
          return Client.fromConnectionString(value);
        }, ReferenceError, 'value is \'' + value + '\'');
      });
    });
    
    /*Tests_SRS_NODE_DEVICE_CLIENT_05_004: [If Transport is falsy, fromConnectionString shall use the default transport, Http.]*/
    /*Tests_SRS_NODE_DEVICE_CLIENT_05_005: [fromConnectionString shall derive and transform the needed parts from the connection string in order to create a new instance of Transport.]*/
    it('creates an instance of the default transport', function () {
      var client = Client.fromConnectionString(connectionString);
      assert.instanceOf(client._transport, require('./http.js'));
    });
    
    /*Tests_SRS_NODE_DEVICE_CLIENT_05_006: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(new Transport(...)).]*/
    it('returns an instance of Client', function () {
      var client = Client.fromConnectionString(connectionString);
      assert.instanceOf(client, Client);
    });
  });
});

function badConfigTests(opName, badConnStrings, Transport, requestFn) {

  function makeRequestWith(connectionString, test, done) {
    var client = Client.fromConnectionString(connectionString, Transport);
    requestFn(client, function (err, res) {
      test(err, res);
      done();
    });
  }

  function expectNotFoundError(err) {
    assert.include(err.message, 'getaddrinfo ENOTFOUND bad');
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
          assert.isNull(err);
          assert.equal(res.statusCode, 204);
          done();
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
          assert.isNull(err);
          assert.equal(res.statusCode, 204);
          done();
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

    describe('#receive', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_009: [The receive method shall query the IoT Hub for the next message via the transport associated with the Client instance.]*/
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_018: [When receive completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - null
      message - the received Message (for receive), otherwise null
      response - a transport-specific response object]*/
      it('queries the service for a message', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        client.receive(function (err, msg, res) {
          assert.isNull(err);
          assert.isBelow(res.statusCode, 300);
          assert.instanceOf(msg, Message);
          done();
        });
      });

      badConfigTests('receive messages', badConnectionStrings, Transport, function (client, done) {
        client.receive(function (err, message, response) {
          done(err, response);
        });
      });
    });

    describe('#abandon', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_011: [Otherwise it shall invoke the done callback with ArgumentError.]*/
      it('fails if message doesn\'t have a lock token', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var message = new Message('hello');
        client.abandon(message, function (err) {
          assert.isNotNull(err);
          assert.instanceOf(err, ArgumentError);
          done();
        });
      });
      
      it('fails if the lock token doesn\'t match an outstanding message', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var message = new Message('hello');
        message.lockToken = 'FFA945D3-9808-4648-8DD7-D250DDE66EA9';
        client.abandon(message, function (err) {
          assert.isNotNull(err);
          assert.equal(err.response.statusCode, 412);
          done();
        });
      });

      badConfigTests('abandon', badConnectionStrings, Transport, function (client, done) {
        var message = new Message('hello');
        message.lockToken = '44D56613-DC1A-4DA8-906B-DDCC46090776';
        client.abandon(message, done);
      });
    });

    describe('#reject', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_013: [Otherwise is shall invoke the done callback with ArgumentError.]*/
      it('fails if message doesn\'t have a lock token', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var message = new Message('hello');
        client.reject(message, function (err) {
          assert.isNotNull(err);
          assert.instanceOf(err, ArgumentError);
          done();
        });
      });

      it('fails if the lock token doesn\'t match an outstanding message', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var message = new Message('hello');
        message.lockToken = 'FFA945D3-9808-4648-8DD7-D250DDE66EA9';
        client.reject(message, function (err) {
          assert.isNotNull(err);
          assert.equal(err.response.statusCode, 412);
          done();
        });
      });

      badConfigTests('reject', badConnectionStrings, Transport, function (client, done) {
        var message = new Message('hello');
        message.lockToken = '44D56613-DC1A-4DA8-906B-DDCC46090776';
        client.reject(message, done);
      });
    });

    describe('#complete', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_015: [Otherwise is shall invoke the done callback with ArgumentError.]*/
      it('fails if message doesn\'t have a lock token', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var message = new Message('hello');
        client.complete(message, function (err) {
          assert.isNotNull(err);
          done();
        });
      });

      it('fails if the lock token doesn\'t match an outstanding message', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var message = new Message('hello');
        message.lockToken = 'FFA945D3-9808-4648-8DD7-D250DDE66EA9';
        client.complete(message, function (err) {
          assert.isNotNull(err);
          assert.equal(err.response.statusCode, 412);
          done();
        });
      });

      badConfigTests('complete', badConnectionStrings, Transport, function (client, done) {
        var message = new Message('hello');
        message.lockToken = '44D56613-DC1A-4DA8-906B-DDCC46090776';
        client.complete(message, done);
      });
    });
    
    /*Tests_SRS_NODE_DEVICE_HTTP_16_005: [The getReceiver method shall call the done method passed as argument with the receiver object as a parameter]*/ 
    /*Tests_SRS_NODE_DEVICE_HTTP_16_006: [The getReceiver method shall return the same unique instance if called multiple times in a row]*/ 
    describe('#getReceiver', function() {
      it('returns the same receiver instance if called multiple times', function() {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var firstInstance = null;
        var secondInstance = null;
        
        client.getReceiver(function(receiver) {
          firstInstance = receiver;
        });
        
        client.getReceiver(function(receiver) {
          secondInstance = receiver;
        });
        
        assert.isNotNull(firstInstance);
        assert.isNotNull(secondInstance);
        assert.equal(firstInstance, secondInstance);
      });
    });
  });
}

function makeConnectionString(host, device, key) {
  return 'HostName='+host+';DeviceId='+device+';SharedAccessKey='+key;
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

module.exports = runTests;
