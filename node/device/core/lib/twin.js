// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var errors = require('azure-iot-common').errors;
var _ = require('lodash');
var traverse = require('traverse');
require('es5-shim');
var translateError = require('./twin_errors.js');

var Twin = function(client) {
  EventEmitter.call(this);

  this._client = client;
  this._rid = 4200;   // arbitrary starting value.  
};

util.inherits(Twin, EventEmitter);

Twin.timeout = 1000;

Twin.errorEvent = 'error';
Twin.subscribedEvent = 'subscribed';

Twin.responseEvent = 'response';
Twin.postEvent = 'post';

Twin.desiredPath = 'properties.desired';

/**
 * @method          module:azure-iot-device.Twin#fromDeviceClient
 * @description     Get a Twin object for the given client connection
 *
 * @param {Object}      client  The [client]{@link module:azure-iot-device.Client} object that this Twin object is associated with.
 *
 * @param {Function}      done  the callback to be invoked when this function completes.
 *
 * @throws {ReferenceError}   One of the required parameters is falsy
 */

Twin.fromDeviceClient = function(client, done) {
  /* Codes_SRS_NODE_DEVICE_TWIN_18_002: [** `fromDeviceclient` shall throw `ReferenceError` if the `client` object is falsy **]** */
  if (!client) {
    throw new ReferenceError('client parameter is required');
  }
  /* Codes_SRS_NODE_DEVICE_TWIN_18_030: [** `fromDeviceclient` shall throw `ReferenceError` if the `done` argument is falsy **]** */
  if (!done) {
    throw new ReferenceError('done parameter is required');
  }

  /* Codes_SRS_NODE_DEVICE_TWIN_18_028: [** if `fromDeviceClient` has previously been called for this client, it shall perform a GET operation and return the same object **]** */
  if (client._twin) {
    client._twin._getPropertiesFromService(function (err) {
        done(err,client._twin);
      });
  } else {
    /* Codes_SRS_NODE_DEVICE_TWIN_18_029: [** if `fromDeviceClient` is called with 2 different `client`s, it shall return 2 unique `Twin` objects **]** */
    /* Codes_SRS_NODE_DEVICE_TWIN_18_003: [** `fromDeviceClient` shall allocate a new `Twin` object **]**  */
    var twin = new Twin(client);

    twin.on("newListener", twin._handleNewListener.bind(twin));

    /* Codes_SRS_NODE_DEVICE_TWIN_18_005: [** If the protocol does not contain a `getTwinReceiver` method, `fromDeviceClient` shall call the `done` callback with a `NotImplementedError` object **]**  */
    if (!client._transport.getTwinReceiver) {
      done(new errors.NotImplementedError('transport does not support Twin'));
    } else {
      client._twin = twin;
      client.on('_sharedAccessSignatureUpdated', function() {
        twin._connectSubscribeAndGetProperties(function() {});
      });
      twin._connectSubscribeAndGetProperties(done);
    }
  }
};

Twin.prototype.updateSharedAccessSignature = function() {
  this._receiver.removeAllListeners(Twin.responseEvent);
  this._receiver.removeAllListeners(Twin.postEvent);
};

Twin.prototype._connectSubscribeAndGetProperties = function(done) {
  var self = this;
  /* Codes_SRS_NODE_DEVICE_TWIN_18_004: [** `fromDeviceClient` shall call `getTwinReceiver` on the protocol object to get a twin receiver. **]**  */
  this._client._transport.getTwinReceiver(function(err, receiver) {
    if (err) {
      done(err);
    } else {
      self._receiver = receiver;

       self._subscribe( function(err) {
        if (err) {
          done(err);
        } else {
          self._getPropertiesFromService(function (err) {
            /* Codes_SRS_NODE_DEVICE_TWIN_18_043: [** If the GET operation fails, `fromDeviceClient` shall call the done method with the error object in the first parameter **]** */
            done(err, self);
          });
        }
      });
    }
  });
};


