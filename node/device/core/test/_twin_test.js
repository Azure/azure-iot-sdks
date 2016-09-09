// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var DeviceTwin = require('../lib/twin.js');

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
        if (eventname === DeviceTwin.responseEvent) {
          if (self.forceError) {
            self.emit('error', new Error('failed to subscribe to ' + eventname))                               ;
          } else if (self.successfulSubscription) {
            self.emit('subscribed', {eventName: self.fakeSubscriptionName || eventname});
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
        '$rid' : self.responseId || properties.$rid,
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

DeviceTwin.timeout = 10;

describe('DeviceTwin', function () {
  describe('fromDeviceClient', function () {
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_002: [** `fromDeviceclient` shall throw `ReferenceError` if the `client` object is falsy **]** */
    it('throws if client is falsy', function () {
      assert.throws(function() { DeviceTwin.fromDeviceClient(null, function() {}); }, ReferenceError);
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_030: [** `fromDeviceclient` shall throw `ReferenceError` if the `done` argument is falsy **]** */
    it('throws if done is falsy', function () {
      assert.throws(function() { DeviceTwin.fromDeviceClient({}); }, ReferenceError);
    });
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_003: [** `fromDeviceClient` shall allocate a new `DeviceTwin` object **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_011: [** `fromDeviceClient` shall call the `done` callback with `err`=`null` if it receives a `subscribed` event for the `response` topic. **]**  */
    it('Allocates a new twin object', function () {
      var client = new FakeClient();
      DeviceTwin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        assert.instanceof(obj, DeviceTwin);
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_028: [** if `fromDeviceClient` has previously been called for this client, it shall return the same object **]** */
    it('returns the same object if called twice', function (done) {
      var client = new FakeClient();
      DeviceTwin.fromDeviceClient(client, function(err, obj1) {
        if (err) throw err;
        DeviceTwin.fromDeviceClient(client, function(err, obj2) {
          assert.equal(obj1, obj2);
          done();
        });
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_029: [** if `fromDeviceClient` is called with 2 different `client`s, it shall return 2 unique `DeviceTwin` objects **]** */
    it('returns a unique twin per client', function (done) {
      var client1 = new FakeClient();
      var client2 = new FakeClient();
      DeviceTwin.fromDeviceClient(client1, function(err, obj1) {
        if (err) throw err;
        DeviceTwin.fromDeviceClient(client2, function(err, obj2) {
          assert.notEqual(obj1, obj2);
          done();
        });
      });
    });


    /* Tests_SRS_NODE_DEVICE_TWIN_18_004: [** `fromDeviceClient` shall call `getTwinReceiver` on the protocol object to get a twin receiver. **]**  */
    it('calls getTwinReceiver', function (done) {
      var client = new FakeClient();
      DeviceTwin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        assert.instanceOf(obj._receiver, FakeReceiver);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_005: [** If the protocol does not contain a `getTwinReceiver` method, `fromDeviceClient` shall call the `done` callback with a `NotImplementedError` object **]**  */
    it('returns failure if the protocol doesnt support twin', function (done) {
      var client = new FakeClient({});
      DeviceTwin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err,errors.NotImplementedError);
        done();
      });
    });
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_007: [** `fromDeviceClient` shall add handlers for the both the `subscribed` and `error` events on the twinReceiver **]**  */
    it('adds correct handlers', function (done) {
      var receiver = new FakeReceiver();
      receiver.removeListener("newListener", receiver._handleNewListener);
      var handleNewListener = sinon.spy(receiver,"_handleNewListener");
      receiver.on("newListener", receiver._handleNewListener);
      var transport = new FakeTransport(receiver);
      var client = new FakeClient(transport);
      DeviceTwin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        assert(handleNewListener.withArgs('subscribed').calledOnce);
        assert(handleNewListener.withArgs('error').calledOnce);
        assert(handleNewListener.withArgs('response', obj._onServiceResponse).calledOnce);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_009: [** `fromDeviceClient` shall call the `done` callback passing a `TimeoutError` if it has not received a `subscribed` event within `DeviceTwin.timeout` milliseconds. **]**  */
    it('returns timeout correctly', function(done) {
      var receiver = new FakeReceiver();
      receiver.successfulSubscription = false;
      var transport = new FakeTransport(receiver);
      var client = new FakeClient(transport);
      DeviceTwin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err, errors.TimeoutError);
        done();
      });
    });


    /* Tests_SRS_NODE_DEVICE_TWIN_18_010: [** `fromDeviceClient` shall call the `done` callback passing  the first error that is returned from `error` event on the twinReceiver. **]**  */
    it('returns error on subscription failure', function (done) {
      var receiver = new FakeReceiver();
      receiver.forceError = true;
      var transport = new FakeTransport(receiver);
      var client = new FakeClient(transport);
      DeviceTwin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err,Error);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_012: [** `fromDeviceClient` shall remove the handlers for both the `subscribed` and `error` events before calling the `done` callback. **]**   */
    it('cleans up handlers before calling done', function (done) {
      var client = new FakeClient();
      DeviceTwin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        assert.equal(EventEmitter.listenerCount(obj,'subscribed'),0);
        assert.equal(EventEmitter.listenerCount(obj,'error'),0);
        done();
      });
    });

    it('gracefully handles getTwinReceiver failures', function(done) {
      var client = new FakeClient();
      client._transport.getTwinReceiver = function(done) {
        done(new errors.TimeoutError());
      };
      DeviceTwin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err, errors.TimeoutError);
        done();
      });
    });

    it ('ignores unkonwn subscription events', function(done) {
      var client = new FakeClient();
      client._transport._receiver.fakeSubscriptionName = 'bogus';
      DeviceTwin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err, errors.TimeoutError);
        done();
      });
    });

    it ('relies on clearTimeout(null) not excepting or crashing', function(done) {
      clearTimeout(null);
      done();
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
      DeviceTwin.fromDeviceClient(client, function(err, obj) {
        if (err) throw err;
        twin = obj;
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_016: [** `_sendTwinRequest` shall use the `sendTwinRequest` method on the transport to send the request **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_019: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 or 204 **]**   */
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

    /* Tests_SRS_NODE_DEVICE_TWIN_18_018: [** The response handler shall ignore any messages that dont have an `rid` that matches the `rid` of the request **]**   */
    it ('ignores non-matching responses', function(done) {
      transport.responseId = 10;
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        assert.instanceOf(err, errors.TimeoutError);
        done();
      });
    });
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_019: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 or 204 **]**   */
    it ('returns success on 204', function(done) {
      transport.status = 204;
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        if (err) throw err;
        done();
      });
    });
      
    /* Tests_SRS_NODE_DEVICE_TWIN_18_014: [** `_sendTwinRequest` shall use an arbitrary starting `rid` and increment it by one for every call to `_sendTwinRequest` **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_017: [** `_sendTwinRequest` shall put the `rid` value into the `properties` object that gets passed to `sendTwinRequest` on the transport **]**  */
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

    /* Tests_SRS_NODE_DEVICE_TWIN_18_015: [** `_sendTwinRequest` shall add a handler for the `response` event on the twinReceiver object.  **]** */
    it ('adds a response handler', function(done) {
        twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
          if (err) throw err;
          assert(receiver.on.withArgs('response').calledTwice);   // once for the empty implementation and once for our handler
          done();
        });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_021: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]**  */
    it ('removes the response handler', function(done) {
        twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
          if (err) throw err;
          assert(receiver.removeListener.withArgs('response').calledOnce);
          done();
        });
    });
          
    /* Tests_SRS_NODE_DEVICE_TWIN_18_022: [** If the response doesnt come within `DeviceTwin.timeout` milliseconds, `_sendTwinRequest` shall call `done` with `err`=`TimeoutError` **]**   */
    it ('returns timeout correctly', function(done) {
      transport.sendTwinRequest = function () {};
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        assert.instanceOf(err, errors.TimeoutError);
        done();
      });
    });
      
    /* Tests_SRS_NODE_DEVICE_TWIN_18_020: [** `_sendTwinRequest` shall call `done` with an `err` value translated using http_errors.js **]**  */
    it('returns error using http_errors.js', function(done) {
      transport.status = 404;
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        assert.instanceOf(err, errors.DeviceNotFoundError);
        done();
      });
    
    
    });
  });

  describe('properties.reported.update', function () {
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_025: [** `properties.reported.update` shall use _sendTwinRequest to send the patch object to the service. **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_026: [** When calling `_sendTwinRequest`, `properties.reported.update` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the `body` parameter passed in to `reportState` as a string. **]**    */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_027: [** `properties.reported.update` shall call `done` with the results from `_sendTwinRequest` **]**  */
    it('calls _sendTwinRequest correctly', function(done)  {
      var client = new FakeClient();
      DeviceTwin.fromDeviceClient(client, function(err, twin) {
        var sendTwinRequest = sinon.spy(twin,'_sendTwinRequest');
        if (err) throw err;
        twin.properties.reported.update( { 'phone_number' : '867-5309' }, function(err) {
          if (err) throw err;
          assert(sendTwinRequest.calledOnce);
          assert.equal(sendTwinRequest.firstCall.args[0],'PATCH');
          assert.equal(sendTwinRequest.firstCall.args[1],'/properties/reported/');
          assert.equal(JSON.stringify(sendTwinRequest.firstCall.args[2]),'{}');
          assert.equal(sendTwinRequest.firstCall.args[3],'{"phone_number":"867-5309"}');
          done();
        });
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_031: [** `properties.reported.update` shall merge the contents of the patch object into `properties.reported` **]** */
    it ('merges the patch contents', function(done) {
      var client = new FakeClient();
      DeviceTwin.fromDeviceClient(client, function(err, twin) {
        twin.properties.reported.update( { 'phone_number' : '867-5309', 'name' : 'Jennifer' }, function(err) {
          if (err) done(err);
          twin.properties.reported.update( { 'name' : 'Jenny', 'age' : 42  }, function(err) {
            if (err) done(err);
            assert.equal(twin.properties.reported.name, 'Jenny');
            assert.equal(twin.properties.reported.phone_number, '867-5309');
            assert.equal(twin.properties.reported.age, 42);
            done();
          });
        });
      });
    });
            
    /* Tests_SRS_NODE_DEVICE_TWIN_18_032: [** When merging the patch, if any properties are set to `null`, `properties.reported.update` shall delete that property from `properties.reported`. **]** */
    it('handles null properties when merging', function(done) {
      var client = new FakeClient();
      var patchAddStuff = {
        a: {
          b: { 
            c: 'abc',
            d: {
              e: 'def'
            }
          }
        }
      };
      var patchRemoveE = {
        a: {
          b: {
            d: {
              e: null
            }
          }
        }
      };
      
      DeviceTwin.fromDeviceClient(client, function(err, twin) {
        twin.properties.reported.update( patchAddStuff, function(err) {
          if (err) done(err);
          twin.properties.reported.update( patchRemoveE, function(err) {
            if (err) done(err);
            assert.equal(twin.properties.reported.a.b.c, 'abc');
            assert.isDefined(twin.properties.reported.a.b.d);
            assert.isUndefined(twin.properties.reported.a.b.d.e);
            done();
          });
        });
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_033: [** If `_sendTwinRequest` fails, `properties.reported.update` shall not merge the contents of the patch object into `properties.reported` **]** */
    it ('does not merge patch if _sendTwinRequest fails', function(done) {
      var client = new FakeClient();
      DeviceTwin.fromDeviceClient(client, function(err, twin) {
        client._transport.sendTwinRequest = sinon.stub(); // will cause timeout
        twin.properties.reported.update( { 'phone_number' : '867-5309', 'name' : 'Jennifer' }, function(err) {
          assert.instanceOf(err,errors.TimeoutError);
          assert.isUndefined(twin.properties.reported.name);
          done();
        });
      });
    });

  });
});

