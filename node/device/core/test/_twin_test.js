// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Twin = require('../lib/twin.js');

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var errors = require('azure-iot-common').errors;

var sinon = require('sinon');


var FakeReceiver = function() {
  
  EventEmitter.call(this);

  var self = this;
  this.forceError = false;
  this.successfulSubscription = true;
  
   this._handleNewListener = function(eventname) {
    if (EventEmitter.listenerCount(self, eventname) === 0)
    {
      process.nextTick(function() {
        if (eventname === Twin.responseEvent) {
          if (self.forceError) {
            self.emit('error', new Error('failed to subscribe to ' + eventname))                               ;
          } else if (self.successfulSubscription) {
            self.emit('subscribed', eventname);
          }
          // else time out
        }
      });
    }
  };
  this.on("newListener", this._handleNewListener);
};
util.inherits(FakeReceiver, EventEmitter);



var FakeTransport = function(receiver) {
  var self = this;
  this.status=200;

  if (receiver) {  
    this._receiver = receiver;
  } else {
    this._receiver = new FakeReceiver();
  }
  this.getTwinReceiver = function(done) {
    done(null, this._receiver);
  };

  this.sendTwinRequest = function(method, resource, properties, body, done)  {
    void(method, resource, properties, body);
      
    process.nextTick(function() {
      var response = {
        'status' : self.status,
        '$rid' : properties.$rid,
        'body' : ' '
      };
      self._receiver.emit('response', response);
    });
    if (done) {
      done();
    }
  };
};



var FakeClient = function(transport) {
  if (transport) {
    this._transport = transport;  
  } else {
    this._transport = new FakeTransport();
  }
};

Twin.timeout = 10;

