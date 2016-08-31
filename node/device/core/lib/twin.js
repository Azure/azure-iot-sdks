// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var errors = require('azure-iot-common').errors;

var Twin = function(client) {
  EventEmitter.call(this);

  this._client = client;
  this._rid = 4200;   // arbitrary starting value.  
};

util.inherits(Twin, EventEmitter);

Twin.timeout = 1000;

Twin.errorEvent = 'error';
Twin.responseEvent = 'response';
Twin.subscribedEvent = 'subscribed';

/**
 * @method          module:azure-iot-device.Twin#fromDeviceClient
 * @description     Get a Device Twin object for the given client connection
 *
 * @param {Object}      client  The [client]{@link module:azure-iot-device.Client} object that this Twin object is associated with.
 *
 * @param {Function}      done  the callback to be invoked when this function completes.
 *
 * @throws {ReferenceError}   One of the required parameters is falsy
 */

/* Codes_SRS_NODE_DEVICE_TWIN_18_001: [** `fromDeviceclient` shall accept 2 parameters: an azure-iot-device `client` and a `done` method. **]** */
Twin.fromDeviceClient = function(client, done) {
/* Codes_SRS_NODE_DEVICE_TWIN_18_002: [** `fromDeviceclient` shall throw `ReferenceError` if the client object is falsy **]** */
  if (!client) {
    throw new ReferenceError('client parameter is required');
  }
  /* Codes_SRS_NODE_DEVICE_TWIN_18_030: [** `fromDeviceclient` shall throw `ReferenceError` if the `done` argument is falsy **]** */
  if (!done) {
    throw new ReferenceError('done parameter is required');
  }

  /* Codes_SRS_NODE_DEVICE_TWIN_18_028: [** if `fromDeviceClient` has previously been called for this client, it shall return the same object **]** */
  if (client._twin) {
    done(null,client._twin);
  } else {
    /* Codes_SRS_NODE_DEVICE_TWIN_18_029: [** if `fromDeviceClient` is called with 2 different `client`s, it shall return 2 unique `Twin` objects **]** */
    /* Codes_SRS_NODE_DEVICE_TWIN_18_003: [** `fromDeviceClient` shall allocate a new `Twin` object **]**  */
    var twin = client._twin = new Twin(client);

    /* Codes_SRS_NODE_DEVICE_TWIN_18_005: [** If the protocol does not contain a `getTwinReceiver` method, `fromDeviceClient` shall return a `NotImplementedError` in the `done` callback. **]**  */
    if (!client._transport.getTwinReceiver) {
      done(new errors.NotImplementedError('transport does not support DeviceTwin'));
    } else {
      /* Codes_SRS_NODE_DEVICE_TWIN_18_004: [** `fromDeviceClient` shall call `getTwinReceiver` on the protocol object to get a twin receiver. **]**  */
      client._transport.getTwinReceiver(function(err, receiver) {
        if (err) {
          done(err);
        } else {
          /* Codes_SRS_NODE_DEVICE_TWIN_18_006: [** `fromDeviceClient` shall store the twinReceiver obejct as a property **]**  */
          twin._receiver = receiver;

          twin._subscribe( function(err) {
            done(err, twin);
          });
        }
      });
    }
  }
};

Twin.prototype._subscribe = function(done) {
  
  var twin = this;
  var receiver = twin._receiver;

  /* Codes_SRS_NODE_DEVICE_TWIN_18_011: [** `fromDeviceClient` shall call `done` with `err`=`null` if it receives a `subscribed` events for the `response` topic. **]**  */
  var handleSubscribed = function(obj) {
      if (obj.eventName === Twin.responseEvent) {
        cleanupAndReturn(null, twin);
      }
  };

  /* Codes_SRS_NODE_DEVICE_TWIN_18_010: [** `fromDeviceClient` shall return the first error that is returned from `error` event on the twinReceiver. **]**  */
  var errAlreadySent = false;
  var handleError = function(topic) {
      if (!errAlreadySent) {
        errAlreadySent = true;
        cleanupAndReturn(new errors.ServiceUnavailableError('failed to subscribe to'   + topic));
      }
  };
      

  /* Codes_SRS_NODE_DEVICE_TWIN_18_012: [** `fromDeviceClient` shall remove the handlers for both the `subscribed` and `error` events ** ] ** */
  var timeout;
  var cleanupAndReturn = function(err) {
    if (timeout) {
      clearTimeout(timeout);
      timeout = null;
    }
    receiver.removeListener(Twin.subscribedEvent, handleSubscribed);
    receiver.removeListener(Twin.errorEvent, handleError);
    done(err);
  };

  receiver.on(Twin.subscribedEvent, handleSubscribed);
  receiver.on(Twin.errorEvent, handleError);

  /* Codes_SRS_NODE_DEVICE_TWIN_18_009: [** `fromDeviceClient` shall return a `ServiceUnavailableError` it has not received a `subscribed` event within `Twin.timeout` milliseconds. **]**  */
  timeout = setTimeout(function() {
    cleanupAndReturn(new errors.ServiceUnavailableError('subscription to twin messages timed out'), null);
  }, Twin.timeout);
    
  /* Codes_SRS_NODE_DEVICE_TWIN_18_008: [** `fromDeviceClient` shall add `_onServiceResponse` as a handler for the `response` event on the twinReceiver **]**  */
  receiver.on(Twin.responseEvent, twin._onServiceResponse);
};