/* Codes_SRS_NODE_DEVICE_TWIN_18_031: [** `fromDeviceClient` shall subscribe to the response topic **]** */
/* Codes_SRS_NODE_DEVICE_TWIN_18_032: [** `fromDeviceClient` shall subscribe to the desired property patch topic **]** */
Twin.prototype._subscribe = function(done) {

  var twin = this;
  var receiver = twin._receiver;
  var topics = [
    {
      topicName : Twin.responseEvent,
      subscribed : false,
      handler : twin._onServiceResponse.bind(this)
    },
    {
      topicName : Twin.postEvent,
      subscribed : false,
      handler : twin._onServicePost.bind(this)
    }
  ];

  var handleSubscribed = function(obj) {
    if (topics.some(function(topic) {
      if (topic.topicName === obj.eventName) {
        topic.subscribed = true;
        return true;
      }
    })) {
      if (topics.every(function(topic) {
        return topic.subscribed; 
      })) {
        cleanupAndReturn(null, twin);
      }
    }
  };
    
  
  /* Codes_SRS_NODE_DEVICE_TWIN_18_010: [** `fromDeviceClient` shall call the `done` callback passing  the first error that is returned from `error` event on the twinReceiver. **]**  */
  var errAlreadySent = false;
  var handleError = function(topic) {
    if (!errAlreadySent) {
      errAlreadySent = true;
      cleanupAndReturn(new errors.ServiceUnavailableError('failed to subscribe to '   + topic));
    }
  };
      

  var timeout = null;
  var cleanupAndReturn = function(err) {
    clearTimeout(timeout);
    timeout = null;
    /* Codes_SRS_NODE_DEVICE_TWIN_18_012: [** `fromDeviceClient` shall remove the handlers for both the `subscribed` and `error` events before calling the `done` callback. **]**   */
    receiver.removeListener(Twin.subscribedEvent, handleSubscribed);
    receiver.removeListener(Twin.errorEvent, handleError);
    done(err);
  };

  /* Codes_SRS_NODE_DEVICE_TWIN_18_007: [** `fromDeviceClient` shall add handlers for the both the `subscribed` and `error` events on the twinReceiver **]**  */
  receiver.on(Twin.subscribedEvent, handleSubscribed);
  receiver.on(Twin.errorEvent, handleError);

  /* Codes_SRS_NODE_DEVICE_TWIN_18_009: [** `fromDeviceClient` shall call the `done` callback passing a `TimeoutError` if it has not received all necessary `subscribed` events within `Twin.timeout` milliseconds. **]** */
  timeout = setTimeout(function() {
    cleanupAndReturn(new errors.TimeoutError('subscription to twin messages timed out'), null);
  }, Twin.timeout);

  topics.forEach(function(topic) {
    receiver.on(topic.topicName, topic.handler);
  });
  
};

Twin.prototype._sendTwinRequest = function(method, resource, properties, body, done) {

  /* Codes_SRS_NODE_DEVICE_TWIN_18_014: [** `_sendTwinRequest` shall use an arbitrary starting `rid` and increment it by one for every call to `_sendTwinRequest` **]**  */
  /* Codes_SRS_NODE_DEVICE_TWIN_18_017: [** `_sendTwinRequest` shall put the `rid` value into the `properties` object that gets passed to `sendTwinRequest` on the transport **]**  */
  var propCopy = JSON.parse(JSON.stringify(properties));  
  propCopy .$rid = this._rid;
  this._rid++;

  var self = this;

  var handleResponse = function(resp) {
    /* Codes_SRS_NODE_DEVICE_TWIN_18_018: [** The response handler shall ignore any messages that dont have an `rid` that matches the `rid` of the request **]**   */
    if (resp.$rid.toString() === propCopy.$rid.toString())
    {
      /* Codes_SRS_NODE_DEVICE_TWIN_18_019: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 or 204 **]**   */
      if (resp.status.toString() === '200' || resp.status.toString() === '204') {
       cleanupAndReturn(null, resp.body);
      } else {
        /* Codes_SRS_NODE_DEVICE_TWIN_18_020: [** `_sendTwinRequest` shall call `done` with an `err` value translated using `translateError`**]**  */
        cleanupAndReturn(translateError(resp, resp.status));
      }
    }
  };
  
  /* Codes_SRS_NODE_DEVICE_TWIN_18_021: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]**  */
  var timeout = null;
  var cleanupAndReturn = function(err, body) {
    clearTimeout(timeout);
    timeout = null;
    self._receiver.removeListener('response', handleResponse);
    done(err, body);
  };

  /* Codes_SRS_NODE_DEVICE_TWIN_18_015: [** `_sendTwinRequest` shall add a handler for the `response` event on the twinReceiver object.  **]** */
  this._receiver.on('response', handleResponse);

  /* Codes_SRS_NODE_DEVICE_TWIN_18_022: [** If the response doesnt come within `Twin.timeout` milliseconds, `_sendTwinRequest` shall call `done` with `err`=`TimeoutError` **]**   */
  timeout = setTimeout(function() {
    cleanupAndReturn(new errors.TimeoutError('message timed out'), null);
  }, Twin.timeout);

  /* Codes_SRS_NODE_DEVICE_TWIN_18_016: [** `_sendTwinRequest` shall use the `sendTwinRequest` method on the transport to send the request **]**  */
  this._client._transport.sendTwinRequest(method, resource, propCopy, body);
};


