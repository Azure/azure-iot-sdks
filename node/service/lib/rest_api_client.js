// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var anHourFromNow = require('azure-iot-common').anHourFromNow;
var HttpBase = require('azure-iot-http-base').Http;
var errors = require('azure-iot-common').errors;
var uuid = require('uuid');
var PackageJson = require('../package.json');

/**
 * @class       module:azure-iothub.RestApiClient
 * @classdesc   Constructs an {@linkcode RestApiClient} object that can be used to make REST calls to the IoT Hub service.
 *
 * @params {Object}  config              The configuration object that should be used to connect to the IoT Hub service.
 * @params {Object}  httpRequestBuilder  OPTIONAL: The base http transport object. `azure-iot-common.Http` will be used if no argument is provided.
 * 
 * @throws {ReferenceError}  If the config argument is falsy
 * @throws {ArgumentError}   If the config argument is missing a host or sharedAccessSignature error
 */
function RestApiClient(config, httpRequestBuilder) {
  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_001: [The `RestApiClient` constructor shall throw a `ReferenceError` if config is falsy.]*/
  if(!config) throw new ReferenceError('config cannot be \'' + config + '\'');
  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_002: [The `RestApiClient` constructor shall throw an `ArgumentError` if config is missing a `host` or `sharedAccessSignature` property.]*/
  if(!config.host) throw new errors.ArgumentError('config.host cannot be \'' + config.host + '\'');
  if(!config.sharedAccessSignature) throw new errors.ArgumentError('config.sharedAccessSignature cannot be \'' + config.sharedAccessSignature + '\'');

  this._config = config;

  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_003: [The `RestApiClient` constructor shall use `azure-iot-common.Http` as the internal HTTP client if the `httpBase` argument is `undefined`.]*/
  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_004: [The `RestApiClient` constructor shall use the value of the `httpBase` argument as the internal HTTP client if present.]*/
  this._http = httpRequestBuilder || new HttpBase();
}

/**
 * @method             module:azure-iothub.RestApiClient.executeApiCall
 * @description        Creates an HTTP request, sends it and parses the response, then call the callback with the resulting object.
 * 
 * @param {Function}   method        The HTTP method that should be used.
 * @param {Function}   path          The path for the HTTP request.
 * @param {Function}   headers       Headers to add to the request on top of the defaults (Authorization, Request-Id and User-Agent will be populated automatically).
 * @param {Function}   requestBody   Body of the HTTP request.
 * @param {Function}   timeout       [optional] Custom timeout value.
 * @param {Function}   done          Called when a response has been received or if an error happened.
 * 
 * @throws {ReferenceError} If the method or path arguments are falsy.
 * @throws {TypeError}      If the type of the requestBody is not a string when Content-Type is text/plain
 */
