// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Twin = require('../lib/twin.js');
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var errors = require('azure-iot-common').errors;
var sinon = require('sinon');
var _ = require('lodash');


var FakeReceiver = function() {
  
  EventEmitter.call(this);

  var self = this;
  this.forceError = false;
  this.successfulSubscription = true;
  
   this._handleNewListener = function(eventname) {
    if (EventEmitter.listenerCount(self, eventname) === 0)
    {
      process.nextTick(function() {
        if (eventname === Twin.responseEvent || eventname === Twin.postEvent) {
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
  this.getResponse = '{}';

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
        'body' : method === 'GET' ? self.getResponse : '{}'
      };
      self._receiver.emit('response', response);
    });
    if (done) {
      done();
    }
  };
};



var FakeClient = function(transport) {
  EventEmitter.call(this);
  if (transport) {
    this._transport = transport;  
  } else {
    this._transport = new FakeTransport();
  }
};
util.inherits(FakeClient, EventEmitter);

Twin.timeout = 10;

describe('Twin', function () {
  describe('fromDeviceClient', function () {
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_002: [** `fromDeviceclient` shall throw `ReferenceError` if the `client` object is falsy **]** */
    it('throws if client is falsy', function () {
      assert.throws(function() { Twin.fromDeviceClient(null, function() {}); }, ReferenceError);
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_030: [** `fromDeviceclient` shall throw `ReferenceError` if the `done` argument is falsy **]** */
    it('throws if done is falsy', function () {
      assert.throws(function() { Twin.fromDeviceClient({}); }, ReferenceError);
    });
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_003: [** `fromDeviceClient` shall allocate a new `Twin` object **]**  */
    it('Allocates a new twin object', function (done) {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err, obj) {
        if (err) return done(err);
        assert.instanceOf(obj, Twin);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_044: [** `fromDeviceClient` shall do a GET call to retrieve desired properties from the service. **]** */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_028: [** if `fromDeviceClient` has previously been called for this client, it shall perform a GET operation and return the same object **]** */
    it('returns the same object if called twice', function (done) {
      var client = new FakeClient();
      sinon.spy(client._transport, "sendTwinRequest");
      Twin.fromDeviceClient(client, function(err, obj1) {
        assert(client._transport.sendTwinRequest.withArgs('GET').calledOnce);
        if (err) return done(err);
        Twin.fromDeviceClient(client, function(err, obj2) {
          assert(client._transport.sendTwinRequest.withArgs('GET').calledTwice);
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
        if (err) return done(err);
        Twin.fromDeviceClient(client2, function(err, obj2) {
          assert.notEqual(obj1, obj2);
          done();
        });
      });
    });


    /* Tests_SRS_NODE_DEVICE_TWIN_18_004: [** `fromDeviceClient` shall call `getTwinReceiver` on the protocol object to get a twin receiver. **]**  */
    it('calls getTwinReceiver', function (done) {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err, obj) {
        if (err) return done(err);
        assert.instanceOf(obj._receiver, FakeReceiver);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_005: [** If the protocol does not contain a `getTwinReceiver` method, `fromDeviceClient` shall call the `done` callback with a `NotImplementedError` object **]**  */
    it('returns failure if the protocol doesnt support twin', function (done) {
      var client = new FakeClient({});
      Twin.fromDeviceClient(client, function(err) {
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
      Twin.fromDeviceClient(client, function(err) {
        if (err) return done(err);
        assert(handleNewListener.withArgs('subscribed').calledOnce);
        assert(handleNewListener.withArgs('error').calledOnce);
        /* Tests_SRS_NODE_DEVICE_TWIN_18_031: [** `fromDeviceClient` shall subscribe to the response topic **]** */
        assert(handleNewListener.withArgs('response').called);
        /* Tests_SRS_NODE_DEVICE_TWIN_18_032: [** `fromDeviceClient` shall subscribe to the desired property patch topic **]** */
        assert(handleNewListener.withArgs('post').called);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_009: [** `fromDeviceClient` shall call the `done` callback passing a `TimeoutError` if it has not received all necessary `subscribed` events within `Twin.timeout` milliseconds. **]** */
    it('returns timeout correctly', function(done) {
      var receiver = new FakeReceiver();
      receiver.successfulSubscription = false;
      var transport = new FakeTransport(receiver);
      var client = new FakeClient(transport);
      Twin.fromDeviceClient(client, function(err) {
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
      Twin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err,Error);
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_012: [** `fromDeviceClient` shall remove the handlers for both the `subscribed` and `error` events before calling the `done` callback. **]**   */
    it('cleans up handlers before calling done', function (done) {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err, obj) {
        if (err) return done(err);
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
      Twin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err, errors.TimeoutError);
        done();
      });
    });

    it ('ignores unkonwn subscription events', function(done) {
      var client = new FakeClient();
      client._transport._receiver.fakeSubscriptionName = 'bogus';
      Twin.fromDeviceClient(client, function(err) {
        assert.instanceOf(err, errors.TimeoutError);
        done();
      });
    });

    it ('relies on clearTimeout(null) not excepting or crashing', function(done) {
      clearTimeout(null);
      done();
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_043: [** If the GET operation fails, `fromDeviceClient` shall call the done method with the error object in the first parameter **]** */
    it('fails when the GET fails', function(done) {
      var errorToReturn = new Error();
      var client = new FakeClient();
      var oldFunction = Twin.prototype._sendTwinRequest; 
      Twin.prototype._sendTwinRequest = function(method, resource, properties, body, done) {
        if (method === "GET") {
          done(errorToReturn);
        } else {
          oldFunction(method, resource, properties, body, done);
        }
      };
      Twin.fromDeviceClient(client, function(err) {
        Twin.prototype._sendTwinRequest = oldFunction;
        assert.equal(err, errorToReturn);
        done();
      });
    });
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_035: [** When a the results of a GET operation is received, the `Twin` object shall merge the properties into `this.properties.desired`. **]** */
    it('fires property changed events recursively', function(done) {
      var client = new FakeClient();
      
      var patch = {
        desired : {
          foo : {
            bar : 1
          },
          baz : 1
        }
      };

      client._transport.sendTwinRequest = function(method, resource, properties, body, done)  {
        void(method, resource, properties, body);

        process.nextTick(function() {
          var response = {
            'status' : 200,
            '$rid' : properties.$rid,
            'body' : JSON.stringify(patch)
          };
          client._transport._receiver.emit('response', response);
        });

        if (done) {
          done();
        }
      };

      Twin.fromDeviceClient(client, function(err, twin) {
        if (err) return done(err);

        var propArray = [
          'desired',
          'desired.foo',
          'desired.foo.bar',
          'desired.baz' 
        ];
        
        propArray.forEach(function(prop) {
          assert.deepEqual(_.at(twin.properties, prop), _.at(patch, prop));
        });

        done();


      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_045: [** If a property is already set when a handler is added for that property, the `Twin` object shall fire a property changed event for the property. **]*  */
    it ('fires events if the property is already set when the handler is added', function(done) {
      var client = new FakeClient();
      client._transport.getResponse = JSON.stringify({desired : { foo : 42 }});
      Twin.fromDeviceClient(client, function(err, twin) {
        if (err) return done(err);
        twin.on('properties.desired.foo', function(delta) {
          if (delta === 42) {
            done();
          }
        });
      });
    });


  });
    
  describe('_sendTwinRequest', function () {
    var receiver;
    var transport;
    var client;
    var twin;
    
    beforeEach(function(done) {
      receiver = new FakeReceiver();
      sinon.spy(receiver, 'on');
      sinon.spy(receiver, 'removeListener');
      transport = new FakeTransport(receiver);
      client = new FakeClient(transport);
      Twin.fromDeviceClient(client, function(err, obj) {
        if (err) return done(err);
        twin = obj;
        done();
      });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_016: [** `_sendTwinRequest` shall use the `sendTwinRequest` method on the transport to send the request **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_019: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 or 204 **]**   */
    it ('uses the right parameters', function(done) {
      var sendTwinRequest = sinon.spy(client._transport, 'sendTwinRequest');
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        if (err) return done(err);
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
        if (err) return done(err);
        done();
      });
    });
      
    /* Tests_SRS_NODE_DEVICE_TWIN_18_014: [** `_sendTwinRequest` shall use an arbitrary starting `rid` and increment it by one for every call to `_sendTwinRequest` **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_017: [** `_sendTwinRequest` shall put the `rid` value into the `properties` object that gets passed to `sendTwinRequest` on the transport **]**  */
    it ('uses and incriments request ID', function(done) {
      var sendTwinRequest = sinon.spy(client._transport, 'sendTwinRequest');
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        if (err) return done(err);
        twin._sendTwinRequest('fake_method2', 'fake_resource2', {}, 'fake_body2', function(err) {
          if (err) return done(err);
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
          if (err) return done(err);
          assert(receiver.on.withArgs('response').calledThrice);   // once for the empty implementation, once for the GET handler, and once for our handler
          done();
        });
    });

    /* Tests_SRS_NODE_DEVICE_TWIN_18_021: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]**  */
    it ('removes the response handler', function(done) {
        twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
          if (err) return done(err);
          assert(receiver.removeListener.withArgs('response').calledTwice); // Once for the GET handler and once for our handler
          done();
        });
    });
          
    /* Tests_SRS_NODE_DEVICE_TWIN_18_022: [** If the response doesnt come within `Twin.timeout` milliseconds, `_sendTwinRequest` shall call `done` with `err`=`TimeoutError` **]**   */
    it ('returns timeout correctly', function(done) {
      transport.sendTwinRequest = function () {};
      twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
        assert.instanceOf(err, errors.TimeoutError);
        done();
      });
    });
      
    /*Tests_SRS_NODE_DEVICE_TWIN_18_020: [** `_sendTwinRequest` shall call `done` with an `err` value translated using `translateError` **]**  */
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_003: [`translateError` shall return an `ArgumentError` if the response status code is `400`.]*/
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_004: [`translateError` shall return an `UnauthorizedError` if the response status code is `401`.]*/
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_005: [`translateError` shall return an `IotHubQuotaExceededError` if the response status code is `403`.]*/
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_006: [`translateError` shall return an `DeviceNotFoundError` if the response status code is `404`.]*/
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_007: [`translateError` shall return an `MessageTooLargeError` if the response status code is `413`.]*/
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_008: [`translateError` shall return an `InternalServerError` if the response status code is `500`.]*/
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_009: [`translateError` shall return an `ServiceUnavailableError` if the response status code is `503`.]*/
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_011: [`translateError` shall return an `ServiceUnavailableError` if the response status code is `504`.]*/
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_012: [`translateError` shall return an `ThrottlingError` if the response status code is `429`.] */
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_013: [`translateError` shall return an `InvalidEtagError` if the response status code is `412`.] */
    /*Tests_SRS_NODE_DEVICE_TWIN_ERRORS_18_002: [If the error code is unknown, `translateError` should return a generic Javascript `Error` object.]*/
    [
      {status: 400, errortype: errors.ArgumentError},
      {status: 401, errortype: errors.UnauthorizedError},
      {status: 403, errortype: errors.IotHubQuotaExceededError},
      {status: 404, errortype: errors.DeviceNotFoundError},
      {status: 413, errortype: errors.MessageTooLargeError},
      {status: 500, errortype: errors.InternalServerError},
      {status: 503, errortype: errors.ServiceUnavailableError},
      {status: 504, errortype: errors.ServiceUnavailableError},
      {status: 429, errortype: errors.ThrottlingError},
      {status: 412, errortype: errors.InvalidEtagError},
      {status: 999, errortype: Error},
    ].forEach(function(errorMap) {
      it('returns '+errorMap.errortype.prototype.name+ ' with error ' + errorMap.status, function(done) {
        transport.status = errorMap.status;
        twin._sendTwinRequest('fake_method', 'fake_resource', {}, 'fake_body', function(err) {
          assert.instanceOf(err, errorMap.errortype);
          done();
        });
      });
    });
  });

  describe('properties.reported.update', function () {
    
    /* Tests_SRS_NODE_DEVICE_TWIN_18_025: [** `properties.reported.update` shall use _sendTwinRequest to send the patch object to the service. **]**  */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_026: [** When calling `_sendTwinRequest`, `properties.reported.update` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the `body` parameter passed in to `reportState` as a string. **]**    */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_027: [** `properties.reported.update` shall call `done` with the results from `_sendTwinRequest` **]**  */
    it('calls _sendTwinRequest correctly', function(done)  {
      var client = new FakeClient();
      Twin.fromDeviceClient(client, function(err, twin) {
        var sendTwinRequest = sinon.spy(twin,'_sendTwinRequest');
        if (err) return done(err);
        twin.properties.reported.update( { 'phone_number' : '867-5309' }, function(err) {
          if (err) return done(err);
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
      Twin.fromDeviceClient(client, function(err, twin) {
        twin.properties.reported.update( { 'phone_number' : '867-5309', 'name' : 'Jennifer' }, function(err) {
          if (err) return done(err);
          twin.properties.reported.update( { 'name' : 'Jenny', 'age' : 42  }, function(err) {
            if (err) return done(err);
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
      
      Twin.fromDeviceClient(client, function(err, twin) {
        twin.properties.reported.update( patchAddStuff, function(err) {
          if (err) return done(err);
          twin.properties.reported.update( patchRemoveE, function(err) {
            if (err) return done(err);
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
      Twin.fromDeviceClient(client, function(err, twin) {
        client._transport.sendTwinRequest = sinon.stub(); // will cause timeout
        twin.properties.reported.update( { 'phone_number' : '867-5309', 'name' : 'Jennifer' }, function(err) {
          assert.instanceOf(err,errors.TimeoutError);
          assert.isUndefined(twin.properties.reported.name);
          done();
        });
      });
    });


  });

  describe('PATCH operations', function() {
    /* Tests_SRS_NODE_DEVICE_TWIN_18_040: [** After merging a PATCH result, the `Twin` object shall recursively fire property changed events for every changed property. **]** */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_036: [** When a PATCH operation is received, the `Twin` object shall merge the properties into `this.properties.desired`. **]** */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_041: [** When firing a property changed event, the `Twin` object shall name the event from the property using dot notation starting with 'properties.desired.' **]** */
    /* Tests_SRS_NODE_DEVICE_TWIN_18_042: [** When firing a property changed event, the `Twin` object shall pass the changed value of the property as the event parameter **]** */
    it('fires and merges', function(done) {
      var client = new FakeClient();
      
      var patch = {
        desired : {
          foo : {
            bar : 1
          },
          baz : 1
        }
      };

      Twin.fromDeviceClient(client, function(err, twin) {
        if (err) return done(err);
        var eventCount = 0;

        var propArray = [
          'desired',
          'desired.foo',
          'desired.foo.bar',
          'desired.baz' 
        ];

        propArray.forEach(function(prop) {
          twin.on('properties.'+prop, function(delta) {
            
            assert.deepEqual(_.at(patch,prop)[0],delta);
            eventCount++;

            if (eventCount === propArray.length) {
              propArray.forEach(function(prop) {
                assert.deepEqual(_.at(twin.properties, prop), _.at(patch, prop));
              });
              done();
            }

          });
        });

        client._transport._receiver.emit('post', JSON.stringify(patch.desired));

      });
    });
    
  });

  it('updateSharedAccessSignature removes all listeners for the response and post events', function(done) {
    var client = new FakeClient();
    var removeAllListeners = sinon.spy(client._transport._receiver, "removeAllListeners");
    Twin.fromDeviceClient(client, function(err, twin) {
      if (err) return done(err);
      twin.updateSharedAccessSignature();
      assert(removeAllListeners.calledWith('response'));
      assert(removeAllListeners.calledWith('post'));
      done();
    });
    
  });

  it('when _sharedAccessSignatureUpdated is fired, the twin object shall re-get and re-subscribe', function(done) {
    var client = new FakeClient();
    Twin.fromDeviceClient(client, function(err, twin) {
      if (err) return done(err);
      var on = sinon.spy(client._transport._receiver, "on");
      twin.on('properties.desired', function() {
        if (on.calledWith('response') && on.calledWith('post')) {
          done();
        }
      });
      process.nextTick(function() {
        twin.updateSharedAccessSignature();
        client.emit('_sharedAccessSignatureUpdated');
      });
    });
  });

});



 
