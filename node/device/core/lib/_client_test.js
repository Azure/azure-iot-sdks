// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

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
    
    /*Tests_SRS_NODE_DEVICE_CLIENT_05_006: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(new Transport(...)).]*/
    it('returns an instance of Client', function () {
      var DummyTransport = function(){};
      var client = Client.fromConnectionString(connectionString, DummyTransport);
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
    
    /*Tests_SRS_NODE_DEVICE_HTTP_16_005: [The getReceiver method shall call the done method passed as argument with the receiver object as a parameter]*/ 
    /*Tests_SRS_NODE_DEVICE_HTTP_16_006: [The getReceiver method shall return the same unique instance if called multiple times in a row]*/ 
    describe('#getReceiver', function() {
      it('returns the same receiver instance if called multiple times', function() {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var firstInstance = null;
        var secondInstance = null;
        
        client.getReceiver(function(err, receiver) {
            assert.isNull(err);
            firstInstance = receiver;
        });
        
        client.getReceiver(function(err, receiver) {
            assert.isNull(err);
            secondInstance = receiver;
        });
        
        assert.isNotNull(firstInstance);
        assert.isNotNull(secondInstance);
        assert.equal(firstInstance, secondInstance);
      });
    });
    
    describe('#open', function () {
       var TransportCanOpen = function(config) {
           this.config = config;
       };
       
       TransportCanOpen.prototype.connect = function(callback) {
           callback();
       };
       
       /* Tests_SRS_NODE_DEVICE_CLIENT_12_001: [The open function shall call the transport’s connect function, if it exists.] */
       it('calls connect on the transport if the method exists', function (done) {
           var client = Client.fromConnectionString(goodConnectionString, TransportCanOpen);
           client.open(function() {
               done();
           });    
       });
    });
    
    describe('#close', function () {
       var TransportCanClose = function(config) {
           this.config = config;
       };
       
       TransportCanClose.prototype.disconnect = function(callback) {
           callback();
       };
       
       /* Tests_SRS_NODE_DEVICE_CLIENT_16_001: [The close function shall call the transport’s disconnect function if it exists.] */
       it('calls disconnect on the transport if the method exists', function (done) {
           var client = Client.fromConnectionString(goodConnectionString, TransportCanClose);
           client.close(function() {
               done();
           });    
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