RestApiClient.prototype.executeApiCall = function (method, path, headers, requestBody, timeout, done) {
  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_005: [The `executeApiCall` method shall throw a `ReferenceError` if the `method` argument is falsy.]*/
  if (!method) throw new ReferenceError('method cannot be \'' + method + '\'');
  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_006: [The `executeApiCall` method shall throw a `ReferenceError` if the `path` argument is falsy.]*/
  if (!path) throw new ReferenceError('path cannot be \'' + path + '\'');

  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_029: [If `done` is `undefined` and the `timeout` argument is a function, `timeout` should be used as the callback.]*/
  if (done === undefined && typeof(timeout) === 'function') {
    done = timeout;
    timeout = undefined;
  }

  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_007: [The `executeApiCall` method shall add the following headers to the request:
  - Authorization: <this.sharedAccessSignature>
  - Request-Id: <guid>
  - User-Agent: <version string>]*/
  var httpHeaders = headers || {};
  httpHeaders.Authorization = (typeof(this._config.sharedAccessSignature) === 'string') ? this._config.sharedAccessSignature : this._config.sharedAccessSignature.extend(anHourFromNow());
  httpHeaders['Request-Id'] = uuid.v4();
  httpHeaders['User-Agent'] = PackageJson.name + '/' + PackageJson.version;

  var requestBodyString = null;

  if (requestBody) {
    /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_035: [If there's is a `Content-Type` header and its value is `application/json; charset=utf-8` and the `requestBody` argument is a `string` it shall be used as is as the body of the request.]*/
    /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_031: [If there's is a `Content-Type` header and its value is `application/json; charset=utf-8` and the `requestBody` argument is not a `string`, the body of the request shall be stringified using `JSON.stringify()`.]*/
    if (!!headers['Content-Type'] && headers['Content-Type'].indexOf('application/json') >= 0) {
      if (typeof requestBody === 'string') {
        requestBodyString = requestBody;
      } else {
        requestBodyString = JSON.stringify(requestBody);
      }
    } else if (!!headers['Content-Type'] && headers['Content-Type'].indexOf('text/plain') >= 0) {
      if (typeof requestBody !== 'string') {
        /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_033: [The `executeApiCall` shall throw a `TypeError` if there's is a `Content-Type` header and its value is `text/plain; charset=utf-8` and the `body` argument is not a string.]*/
        throw new TypeError('requestBody must be a string');
      } else {
        /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_032: [If there's is a `Content-Type` header and its value is `text/plain; charset=utf-8`, the `requestBody` argument shall be used.]*/
        requestBodyString = requestBody;
      }
    }

    /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_036: [The `executeApiCall` shall set the `Content-Length` header to the length of the serialized value of `requestBody` if it is truthy.]*/
    headers['Content-Length'] = requestBodyString.length;
  }

  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_008: [The `executeApiCall` method shall build the HTTP request using the arguments passed by the caller.]*/
  var request = this._http.buildRequest(method, path, httpHeaders, this._config.host, function (err, responseBody, response) {
    if (err) {
      if (response) {
        /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_010: [If the HTTP request fails with an error code >= 300 the `executeApiCall` method shall translate the HTTP error into a transport-agnostic error using the `translateError` method and call the `done` callback with the resulting error as the only argument.]*/
        done(RestApiClient.translateError(responseBody, response));
      } else {
        /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_011: [If the HTTP request fails without an HTTP error code the `executeApiCall` shall call the `done` callback with the error itself as the only argument.]*/
        done(err);
      }
    } else {
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_009: [If the HTTP request is successful the `executeApiCall` method shall parse the JSON response received and call the `done` callback with a `null` first argument, the parsed result as a second argument and the HTTP response object itself as a third argument.]*/
      var result = responseBody ? JSON.parse(responseBody) : '';
      done(null, result, response);
    }
  });

  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_030: [If `timeout` is defined and is not a function, the HTTP request timeout shall be adjusted to match the value of the argument.]*/
  if (timeout) {
    request.setTimeout(timeout);
  }

  if(requestBodyString) {
    request.write(requestBodyString);
  }

  request.end();
};

/**
 * @method             module:azure-iothub.RestApiClient.translateError
 * @description        Translates an HTTP error into a transport-agnostic error.
 * 
 * @param {string}   body        The HTTP error response body.
 * @param {string}   response    The HTTP response itself.
 * 
 */
/*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_027: [`translateError` shall accept 2 arguments:
- the body of  the HTTP response, containing the explanation of why the request failed.
- the HTTP response object itself.]*/
RestApiClient.translateError = function (body, response) {
  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_012: [Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 3 properties:
  - `response` shall contain the `IncomingMessage` object returned by the HTTP layer.
  - `reponseBody` shall contain the content of the HTTP response.
  - `message` shall contain a human-readable error message.]*/
  var error;
  var errorContent = HttpBase.parseErrorBody(body);
  var message = errorContent ? errorContent.message : "Error: " + body;

  switch(response.statusCode) {
    case 400:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_014: [`translateError` shall return an `ArgumentError` if the HTTP response status code is `400`.]*/
      error = new errors.ArgumentError(message);
      break;
    case 401:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_015: [`translateError` shall return an `UnauthorizedError` if the HTTP response status code is `401`.]*/
      error = new errors.UnauthorizedError(message);
      break;
    case 403:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_016: [`translateError` shall return an `TooManyDevicesError` if the HTTP response status code is `403`.]*/
      error = new errors.TooManyDevicesError(message);
      break;
    case 404:
      if (errorContent && errorContent.code === 'DeviceNotFound') {
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_018: [`translateError` shall return an `DeviceNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `DeviceNotFound`.]*/
        error = new errors.DeviceNotFoundError(message);
      } else if (errorContent && errorContent.code === 'IotHubNotFound') {
        /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_017: [`translateError` shall return an `IotHubNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `IotHubNotFound`.]*/
        error = new errors.IotHubNotFoundError(message);
      } else {
        error = new Error('Not found');
      }
      break;
    case 408:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_019: [`translateError` shall return a `DeviceTimeoutError` if the HTTP response status code is `408`.]*/
      error = new errors.DeviceTimeoutError(message);
      break;
    case 409:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_020: [`translateError` shall return an `DeviceAlreadyExistsError` if the HTTP response status code is `409`.]*/
      error = new errors.DeviceAlreadyExistsError(message);
      break;
    case 412:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_021: [`translateError` shall return an `InvalidEtagError` if the HTTP response status code is `412`.]*/
      error = new errors.InvalidEtagError(message);
      break;
    case 429:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_022: [`translateError` shall return an `ThrottlingError` if the HTTP response status code is `429`.]*/
      error = new errors.ThrottlingError(message);
      break;
    case 500:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_023: [`translateError` shall return an `InternalServerError` if the HTTP response status code is `500`.]*/
      error = new errors.InternalServerError(message);
      break;
    case 502:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_024: [`translateError` shall return a `BadDeviceResponseError` if the HTTP response status code is `502`.]*/
      error = new errors.BadDeviceResponseError(message);
      break;
    case 503:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_025: [`translateError` shall return an `ServiceUnavailableError` if the HTTP response status code is `503`.]*/
      error = new errors.ServiceUnavailableError(message);
      break;
    case 504:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_026: [`translateError` shall return a `GatewayTimeoutError` if the HTTP response status code is `504`.]*/
      error = new errors.GatewayTimeoutError(message);
      break;
    default:
      /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_013: [If the HTTP error code is unknown, `translateError` should return a generic Javascript `Error` object.]*/
      error = new Error(message);
  }

  error.response = response;
  error.responseBody = body;
  return error;
};

/**
 * @method             module:azure-iothub.RestApiClient.updateSharedAccessSignature
 * @description        Updates the shared access signature used to authentify API calls.
 * 
 * @param  {string}          sharedAccessSignature  The new shared access signature that should be used.
 * 
 * @throws {ReferenceError}  If the new sharedAccessSignature is falsy.
 */
RestApiClient.prototype.updateSharedAccessSignature = function (sharedAccessSignature) {
  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_034: [The `updateSharedAccessSignature` method shall throw a `ReferenceError` if the `sharedAccessSignature` argument is falsy.]*/
  if (!sharedAccessSignature) throw new ReferenceError('sharedAccessSignature cannot be \'' + sharedAccessSignature + '\'');

  /*Codes_SRS_NODE_IOTHUB_REST_API_CLIENT_16_028: [The `updateSharedAccessSignature` method shall update the `sharedAccessSignature` configuration parameter that is used in the `Authorization` header of all HTTP requests.]*/
  this._config.sharedAccessSignature = sharedAccessSignature;
};

module.exports = RestApiClient;