// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var Client = require('./client.js');
var Message = require('azure-iot-common').Message;
var SimulatedHttps = require('azure-iot-common').SimulatedHttps;

function badConfigTests(opName, badConnStrings, transportFactory, requestFn) {

  /*Tests_SRS_NODE_IOTHUB_CLIENT_05_003: [When the sendEvent method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback.]*/
  /*Tests_SRS_NODE_IOTHUB_CLIENT_05_005: [When the receive method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback.]*/
  function makeRequestWith(connString, test, done) {
    var transport = transportFactory();
    var client = new Client(connString, transport);
    requestFn(client, function (err, res, msg) {
      test(err, res, msg);
      done();
    });
  }

  /*Tests_SRS_NODE_IOTHUB_HTTPS_05_003: [If sendEvent encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  /*Tests_SRS_NODE_IOTHUB_HTTPS_05_007: [If receive encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  function expectNotFoundError(err) {
    assert.equal(err.message, 'getaddrinfo ENOTFOUND bad');
  }

  /*Tests_SRS_NODE_IOTHUB_HTTPS_05_004: [When sendEvent receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object
  res - the Node.js http.ServerResponse object returned by the transport]*/
  /*Tests_SRS_NODE_IOTHUB_HTTPS_05_008: [When receive receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object
  res - the Node.js http.ServerResponse object returned by the transport]*/
  function expect401Response(err, res) {
    assert.isNotNull(err);
    assert.equal(res.statusCode, 401);
  }

  function expect404Response(err, res) {
    assert.isNotNull(err);
    assert.equal(res.statusCode, 404);
  }

  var tests = [
    { name: 'hostname is malformed', expect: expectNotFoundError },
    { name: 'device is not registered', expect: expect404Response },
    { name: 'password is wrong', expect: expect401Response }
  ];

  badConnStrings.forEach(function (test, index, array) {
    it('fails to ' + opName + ' when the ' + tests[index].name, function (done) {
      makeRequestWith(test, tests[index].expect, done);
    });
  });
}

