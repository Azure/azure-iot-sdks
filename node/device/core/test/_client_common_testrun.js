// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var Client = require('../lib/client.js');
var ConnectionString = require('../lib/connection_string.js');
var Message = require('azure-iot-common').Message;

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

function runTests(Transport, goodConnectionString, badConnectionStrings, certificate, key, passphrase) {
  var cn = ConnectionString.parse(goodConnectionString);
  var x509 = !!cn.x509;

  var setx509OptionsIfSpecified = function(client) {
    if(x509) {
      client.setOptions({
        cert: certificate,
        key: key,
        passphrase: passphrase
      });
    }
  };

  describe('Client', function () {
    describe('#sendEvent', function () {
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_007: [The sendEvent method shall send the event indicated by the message argument via the transport associated with the Client instance.]*/
      /*Tests_SRS_NODE_DEVICE_CLIENT_05_017: [With the exception of receive, when a Client method completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - null
      response - a transport-specific response object]*/
      it('sends the event using shared access signature', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        var message = new Message('hello');

        setx509OptionsIfSpecified(client);

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

        setx509OptionsIfSpecified(client);

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
  });
}

module.exports = runTests;