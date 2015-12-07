// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var HttpReceiver = require('./http_receiver.js');
var sinon = require('sinon');

var FakeHttp = function () { };

FakeHttp.prototype.buildRequest = function (method, path, httpHeaders, host, done) {
  return {
    end: function () {
      if (this.messageCount > 0) {
        this.messageCount--;
        done(null, "foo");
      } else {
        done(null, "");
      }
    }.bind(this)
  };
};

FakeHttp.prototype.toMessage = function() { };

FakeHttp.prototype.setMessageCount = function (messageCount) {
  this.messageCount = messageCount;
};

describe('HttpReceiver', function () {
  describe('#receiveTimers', function() {
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
    it('receives messages after the set interval', function(done) {
      var messageCount = 2;
      var messageReceivedCount = 0;
      this.fakeHttp.setMessageCount(messageCount);
      this.receiver.setOptions({interval: 5});
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
    it('receives messages at the set time', function(done) {
      var messageReceived = false;
      this.fakeHttp.setMessageCount(1);      
      var inFiveSeconds = new Date((new Date()).getTime() + 5000);
      this.receiver.setOptions({at: inFiveSeconds });
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
      this.receiver.setOptions({cron: everyMinute });
      this.receiver.on('message', function () {
        messageReceivedCount++;
        if(messageReceivedCount === messageCount) {
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
  
  describe('#drain', function() {
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_017: [If opts.drain is true all messages in the queue should be pulled at once.]*/
    it('drains the message queue', function (done) {
      var config = { deviceId: "deviceId", hubName: "hubName", host: "hubname.azure-devices.net", sharedAccessSignature: "sas" };
      var fakeHttp = new FakeHttp();
      var receiver = new HttpReceiver(config, fakeHttp);
      var msgCount = 5;
      fakeHttp.setMessageCount(msgCount);
      receiver.setOptions({ at: new Date(), drain: true });
      var received = 0;
      receiver.on('message', function() {
        received++;
        if (received === msgCount) done();
      });
    });  
  });
  
  describe('#setOptions', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_002: [opts.interval is not a number, an ArgumentError should be thrown.]*/ 
    it('throws if opts.interval is not a number', function() {
      assert.throws(function() {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: "foo", at: null, cron: null, drain: false });
      }, ArgumentError);
    });
    
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_005: [If opts.interval is a negative number, an ArgumentError should be thrown.]*/
    it('throws if opts.interval is negative', function() {
      assert.throws(function() {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: -10, at: null, cron: null, drain: false });
      }, ArgumentError);
    });
    
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_022: [If opts.at is not a Date object, an ArgumentError should be thrown] */ 
    it('throws if opts.at is not a date', function() {
      assert.throws(function() {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: null, at: "foo", cron: null, drain: false });
      }, ArgumentError);
      assert.throws(function() {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: null, at: 42, cron: null, drain: false });
      }, ArgumentError);
    });
    
    /*Tests_SRS_NODE_DEVICE_HTTP_RECEIVER_16_008: [Only one of the interval, at, and cron fields should be populated: if more than one is populated, an ArgumentError shall be thrown.]*/
    it('throws if more than one option is specified', function() {
      assert.throws(function() {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: new Date(), cron: "* * * * *", drain: false });
      }, ArgumentError);
      assert.throws(function() {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: null, at: new Date(), cron: "* * * * *", drain: false });
      }, ArgumentError);  
      assert.throws(function() {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: null, cron: "* * * * *", drain: false });
      }, ArgumentError);   
      assert.throws(function() {
        var receiver = new HttpReceiver();
        receiver.setOptions({ interval: 42, at: new Date(), cron: null, drain: false });
      }, ArgumentError);   
    });
  });
});