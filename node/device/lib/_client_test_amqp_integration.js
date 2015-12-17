// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var Client = require('./client.js');
var ConnectionString = require('azure-iot-common').ConnectionString;
var Message = require('azure-iot-common').Message;
var Amqp = require('./amqp.js');
var AmqpWS = require('./amqp_ws.js');

var host = ConnectionString.parse(process.env.IOTHUB_CONNECTION_STRING).HostName;
var deviceId = process.env.IOTHUB_DEVICE_ID;
var key = process.env.IOTHUB_DEVICE_KEY;

var connectionString = 'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=' + key;
var badConnStrings = [
  'HostName=bad;DeviceId=' + deviceId + ';SharedAccessKey=' + key,
  'HostName=' + host + ';DeviceId=bad;SharedAccessKey=' + key,
  'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=bad;'
];

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
  
  function expectTimeout(err) {
    assert.isNotNull(err);
    assert.include(err.message, "ETIMEDOUT");
  }

  var tests = [
    { name: 'hostname is malformed', expect: expectNotFoundError },
    { name: 'device is not registered', expect: expectTimeout },
    { name: 'password is wrong', expect: expectTimeout }
  ];

  /*Tests_SRS_NODE_DEVICE_CLIENT_05_016: [When a Client method encounters an error in the transport, the callback function (indicated by the done argument) shall be invoked with the following arguments:
  err - the standard JavaScript Error object, with a response property that points to a transport-specific response object, and a responseBody property that contains the body of the transport response.]*/
  badConnStrings.forEach(function (test, index) {
    it.skip('fails to ' + opName + ' when the ' + tests[index].name, function (done) {
      this.timeout(30000);
      makeRequestWith(test, tests[index].expect, done);
    });
  });
}

function runAmqpTests(Transport, goodConnectionString, badConnectionStrings) {

  describe('Client', function () {
    describe('#sendEvent', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_007: [The sendEvent method shall send the event indicated by the message argument via the transport associated with the Client instance.]*/
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_017: [With the exception of receive, when a Client method completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
        err - null
        response - a transport-specific response object]*/
      /*Tests_SRS_NODE_DEVICE_AMQP_16_002: [The sendEvent method shall construct an AMQP request using the message passed in argument as the body of the message.] */ 
      /*Tests_SRS_NODE_DEVICE_AMQP_16_003: [The sendEvent method shall call the done() callback with no arguments when the message has been successfully sent.] */ 
      /*Tests_SRS_NODE_DEVICE_AMQP_16_004: [If sendEvent encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message). ] */
      it('sends the event', function (done) {
        this.timeout(15000);
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var message = new Message('hello');
        client.sendEvent(message, function (err, res) {
          assert.isNull(err);
          assert.isNotNull(res);
          done();
        });
      });

      badConfigTests('send an event', badConnectionStrings, Transport, function (client, done) {
        client.sendEvent(new Message(''), done);
      });

    });
	
    /*Tests_SRS_NODE_DEVICE_AMQP_16_006: [If a receiver for this endpoint has already been created, the getReceiver method should call the done() method with the existing instance as an argument.]*/ 
    /*Tests_SRS_NODE_DEVICE_AMQP_16_007: [If a receiver for this endpoint doesn’t exist, the getReceiver method should create a new AmqpReceiver object and then call the done() method with the object that was just created as an argument.]*/ 
    describe('#getReceiver', function() {
      it('returns the same receiver instance if called multiple times', function(done) {
        this.timeout(15000);
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        
        client.getReceiver(function(err1, receiver1) {
          client.getReceiver(function(err2, receiver2) {
            assert.isNotNull(receiver1);
            assert.isNotNull(receiver2);
            assert.equal(receiver1, receiver2);
            
            done();
          });
        });
      });
    });
    
  });
}

function makeConnectionString(host, device, key) {
  return 'HostName='+host+';DeviceId='+device+';SharedAccessKey='+key;
}

var badConnStrings = [
  makeConnectionString('bad', 'device', 'key'),
  makeConnectionString('host', 'bad', 'key'),
  makeConnectionString('host', 'device', 'bad')
];

describe('Over AMQPS', function () {
  runAmqpTests(Amqp, connectionString, badConnStrings);
});

describe('Over AMQP/WS', function () {
  runAmqpTests(AmqpWS, connectionString, badConnStrings);
});

module.exports = runAmqpTests;