Twin.prototype._updateReportedProperties = function (state, done) {
  /* Codes_SRS_NODE_DEVICE_TWIN_18_025: [** `properties.reported.update` shall use _sendTwinRequest to send the patch object to the service. **]**  */
  /* Codes_SRS_NODE_DEVICE_TWIN_18_026: [** When calling `_sendTwinRequest`, `properties.reported.update` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the `body` parameter passed in to `reportState` as a string. **]**    */
  /* Codes_SRS_NODE_DEVICE_TWIN_18_027: [** `properties.reported.update` shall call `done` with the results from `_sendTwinRequest` **]**  */
  var self = this;
  this._sendTwinRequest('PATCH', '/properties/reported/', {}, JSON.stringify(state), function (err) {
    /* Codes_SRS_NODE_DEVICE_TWIN_18_033: [** If `_sendTwinRequest` fails, `properties.reported.update` shall not merge the contents of the patch object into `properties.reported` **]** */
    if (err) {
      done(err);
    } else {
      self._mergePatch(self.properties.reported, state);
      done();
    }
  });
};

/* Codes_SRS_NODE_DEVICE_TWIN_18_031: [** `properties.reported.update` shall merge the contents of the patch object into `properties.reported` **]** */
Twin.prototype._mergePatch = function(dest, patch) {
  _.merge(dest, patch);

  /* Codes_SRS_NODE_DEVICE_TWIN_18_032: [** When merging the patch, if any properties are set to `null`, `properties.reported.update` shall delete that property from `properties.reported`. **]** */
  traverse(dest).forEach(function (prop) {
    if (prop === null) {
      this.remove();
    }
  });
};

Twin.prototype._onServiceResponse = function() {
};

Twin.prototype._clearCachedProperties = function () {
  var self = this;
  this.properties = { 
    reported : {
      update : function(state, done) {
        self._updateReportedProperties(state, done);
      }
    },
    desired : {
    }
  };
};

/* Codes_SRS_NODE_DEVICE_TWIN_18_044: [** `fromDeviceClient` shall do a GET call to retrieve desired properties from the service. **]** */
Twin.prototype._getPropertiesFromService = function(done) {
  var self = this;
  /* Codes_SRS_NODE_DEVICE_TWIN_18_034: [** `fromDeviceClient` shall ignore any PATCH operations that arrive before the GET returns **]** */
  this._clearCachedProperties();
  this._sendTwinRequest('GET', '/', {}, ' ', function(err, body) {
    if(err) {
      done(err);
    } else {
      /* Codes_SRS_NODE_DEVICE_TWIN_18_035: [** When a the results of a GET operation is received, the `Twin` object shall merge the properties into `this.properties.desired`. **]** */
      var props = JSON.parse(body.toString('ascii'));
      self._mergePatch(self.properties.desired, props.desired);
      self._mergePatch(self.properties.reported, props.reported);
      self._fireChangeEvents(self.properties.desired);
      done();
    }
  });
};

/* Codes_SRS_NODE_DEVICE_TWIN_18_039: [** After merging a GET result, the `Twin` object shall recursively fire property changed events for every changed property. **]** */
/* Codes_SRS_NODE_DEVICE_TWIN_18_040: [** After merging a PATCH result, the `Twin` object shall recursively fire property changed events for every changed property. **]** */
Twin.prototype._fireChangeEvents = function(desiredProperties) {
  var self = this;
  this.emit(Twin.desiredPath, desiredProperties);
  traverse(desiredProperties).forEach(function() {
    var path = this.path.join('.');
    if (path) {
      /* Codes_SRS_NODE_DEVICE_TWIN_18_041: [** When firing a property changed event, the `Twin` object shall name the event from the property using dot notation starting with 'properties.desired.' **]** */
      /* Codes_SRS_NODE_DEVICE_TWIN_18_042: [** When firing a property changed event, the `Twin` object shall pass the changed value of the property as the event parameter **]** */
      self.emit(Twin.desiredPath + '.' + path, _.at(desiredProperties,path)[0]);
    }
  });
};

/* Codes_SRS_NODE_DEVICE_TWIN_18_036: [** When a PATCH operation is received, the `Twin` object shall merge the properties into `this.properties.desired`. **]** */
Twin.prototype._onServicePost = function(body) {
  var patch = JSON.parse(body.toString('ascii'));
  this._mergePatch(this.properties.desired, patch);
  this._fireChangeEvents(patch);
};

/* Codes_SRS_NODE_DEVICE_TWIN_18_045: [** If a property is already set when a handler is added for that property, the `Twin` object shall fire a property changed event for the property. **]*  */
Twin.prototype._handleNewListener = function(eventName) {
  var self = this;  
  if (eventName.indexOf(Twin.desiredPath) === 0) {
    var propertyValue = _.at(this, eventName)[0];
    if (propertyValue) {
      process.nextTick(function() {
        self.emit(eventName, propertyValue);
      });
    }
  }
};

module.exports = Twin;

