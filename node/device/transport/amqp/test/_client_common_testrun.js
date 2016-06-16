// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ConnectionString = require('azure-iot-common').ConnectionString;
var assert = require('chai').assert;
var Client = require('azure-iot-device').Client;
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
      /*Tests_SRS_NODE_DEVICE_AMQP_16_002: [The sendEvent method shall construct an AMQP request using the message passed in argument as the body of the message.] */
      /*Tests_SRS_NODE_DEVICE_AMQP_16_003: [The sendEvent method shall call the done() callback with no arguments when the message has been successfully sent.] */
      /*Tests_SRS_NODE_DEVICE_AMQP_16_004: [If sendEvent encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message). ] */
      it('sends the event', function (done) {
        var client = Client.fromConnectionString(goodConnectionString, Transport);
        setx509OptionsIfSpecified(client);
        client.open(function (err, res) {
          if (err) {
            done(err);
          } else {
            assert.equal(res.constructor.name, 'Connected', 'Type of the result object of the client.open method is wrong');
            var message = new Message('hello');
            client.sendEvent(message, function (err, res) {
              if (err) {
                done(err);
              } else {
                assert.equal(res.constructor.name, 'MessageEnqueued', 'Type of the result object of the client.sendEvent method is wrong');
                client.close(function (err) {
                  done(err);
                });
              }
            });
          }
        });
      });

      badConfigTests('send an event', badConnectionStrings, Transport, function (client, done) {
        client.sendEvent(new Message(''), done);
      });
    });
  });
}

module.exports = runTests;