function runTests(transportFactory, goodConfig, badConfigs) {

  describe('Client', function () {
    describe('#sendEvent', function () {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_001: [The Client constructor shall accept a transport object]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_002: [The sendEvent method shall send the event (indicated by the message argument) via the transport associated with the Client instance.]*/
      /*Tests_SRS_NODE_IOTHUB_HTTPS_05_001: [The Https constructor shall accept a config object with three properties:
      host – (string) the fully-qualified DNS hostname of an IoT Hub
      keyName – (string) the identifier of a device registered with the IoT Hub, or the name of an authorization policy
      key – (string) the key associated with the device registration or authorization policy.]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_003: [When the sendEvent method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback.]*/
      /*Tests_SRS_NODE_IOTHUB_HTTPS_05_002: [The sendEvent method shall construct an HTTP request using information supplied by the caller, as follows:
      POST <config.host>/devices/<config.keyName>/messages/events?api-version=<version> HTTP/1.1
      Authorization: <token generated from config>
      iothub-to: /devices/<config.keyName>/messages/events
      Host: <config.host>

      <message>
      ]*/
      /*Tests_SRS_NODE_IOTHUB_HTTPS_05_005: [When sendEvent receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
      err - null
      res - the Node.js http.ServerResponse object returned by the transport]*/
      it('sends the event', function (done) {
        var client = new Client(goodConfig, transportFactory());
        var message = new Message('hello');
        client.sendEvent(message, function (err, res) {
          assert.isNull(err);
          assert.equal(res.statusCode, 204);
          done();
        });
      });

      badConfigTests('send an event', badConfigs, transportFactory, function (client, done) {
        client.sendEvent(new Message(''), done);
      });

    });

    describe('#sendEventBatch', function () {
      /* Test_SRS_NODE_IOTHUB_HTTPS_07_006: [The sendEventBatch method shall construct an HTTP request using information supplied by the caller, as follows:
      POST <config.host>/devices/<config.keyName>/messages/events?api-version=<version> HTTP/1.1
      Authorization: <token generated from config>
      iothub-to: /devices/<config.keyName>/messages/events
      Content-Type: ‘application/vnd.microsoft.iothub.json’
      Host: <config.host>
      {“body”:”<Base64 Message1>”,”properties”:{“<key>”:”<value”}},{ “body”:<Base64 Message1>”}…]*/
      /* Test_SRS_NODE_IOTHUB_HTTPS_07_007: [If sendEventBatch encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).
      When sendEventBatch receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
      err - the standard JavaScript Error object
      res - the Node.js http.ServerResponse object returned by the transport]*/
      /* Test_SRS_NODE_IOTHUB_HTTPS_07_008: [When sendEventBatch receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
      err - null
      res - the Node.js http.ServerResponse object returned by the transport]*/
      it('sends the event batch message', function (done) {
        var client = new Client(goodConfig, transportFactory());
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

      badConfigTests('send an event batch', badConfigs, transportFactory, function (client, done) {
        var messages = [];
        for (var i = 0; i < 5; i++) {
          messages[i] = new Message('Event Msg ' + i);
        }
        client.sendEventBatch(messages, done);
      });
    });

    describe('#receive', function () {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_001: [The Client constructor shall accept a transport object]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_004: [The receive method shall query the IoT Hub for the next message via the transport associated with the Client instance.]*/
      /*Tests_SRS_NODE_IOTHUB_HTTPS_05_001: [The Https constructor shall accept a config object with three properties:
      host – (string) the fully-qualified DNS hostname of an IoT Hub
      keyName – (string) the identifier of a device registered with the IoT Hub, or the name of an authorization policy
      key – (string) the key associated with the device registration or authorization policy.]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_005: [When the receive method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method’s callback.]*/
      /*Tests_SRS_NODE_IOTHUB_HTTPS_05_006: [The receive method shall construct an HTTP request using information supplied by the caller, as follows:
      GET <config.host>/devices/<config.keyName>/messages/devicebound?api-version=<version> HTTP/1.1
      Authorization: <token generated from config>
      iothub-to: /devices/<config.keyName>/messages/devicebound
      Host: <config.host>
      ]*/
      /*Tests_SRS_NODE_IOTHUB_HTTPS_05_009: [When receive receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
      err - null
      res - the Node.js http.ServerResponse object returned by the transport
      msg – the response body, i.e. the content of the message received from IoT Hub, as an iothub.Message object]*/
      it('queries the service for a message', function (done) {
        var client = new Client(goodConfig, transportFactory());
        client.receive(function (err, res, msg) {
          assert.isNull(err);
          assert.isBelow(res.statusCode, 300);
          assert.instanceOf(msg, Message);
          done();
        });
      });

      badConfigTests('receive messages', badConfigs, transportFactory, function (client, done) {
        client.receive(done);
      });
    });

    describe('#abandon', function () {
      /* Tests_SRS_NODE_IOTHUB_CLIENT_07_001: [The abandon method shall call into the transport’s sendFeedback with the ‘abandon’ keyword and the lockToken.] */
      it('message.lockToken is not entered', function (done) {
        var client = new Client(goodConfig, transportFactory());
        var message = new Message('hello');
        client.abandon(message, function (err, res) {
          assert.isNotNull(err);
          done();
        });
      });

      badConfigTests('abandon', badConfigs, transportFactory, function (client, done) {
        var message = new Message('hello');
        message.lockToken = '44D56613-DC1A-4DA8-906B-DDCC46090776';
        client.abandon(message, done);
      });
    });

    describe('#reject', function () {
      /* SRS_NODE_IOTHUB_CLIENT_07_002: [The reject method shall call into the transport’s sendFeedback with the ‘reject’ keyword and the lockToken.] */
      it('message.lockToken is not entered', function (done) {
        var client = new Client(goodConfig, transportFactory());
        var message = new Message('hello');
        client.reject(message, function (err, res) {
          assert.isNotNull(err);
          done();
        });
      });

      badConfigTests('reject', badConfigs, transportFactory, function (client, done) {
        var message = new Message('hello');
        message.lockToken = '44D56613-DC1A-4DA8-906B-DDCC46090776';
        client.reject(message, done);
      });
    });

    describe('#complete', function () {
      /* SRS_NODE_IOTHUB_CLIENT_07_003: [The complete method shall call into the transport’s sendFeedback with the ‘complete’ keyword and the lockToken.] */
      it('message.lockToken is not entered', function (done) {
        var client = new Client(goodConfig, transportFactory());
        var message = new Message('hello');
        client.complete(message, function (err, res) {
          assert.isNotNull(err);
          done();
        });
      });

      badConfigTests('complete', badConfigs, transportFactory, function (client, done) {
        var message = new Message('hello');
        message.lockToken = '44D56613-DC1A-4DA8-906B-DDCC46090776';
        client.complete(message, done);
      });
    });
  });
}

function createTransport() {
  return new SimulatedHttps();
}

function makeConnectionString(host, device, key) {
  return 'HostName='+host+';CredentialType=SharedAccessKey;CredentialScope=Device;DeviceId='+device+';SharedAccessKey='+key;
}

var connectionString = makeConnectionString('host', 'device', 'key');
var badConnStrings = [
  makeConnectionString('bad', 'device', 'key'),
  makeConnectionString('host', 'bad', 'key'),
  makeConnectionString('host', 'device', 'bad')
];

describe('Over simulated HTTPS', function () {
  runTests(createTransport, connectionString, badConnStrings);
});

module.exports = runTests;
