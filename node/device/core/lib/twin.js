// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var errors = require('azure-iot-common').errors;

var DeviceTwin = function(client) {
  EventEmitter.call(this);

  this._client = client;
  this._rid = 4200;   // arbitrary starting value.  
};

util.inherits(DeviceTwin, EventEmitter);

DeviceTwin.timeout = 1000;

DeviceTwin.errorEvent = 'error';
DeviceTwin.responseEvent = 'response';
DeviceTwin.subscribedEvent = 'subscribed';

/**
 * @method          module:azure-iot-device.DeviceTwin#fromDeviceClient
 * @description     Get a DeviceTwin object for the given client connection
 *
 * @param {Object}      client  The [client]{@link module:azure-iot-device.Client} object that this DeviceTwin object is associated with.
 *
 * @param {Function}      done  the callback to be invoked when this function completes.
 *
 * @throws {ReferenceError}   One of the required parameters is falsy
 */

DeviceTwin.fromDeviceClient = function(client, done) {
  /* Codes_SRS_NODE_DEVICE_TWIN_18_002: [** `fromDeviceclient` shall throw `ReferenceError` if the `client` object is falsy **]** */
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
    /* Codes_SRS_NODE_DEVICE_TWIN_18_029: [** if `fromDeviceClient` is called with 2 different `client`s, it shall return 2 unique `DeviceTwin` objects **]** */
    /* Codes_SRS_NODE_DEVICE_TWIN_18_003: [** `fromDeviceClient` shall allocate a new `DeviceTwin` object **]**  */
    var twin = new DeviceTwin(client);

    /* Codes_SRS_NODE_DEVICE_TWIN_18_005: [** If the protocol does not contain a `getTwinReceiver` method, `fromDeviceClient` shall call the `done` callback with a `NotImplementedError` object **]**  */
    if (!client._transport.getTwinReceiver) {
      done(new errors.NotImplementedError('transport does not support DeviceTwin'));
    } else {
      /* Codes_SRS_NODE_DEVICE_TWIN_18_004: [** `fromDeviceClient` shall call `getTwinReceiver` on the protocol object to get a twin receiver. **]**  */
      client._transport.getTwinReceiver(function(err, receiver) {
        if (err) {
          done(err);
        } else {
          twin._receiver = receiver;

          twin._subscribe( function(err) {
            client._twin = twin;
            twin._clearReportedProperties();
            done(err, twin);
          });
        }
      });
    }
  }
};

DeviceTwin.prototype._subscribe = function(done) {
  
  var twin = this;
  var receiver = twin._receiver;

  /* Codes_SRS_NODE_DEVICE_TWIN_18_011: [** `fromDeviceClient` shall call the `done` callback with `err`=`null` if it receives a `subscribed` event for the `response` topic. **]**  */
  var handleSubscribed = function(obj) {
    if (obj.eventName === DeviceTwin.responseEvent) {
      cleanupAndReturn(null, twin);
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
    receiver.removeListener(DeviceTwin.subscribedEvent, handleSubscribed);
    receiver.removeListener(DeviceTwin.errorEvent, handleError);
    done(err);
  };

  /* Codes_SRS_NODE_DEVICE_TWIN_18_007: [** `fromDeviceClient` shall add handlers for the both the `subscribed` and `error` events on the twinReceiver **]**  */
  receiver.on(DeviceTwin.subscribedEvent, handleSubscribed);
  receiver.on(DeviceTwin.errorEvent, handleError);

  /* Codes_SRS_NODE_DEVICE_TWIN_18_009: [** `fromDeviceClient` shall call the `done` callback passing a `TimeoutError` if it has not received a `subscribed` event within `DeviceTwin.timeout` milliseconds. **]**  */
  timeout = setTimeout(function() {
    cleanupAndReturn(new errors.TimeoutError('subscription to twin messages timed out'), null);
  }, DeviceTwin.timeout);
    
  receiver.on(DeviceTwin.responseEvent, twin._onServiceResponse);
};

DeviceTwin.prototype._sendTwinRequest = function(method, resource, properties, body, done) {

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
       cleanupAndReturn(null);
      } else {
        /* Codes_SRS_NODE_DEVICE_TWIN_18_020: [** `_sendTwinRequest` shall call `done` with an `err` value translated using http_errors.js **]**  */
        // TODO: use http_errors.js -- how?
        cleanupAndReturn(new errors.DeviceNotFoundError('response returned status = '+resp.status));
      }
    }
  };
  
  /* Codes_SRS_NODE_DEVICE_TWIN_18_021: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]**  */
  var timeout = null;
  var cleanupAndReturn = function(err) {
    clearTimeout(timeout);
    timeout = null;
    self._receiver.removeListener('response', handleResponse);
    done(err);
  };

  /* Codes_SRS_NODE_DEVICE_TWIN_18_015: [** `_sendTwinRequest` shall add a handler for the `response` event on the twinReceiver object.  **]** */
  this._receiver.on('response', handleResponse);

  /* Codes_SRS_NODE_DEVICE_TWIN_18_022: [** If the response doesnt come within `DeviceTwin.timeout` milliseconds, `_sendTwinRequest` shall call `done` with `err`=`TimeoutError` **]**   */
  timeout = setTimeout(function() {
    cleanupAndReturn(new errors.TimeoutError('message timed out'), null);
  }, DeviceTwin.timeout);

  /* Codes_SRS_NODE_DEVICE_TWIN_18_016: [** `_sendTwinRequest` shall use the `sendTwinRequest` method on the transport to send the request **]**  */
  this._client._transport.sendTwinRequest(method, resource, propCopy, body);
};


DeviceTwin.prototype._updateReportedProperties = function (state, done) {
  /* Codes_SRS_NODE_DEVICE_TWIN_18_025: [** `properties.reported.update` shall use _sendTwinRequest to send the patch object to the service. **]**  */
  /* Codes_SRS_NODE_DEVICE_TWIN_18_026: [** When calling `_sendTwinRequest`, `properties.reported.update` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the `body` parameter passed in to `reportState` as a string. **]**    */
  /* Codes_SRS_NODE_DEVICE_TWIN_18_027: [** `properties.reported.update` shall call `done` with the results from `_sendTwinRequest` **]**  */
  this._sendTwinRequest('PATCH', '/properties/reported/', {}, JSON.stringify(state), done);
};

DeviceTwin.prototype._onServiceResponse = function() {
};

DeviceTwin.prototype._clearReportedProperties = function () {
  var self = this;
  this.properties = { 
    reported : {
      update : function(state, done) {
        self._updateReportedProperties(state, done);
      }
    }
  };
};
        


module.exports = DeviceTwin;

