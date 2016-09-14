// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var HttpBase = require('azure-iot-http-base').Http;
var errors = require('azure-iot-common').errors;
var uuid = require('uuid');
var PackageJson = require('../package.json');

function RestApiClient(config, httpRequestBuilder) {
  if(!config) throw new ReferenceError('config cannot be \'' + config + '\'');
  if(!config.host) throw new errors.ArgumentError('config.host cannot be \'' + config.host + '\'');
  if(!config.sharedAccessSignature) throw new errors.ArgumentError('config.sharedAccessSignature cannot be \'' + config.sharedAccessSignature + '\'');

  this._config = config;
  this._http = httpRequestBuilder || new HttpBase();
}

RestApiClient.prototype.executeApiCall = function (method, path, headers, requestBody, timeout, done) {
  if (!method) throw new ReferenceError('method cannot be \'' + method + '\'');
  if (!path) throw new ReferenceError('path cannot be \'' + path + '\'');

  if (done === undefined && typeof(timeout) === 'function') {
    done = timeout;
    timeout = undefined;
  }

  var httpHeaders = headers || {};
  httpHeaders.Authorization = this._config.sharedAccessSignature;
  httpHeaders['Request-Id'] = uuid.v4();
  httpHeaders['User-Agent'] = PackageJson.name + '/' + PackageJson.version;

  var request = this._http.buildRequest(method, path, httpHeaders, this._config.host, function (err, responseBody, response) {
    if (err) {
      if (response) {
        done(RestApiClient.translateError(responseBody, response));
      } else {
        done(err);
      }
    } else {
      var result = responseBody ? JSON.parse(responseBody) : '';
      done(null, result, response);
    }
  });

  if (timeout) {
    request.setTimeout(timeout);
  }

  if (requestBody) {
    request.write(JSON.stringify(requestBody));
  }

  request.end();
};

RestApiClient.translateError = function (body, response) {
  var error;
  var errorContent = HttpBase.parseErrorBody(body);
  var message = errorContent ? errorContent.message : "Error: " + body;

  switch(response.statusCode) {
    case 400:
      error = new errors.ArgumentError(message);
      break;
    case 401:
      error = new errors.UnauthorizedError(message);
      break;
    case 403:
      error = new errors.TooManyDevicesError(message);
      break;
    case 404:
      if (errorContent && errorContent.code === 'DeviceNotFound') {
        error = new errors.DeviceNotFoundError(message);
      } else if (errorContent && errorContent.code === 'IotHubNotFound') {
        error = new errors.IotHubNotFoundError(message);
      } else {
        error = new Error('Not found');
      }
      break;
    case 409:
      error = new errors.DeviceAlreadyExistsError(message);
      break;
    case 412:
      error = new errors.InvalidEtagError(message);
      break;
    case 429:
      error = new errors.ThrottlingError(message);
      break;
    case 500:
      error = new errors.InternalServerError(message);
      break;
    case 503:
      error = new errors.ServiceUnavailableError(message);
      break;
    default:
      error = new Error(message);
  }

  error.response = response;
  error.responseBody = body;
  return error;
};

RestApiClient.prototype.updateSharedAccessSignature = function (sharedAccessSignature) {
  if (!sharedAccessSignature) throw new ReferenceError('sharedAccessSignature cannot be \'' + sharedAccessSignature + '\'');

  this._config.sharedAccessSignature = sharedAccessSignature;
};

module.exports = RestApiClient;