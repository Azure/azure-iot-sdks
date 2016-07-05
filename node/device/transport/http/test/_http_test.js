// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var sinon = require('sinon');

var Message = require('azure-iot-common').Message;
var Http = require('../lib/http.js');

describe('Http', function () {
  var transport = null;
  var receiver = null;
  var testMessage = new Message();
  var testCallback = function () { };

  beforeEach(function () {
    var DummyReceiver = function () {
      this.complete = sinon.spy();
      this.reject = sinon.spy();
      this.abandon = sinon.spy();
      this.setOptions = sinon.spy();
      this.updateSharedAccessSignature = sinon.spy();
    };

    receiver = new DummyReceiver();

    transport = new Http({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
    transport._receiver = receiver;
  });

  afterEach(function () {
    transport = null;
    receiver = null;
  });

  describe('#setOptions', function () {
    var testOptions = {
      http: {
        receivePolicy: {interval: 1}
      }
    };

    /*Tests_SRS_NODE_DEVICE_HTTP_16_004: [The `setOptions` method shall call the `setOptions` method of the HTTP Receiver with the content of the `http.receivePolicy` property of the `options` parameter.]*/
    it('calls the receiver `setOptions` method', function (done) {
      transport.setOptions(testOptions, function (err) {
        assert.isNull(err);
        assert(receiver.setOptions.calledWith(testOptions.http.receivePolicy));
        done();
      });
    });
    
    it('instanciate the receiver if necessary', function() {
      var transport = new Http({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
      assert.doesNotThrow(function() {
        transport.setOptions(testOptions, function(err){
          assert.isNull(err);
        });
      });
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_16_005: [If `done` has been specified the `setOptions` method shall call the `done` callback with no arguments when successful.]*/
    it('calls the done callback with no arguments if successful', function(done) {
      var transport = new Http({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
      transport.setOptions(testOptions, done);
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_16_009: [If `done` has been specified the `setOptions` method shall call the `done` callback with a standard javascript `Error` object when unsuccessful.]*/
    it('calls the done callback with an error object if the method is unsuccessful', function(done) {
      var transport = new Http({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
      transport.getReceiver = function(callback) {
        callback(new Error('fake error'));
      };

      transport.setOptions(testOptions, function(err) {
        assert.instanceOf(err, Error);
        done();
      });
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_16_010: [`setOptions` should not throw if `done` has not been specified.]*/
    it('does not throw if `done` is not specified', function() {
      var transport = new Http({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
      assert.doesNotThrow(function() {
        transport.setOptions({});
      });
    });
  });

  describe('#complete', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_16_002: [The ‘complete’ method shall call the ‘complete’ method of the receiver object and pass it the message and the callback given as parameters.] */
    it('calls the receiver `complete` method', function () {
      transport.complete(testMessage, testCallback);
      assert(receiver.complete.calledWith(testMessage, testCallback));
    });
  });

  describe('#reject', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_16_003: [The ‘reject’ method shall call the ‘reject’ method of the receiver object and pass it the message and the callback given as parameters.] */
    it('calls the receiver `reject` method', function () {
      transport.reject(testMessage, testCallback);
      assert(receiver.reject.calledWith(testMessage, testCallback));
    });
  });

  describe('#abandon', function () {
    /*Tests_SRS_NODE_DEVICE_HTTP_16_001: [The ‘abandon’ method shall call the ‘abandon’ method of the receiver object and pass it the message and the callback given as parameters.] */
    it('calls the receiver `abandon` method', function () {
      transport.abandon(testMessage, testCallback);
      assert(receiver.abandon.calledWith(testMessage, testCallback));
    });
  });

  describe('#updateSharedAccessSignature', function() {
    /*Codes_SRS_NODE_DEVICE_HTTP_16_006: [The updateSharedAccessSignature method shall save the new shared access signature given as a parameter to its configuration.] */
    /*Codes_SRS_NODE_DEVICE_HTTP_16_007: [The updateSharedAccessSignature method shall call the `done` callback with a null error object and a SharedAccessSignatureUpdated object as a result, indicating that the client does not need to reestablish the transport connection.] */
    it('updates its configuration object with the new shared access signature', function(done) {
      var transportWithoutReceiver = new Http({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
      var newSas = 'newsas';
      transportWithoutReceiver.updateSharedAccessSignature(newSas, function(err, result) {
        if(err) {
          done(err);
        } else {
          assert.equal(result.constructor.name, 'SharedAccessSignatureUpdated');
          assert.equal(transportWithoutReceiver._config.sharedAccessSignature,newSas);
          done();
        }
      });
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_16_008: [The updateSharedAccessSignature method shall call the `updateSharedAccessSignature` method of the current receiver object if it exists.] */
    it('updates the receiver configuration with the new shared access signature', function(done) {
      var newSas = 'newsas';
      transport.updateSharedAccessSignature(newSas, function(err, result) {
        if (err) {
          done (err);
        } else {
          assert.equal(result.constructor.name, 'SharedAccessSignatureUpdated');
          assert(receiver.updateSharedAccessSignature.calledWith(newSas));
          done();
        }
      });
    });
  });
});