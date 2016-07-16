// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Message = require('azure-iot-common').Message;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var HttpReceiver = require('../lib/http_receiver.js');
var sinon = require('sinon');

var FakeHttp = function () { };

FakeHttp.prototype.buildRequest = function (method, path, httpHeaders, host, sslOptions, done) {
  return {
    end: function () {
      if (this.messageCount > 0) {
        this.messageCount--;
        done(null, "foo", { statusCode: 204 });
      } else {
        done(null, "", { statusCode: 204 });
      }
    }.bind(this)
  };
};

FakeHttp.prototype.toMessage = function () { };

FakeHttp.prototype.setMessageCount = function (messageCount) {
  this.messageCount = messageCount;
};

describe('HttpReceiver', function () {
  describe('#receiveTimers', function () {
    beforeEach(function () {
      var config = { deviceId: "deviceId", hubName: "hubName", host: "hubname.azure-devices.net", sharedAccessSignature: "sas" };
      this.fakeHttp = new FakeHttp();
      this.receiver = new HttpReceiver(config, this.fakeHttp);
      this.clock = sinon.useFakeTimers();
    });

    afterEach(function () {
      this.receiver = null;
      this.fakeHttp = null;
      this.clock.restore();
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_021: [If opts.interval is set, messages should be received repeatedly at that interval]*/
    it('receives messages after the set interval', function (done) {
      var messageCount = 2;
      var messageReceivedCount = 0;
      this.fakeHttp.setMessageCount(messageCount);
      this.receiver.setOptions({ interval: 5 });
      this.receiver.on('message', function () {
        messageReceivedCount++;
        if (messageReceivedCount === messageCount) {
          done();
        }
      });
      this.clock.tick(4999);
      assert.strictEqual(messageReceivedCount, 0);
      this.clock.tick(5000);
      assert.strictEqual(messageReceivedCount, 1);
      this.clock.tick(1);
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_003: [if opts.at is set, messages shall be received at the Date and time specified.]*/
    it('receives messages at the set time', function (done) {
      var messageReceived = false;
      this.fakeHttp.setMessageCount(1);
      var inFiveSeconds = new Date((new Date()).getTime() + 5000);
      this.receiver.setOptions({ at: inFiveSeconds });
      this.receiver.on('message', function () {
        messageReceived = true;
        done();
      });
      this.clock.tick(4999);
      assert.isFalse(messageReceived);
      this.clock.tick(1);
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_020: [If opts.cron is set messages shall be received according to the schedule described by the expression.]*/
    it('receives messages as configured with a cron string', function (done) {
      var messageReceivedCount = 0;
      var messageCount = 2;
      this.fakeHttp.setMessageCount(messageCount);
      var everyMinute = "* * * * *";
      this.receiver.setOptions({ cron: everyMinute });
      this.receiver.on('message', function () {
        messageReceivedCount++;
        if (messageReceivedCount === messageCount) {
          done();
        }
      });
      this.clock.tick(59999);
      assert.strictEqual(messageReceivedCount, 0);
      this.clock.tick(60000);
      assert.strictEqual(messageReceivedCount, 1);
      this.clock.tick(1);
    });
  });

  describe('#receive', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_023: [If opts.manualPolling is true, messages shall be received only when receive() is called]*/
    it('receives 1 message when receive() is called and drain is false', function (done) {
      var config = { deviceId: "deviceId", hubName: "hubName", host: "hubname.azure-devices.net", sharedAccessSignature: "sas" };
      var fakeHttp = new FakeHttp();
      var receiver = new HttpReceiver(config, fakeHttp);
      var msgCount = 5;
      fakeHttp.setMessageCount(msgCount);
      receiver.setOptions({ manualPolling: true, drain: false });
      var received = 0;
      receiver.on('message', function () {
        received++;
        if (received === msgCount) done();
      });

      receiver.receive();
      receiver.receive();
      receiver.receive();
      receiver.receive();
      receiver.receive();
    });

    it('receives all messages when receive() is called and drain is true', function (done) {
      var config = { deviceId: "deviceId", hubName: "hubName", host: "hubname.azure-devices.net", sharedAccessSignature: "sas" };
      var fakeHttp = new FakeHttp();
      var receiver = new HttpReceiver(config, fakeHttp);
      var msgCount = 5;
      fakeHttp.setMessageCount(msgCount);
      receiver.setOptions({ manualPolling: true, drain: true });
      var received = 0;
      receiver.on('message', function () {
        received++;
        if (received === msgCount) done();
      });
      receiver.receive();
    });

    it('emits messages only when all requests are done', function(done){
      var config = { deviceId: "deviceId", hubName: "hubName", host: "hubname.azure-devices.net", sharedAccessSignature: "sas" };
      var fakeHttp = new FakeHttp();
      var requestsCount = 0;
      fakeHttp.buildRequest = function (method, path, httpHeaders, host, sslOptions, done) {
        requestsCount++;
        return {
          end: function () {
            if (this.messageCount > 0) {
              this.messageCount--;
              done(null, "foo", { statusCode: 204 });
            } else {
              done(null, "", { statusCode: 204 });
            }
          }.bind(this)
        };
      };

      var receiver = new HttpReceiver(config, fakeHttp);
      fakeHttp.setMessageCount(1);
      receiver.setOptions({ manualPolling: true, drain: true });
      receiver.on('message', function () {
        assert.equal(requestsCount, 2);
        done();
      });
      receiver.receive();
    });
  });

  describe('#drain', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_017: [If opts.drain is true all messages in the queue should be pulled at once.]*/
    it('drains the message queue', function (done) {
      var config = { deviceId: "deviceId", hubName: "hubName", host: "hubname.azure-devices.net", sharedAccessSignature: "sas" };
      var fakeHttp = new FakeHttp();
      var receiver = new HttpReceiver(config, fakeHttp);
      var msgCount = 5;
      fakeHttp.setMessageCount(msgCount);
      receiver.setOptions({ at: new Date(), drain: true });
      var received = 0;
      receiver.on('message', function () {
        received++;
        if (received === msgCount) done();
      });
    });
  });

  describe('#setOptions', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_002: [opts.interval is not a number, an ArgumentError should be thrown.]*/
    it('throws if opts.interval is not a number', function () {
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: "foo", at: null, cron: null, drain: false });
      }, ArgumentError);
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_005: [If opts.interval is a negative number, an ArgumentError should be thrown.]*/
    it('throws if opts.interval is negative', function () {
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: -10, at: null, cron: null, drain: false });
      }, ArgumentError);
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_022: [If opts.at is not a Date object, an ArgumentError should be thrown] */
    it('throws if opts.at is not a date', function () {
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: null, at: "foo", cron: null, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: null, at: 42, cron: null, drain: false });
      }, ArgumentError);
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_008: [Only one of the interval, at, and cron fields should be populated: if more than one is populated, an ArgumentError shall be thrown.]*/
    it('throws if more than one option is specified', function () {
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: new Date(), cron: "* * * * *", manualPolling: true, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: null, at: new Date(), cron: "* * * * *", manualPolling: true, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: null, cron: "* * * * *", manualPolling: true, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: new Date(), cron: null, manualPolling: true, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: new Date(), cron: "* * * * *", manualPolling: false, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: null, at: null, cron: "* * * * *", manualPolling: true, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: null, cron: null, manualPolling: true, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: new Date(), cron: null, manualPolling: false, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: null, at: new Date(), cron: null, manualPolling: true, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: null, cron: "* * * * *", manualPolling: false, drain: false });
      }, ArgumentError);
      assert.throws(function () {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: null, at: new Date(), cron: "* * * * *", manualPolling: false, drain: false });
      }, ArgumentError);
    });
  });

  describe('abandon', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_025: [If message is falsy, `abandon` should throw a ReferenceException]*/
    it('throws if message is falsy', function () {
      var receiver = new HttpReceiver();
      assert.throws(function () {
        receiver.abandon(null);
      }, ReferenceError, 'Invalid message object.');
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_024: [When successful, `abandon` should call the done callback with a null error object and a result object of type `MessageAbandoned`]*/
    it('calls the done() callback with a null error object and a result of type MessageAbandoned', function (done) {
      var transport = new FakeHttp();
      var receiver = new HttpReceiver({ deviceId: 'deviceId', sharedAccessSignature: 'sharedAccessSignature' }, transport);
      var msg = new Message();
      msg.lockToken = 'foo';
      receiver.abandon(msg, function (err, result) {
        assert.isNull(err);
        assert.equal(result.constructor.name, 'MessageAbandoned');
        done();
      });
    });
  });

  describe('reject', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_026: [If message is falsy, `reject` should throw a ReferenceException] */
    it('throws if message is falsy', function () {
      var receiver = new HttpReceiver();
      assert.throws(function () {
        receiver.reject(null);
      }, ReferenceError, 'Invalid message object.');
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_029: [When successful, `reject` should call the done callback with a null error object and a result object of type `MessageRejected`] */
    it('calls the done() callback with a null error object and a result of type MessageRejected', function (done) {
      var transport = new FakeHttp();
      var receiver = new HttpReceiver({ deviceId: 'deviceId', sharedAccessSignature: 'sharedAccessSignature' }, transport);
      var msg = new Message();
      msg.lockToken = 'foo';
      receiver.reject(msg, function (err, result) {
        assert.isNull(err);
        assert.equal(result.constructor.name, 'MessageRejected');
        done();
      });
    });
  });

  describe('complete', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_027: [If message is falsy, ` complete ` should throw a ReferenceException] */
    it('throws if message is falsy', function () {
      var receiver = new HttpReceiver();
      assert.throws(function () {
        receiver.complete(null);
      }, ReferenceError, 'Invalid message object.');
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_028: [When successful, `complete` should call the done callback with a null error object and a result object of type `MessageCompleted`] */
    it('calls the done() callback with a null error object and a result of type MessageCompleted', function (done) {
      var transport = new FakeHttp();
      var receiver = new HttpReceiver({ deviceId: 'deviceId', sharedAccessSignature: 'sharedAccessSignature' }, transport);
      var msg = new Message();
      msg.lockToken = 'foo';
      receiver.complete(msg, function (err, result) {
        assert.isNull(err);
        assert.equal(result.constructor.name, 'MessageCompleted');
        done();
      });
    });
  });

  describe('updateSharedAccessSignature', function() {
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_032: [** `updateSharedAccessSignature` shall throw a `ReferenceError` if the `sharedAccessSignature` argument is falsy.]*/
    [null, undefined, ''].forEach(function(invalidSas) {
      it('throws if sharedAccessSignature is \'' + invalidSas + '\'', function() {
        var transport = new FakeHttp();
        var receiver = new HttpReceiver({ deviceId: 'deviceId', sharedAccessSignature: 'sharedAccessSignature' }, transport);
        assert.throws(function() {
          receiver.updateSharedAccessSignature(invalidSas);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_033: [** All subsequent HTTP requests shall use the value of the `sharedAccessSignature` argument in their headers.]*/
    it('uses the new sharedAccessSignature in subsequent HTTP requests', function(done) {
        var newSignature = 'newSignature';
        var transport = new FakeHttp();
        transport.buildRequest = function (method, path, httpHeaders) {
          assert.equal(httpHeaders.Authorization, newSignature);
          return {
            end: function () {
              done();
            }
          };
        };

        var receiver = new HttpReceiver({ deviceId: 'deviceId', sharedAccessSignature: 'sharedAccessSignature' }, transport);
        receiver.updateSharedAccessSignature(newSignature);
        receiver.receive();
    });
  });
});