/* Codes_SRS_NODE_DEVICE_TWIN_18_013: [** `_sendTwinRequest` shall accept a `method`, `resource`, `properties`, `body`, and `done` callback **]**  */
Twin.prototype._sendTwinRequest = function(method, resource, properties, body, done) {

  /* Codes_SRS_NODE_DEVICE_TWIN_18_014: [** `_sendTwinRequest` shall use an arbitrary starting `rid` and incriment by it by one for every call to `_sendTwinRequest` **]**  */
  /* Codes_SRS_NODE_DEVICE_TWIN_18_017: [** `_sendTwinRequest` shall put the `rid` value into the `properties` object that gets passed to `sendTwinRequest` on the transport **]**  */
  properties.$rid = this._rid;
  this._rid++;

  var self = this;

  console.log('sending');
  console.log('rid = ' + properties.$rid);
  console.log(body);

  var handleResponse = function(resp) {
    console.log('received response');
    console.log(resp);
    /* Codes_SRS_NODE_DEVICE_TWIN_18_018: [** The response handler shall ignore any messages that don't have an `rid` that mattches the `rid` of the request **]**   */
    if (resp.$rid.toString() === properties.$rid.toString())
    {
      /* Codes_SRS_NODE_DEVICE_TWIN_18_019: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 **]**  */
      if (resp.status.toString() === '200') {
       cleanupAndReturn(null);
      } else {
        /* Codes_SRS_NODE_DEVICE_TWIN_18_020: [** `_sendTwinRequest` shall call `done` with an `err` value generated using http_errors.js **]**  */
        // TODO: use http_errors.js -- how?
        cleanupAndReturn(new errors.DeviceNotFoundError('response returned status = '+resp.status));
      }
    }
  };
  
  /* Codes_SRS_NODE_DEVICE_TWIN_18_021: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]**  */
  var timeout;
  var cleanupAndReturn = function(err) {
    if (timeout) {
      clearTimeout(timeout);
      timeout = null;
    }
    self._receiver.removeListener('response', handleResponse);
    done(err);
  };

  /* Codes_SRS_NODE_DEVICE_TWIN_18_015: [** `_sendTwinRequest` shall add a handler for the `response` event on the twinReceiver object.  **]** */
  this._receiver.on('response', handleResponse);

  timeout = setTimeout(function() {
    cleanupAndReturn(new errors.ServiceUnavailableError('message timed out'), null);
  }, Twin.timeout);

  /* Codes_SRS_NODE_DEVICE_TWIN_18_016: [** `_sendTwinRequest` shall use the `sendTwinReqest` method on the transport to send the request **]**  */
  this._client._transport.sendTwinRequest(method, resource, properties, body);
};


/* Codes_SRS_NODE_DEVICE_TWIN_18_024: [** `reportTwinState` shall accept an object which represents the `reported` state and a `done` callback **]**  */
Twin.prototype.reportTwinState = function (state, done) {
/* Codes_SRS_NODE_DEVICE_TWIN_18_025: [** `reportTwinState` shall use _sendTwinRequest to send the patch object to the service. **]**  */
/* Codes_SRS_NODE_DEVICE_TWIN_18_026: [** When calling `_sendTwinRequest`, `reportTwinState` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the result of `_createPatchObject` as a `String`. **]**    */
/* Codes_SRS_NODE_DEVICE_TWIN_18_027: [** `reportTwinState` shall call `done` with the results from `_sendTwinRequest` **]**  */
  this._sendTwinRequest('PATCH', '/properties/reported/', {}, JSON.stringify(state), done);
};

Twin.prototype._onServiceResponse = function() {
  /* Codes_SRS_NODE_DEVICE_TWIN_18_023: [** `_onServiceResponse` shall ignore all parameters and immediately return. **]**  */
};

module.exports = Twin;