describe('Twin', function () {
  describe('fromDeviceClient', function () {
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_001: [** `fromDeviceclient` shall accept 2 parameters: an azure-iot-device `client` and a `done` method. **]** */
    it('accepts 2 parameters', function (done) {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err) {
        if (err) throw err;
        done();
      });
    });
    

    /* Tests_SRS_NODE_DEVICE_TWIN_18_002: [** `fromDeviceclient` shall throw `ReferenceError` if the client object is falsy **]** */
    it('throws if client is falsy', function () {
      assert.throws(function() { Twin.fromDeviceClient(null, function() {}); }, ReferenceError);
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_030: [** `fromDeviceclient` shall throw `ReferenceError` if the `done` argument is falsy **]** */
    it('throws if done is falsy', function () {
      assert.throws(function() { Twin.fromDeviceClient({}); }, ReferenceError);
    });
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_003: [** `fromDeviceClient` shall allocate a new `Twin` object **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_011: [** `fromDeviceClient` shall call `done` with `err`=`null` if it receives a `subscribed` events for the `response` topic. **]**  */
    it('Allocates a new twin object', function () {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        assert.instanceof(obj, Twin);
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_028: [** if `fromDeviceClient` has previously been called for this client, it shall return the same object **]** */
    it('returns the same object if called twice', function (done) {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err, obj1) {
        if (err) throw err;
        Twin.fromDeviceClient(client, function(err, obj2) {
          assert.equal(obj1, obj2);
          done();
        });
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_029: [** if `fromDeviceClient` is called with 2 different `client`s, it shall return 2 unique `Twin` objects **]** */
    it('returns a unique twin per client', function (done) {
      var client1 = new FakeClient();
      var client2 = new FakeClient();
      Twin.fromDeviceClient(client1, function(err, obj1) {
        if (err) throw err;
        Twin.fromDeviceClient(client2, function(err, obj2) {
          assert.notEqual(obj1, obj2);
          done();
        });
      });
    });


    /* Tests_SRS_NODE_DEVICE_TWIN_18_004: [** `fromDeviceClient` shall call `getTwinReceiver` on the protocol object to get a twin receiver. **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_006: [** `fromDeviceClient` shall store the twinReceiver obejct as a property **]**  */
    it('calls getTwinReceiver', function (done) {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        assert.instanceOf(obj._receiver, FakeReceiver);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_005: [** If the protocol does not contain a `getTwinReceiver` method, `fromDeviceClient` shall return a `NotImplementedError` in the `done` callback. **]**  */
    it('returns failure if the protocol doesnt support twin', function (done) {
      var client = new FakeClient({});
      Twin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err,errors.NotImplementedError);
        done();
      });
    });
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_008: [** `fromDeviceClient` shall add `_onServiceResponse` as a handler for the `response` event on the twinReceiver **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_007: [** `fromDeviceClient` shall add handlers for the both the `subscribed` and `error` events on the twinReceiver **]**  */
    it('adds correct handlers', function (done) {
      var receiver = new FakeReceiver();
      receiver.removeListener("newListener", receiver._handleNewListener);
      var handleNewListener = sinon.spy(receiver,"_handleNewListener");
      receiver.on("newListener", receiver._handleNewListener);
      var transport = new FakeTransport(receiver);
      var client = new FakeClient(transport);
      Twin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        assert(handleNewListener.withArgs('subscribed').calledOnce);
        assert(handleNewListener.withArgs('error').calledOnce);
        assert(handleNewListener.withArgs('response', obj._onServiceResponse).calledOnce);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_009: [** `fromDeviceClient` shall return a `ServiceUnavailableError` it has not received a `subscribed` event within `Twin.timeout` milliseconds. **]** */
    it('returns timeout correctly', function(done) {
      var receiver = new FakeReceiver();
      receiver.successfulSubscription = false;
      var transport = new FakeTransport(receiver);
      var client = new FakeClient(transport);
      Twin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err, errors.ServiceUnavailableError);
        done();
      });
    });


    /* Tests_SRS_NODE_DEVICE_TWIN_18_010: [** `fromDeviceClient` shall return the first error that is returned from `error` event on the twinReceiver. **]**  */
    it('returns error on subscription failure', function (done) {
      var receiver = new FakeReceiver();
      receiver.forceError = true;
      var transport = new FakeTransport(receiver);
      var client = new FakeClient(transport);
      Twin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err,Error);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_012: [** `fromDeviceClient` shall remove the handlers for both the `subscribed` and `error` events before calling the `done` callback. **]**   */
    it('cleans up handlers before calling done', function (done) {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        assert.equal(EventEmitter.listenerCount(obj,'subscribed'),0);
        assert.equal(EventEmitter.listenerCount(obj,'error'),0);
        done();
      });
    });
  });
  describe('_sendTwinRequest', function () {
    var receiver;
    var transport;
    var client;
    var twin;
    var sendTwinRequest;
    
    beforeEach(function(done) {
      receiver = new FakeReceiver();
      sinon.spy(receiver, 'on');
      sinon.spy(receiver, 'removeListener');
      transport = new FakeTransport(receiver);
      client = new FakeClient(transport);
      sendTwinRequest = sinon.spy(client._transport, 'sendTwinRequest');
      Twin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        twin = obj;
        done();
      });
    });

    /* Test_SRS_NODE_DEVICE_TWIN_18_013: [** `_sendTwinRequest` shall accept a `method`, `resource`, `properties`, `body`, and `done` callback **]**  */
    /* Test_SRS_NODE_DEVICE_TWIN_18_016: [** `_sendTwinRequest` shall use the `sendTwinReqest` method on the transport to send the request **]**  */
    /* Test_SRS_NODE_DEVICE_TWIN_18_018: [** The response handler shall ignore any messages that don't have an `rid` that mattches the `rid` of the request **]**   */
    /* Test_SRS_NODE_DEVICE_TWIN_18_019: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 **]**  */
    it ('uses the right parameters', function(done) {
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        if (err) throw err;
        assert(sendTwinRequest.calledOnce);
        assert.equal(sendTwinRequest.firstCall.args[0], 'fake_method');
        assert.equal(sendTwinRequest.firstCall.args[1], 'fake_resource');
        assert.equal(sendTwinRequest.firstCall.args[3], 'fake_body');
        done();
      });
    });
      
    /* Test_SRS_NODE_DEVICE_TWIN_18_017: [** `_sendTwinRequest` shall put the `rid` value into the `properties` object that gets passed to `sendTwinRequest` on the transport **]**  */
    /* Test_SRS_NODE_DEVICE_TWIN_18_014: [** `_sendTwinRequest` shall use an arbitrary starting `rid` and incriment by it by one for every call to `_sendTwinRequest` **]**  */
    it ('uses and incriments request ID', function(done) {
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        if (err) throw err;
        twin._sendTwinRequest('fake_method2', 'fake_resource2', {}, 'fake_body2', function(err) {
          if (err) throw err;
          assert(sendTwinRequest.calledTwice);
          assert.isDefined(sendTwinRequest.firstCall.args[2].$rid);
          assert.equal(sendTwinRequest.firstCall.args[2].$rid + 1, sendTwinRequest.secondCall.args[2].$rid);
          done();
        });
      });
    });

    /* Test_SRS_NODE_DEVICE_TWIN_18_015: [** `_sendTwinRequest` shall add a handler for the `response` event on the twinReceiver object.  **]** */
    it ('adds a response handler', function(done) {
        twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
          if (err) throw err;
          assert(receiver.on.withArgs('response').calledTwice);   // once for the empty implementation and once for our handler
          done();
        });
    });

    /* Test_SRS_NODE_DEVICE_TWIN_18_021: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]**  */
    it ('removes the response handler', function(done) {
        twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
          if (err) throw err;
          assert(receiver.removeListener.withArgs('response').calledOnce);
          done();
        });
    });
          
    /* Test_SRS_NODE_DEVICE_TWIN_18_022: [** If the response doesn't come within `Twin.timeout` milliseconds, `_sendTwinRequest` shall call `done` with `err`=`ServiceUnavailableError` **]**   */
    it ('returns timeout correctly', function(done) {
      transport.sendTwinRequest = function () {};
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        assert.instanceOf(err, errors.ServiceUnavailableError);
        done();
      });
    });
      
    /* Test_SRS_NODE_DEVICE_TWIN_18_020: [** `_sendTwinRequest` shall call `done` with an `err` value generated using http_errors.js **]**  */
    it('returns error using http_errors.js', function(done) {
      transport.status = 404;
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        assert.instanceOf(err, errors.DeviceNotFoundError);
        done();
      });
    
    
    });
  });

  describe('reportTwinState', function () {
    
    /* Test_SRS_NODE_DEVICE_TWIN_18_024: [** `reportTwinState` shall accept an object which represents the `reported` state and a `done` callback **]**  */
    /* Test_SRS_NODE_DEVICE_TWIN_18_025: [** `reportTwinState` shall use _sendTwinRequest to send the patch object to the service. **]**  */
    /* Test_SRS_NODE_DEVICE_TWIN_18_026: [** When calling `_sendTwinRequest`, `reportTwinState` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the result of `_createPatchObject` as a `String`. **]**    */
    /* Test_SRS_NODE_DEVICE_TWIN_18_027: [** `reportTwinState` shall call `done` with the results from `_sendTwinRequest` **]**  */
    it('calls _sendTwinRequest correctly', function(done)  {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err, twin) {
        if (err) throw err;
        twin.reportTwinState( { 'phone_number' : '867-5309' }, done);
      });
    });
  });


});

