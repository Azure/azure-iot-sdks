// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Base = require('azure-iot-http-base').Http;
var uuid = require('uuid');
var PackageJson = require('../package.json');
var translateError = require('./registry_http_errors.js');

/*Codes_SRS_NODE_IOTHUB_HTTP_05_001: [The Http constructor shall accept an object with three properties:
host - (string) the fully-qualified DNS hostname of an IoT hub
hubName - (string) the name of the IoT hub, which is the first segment of hostname
sharedAccessSignature - (string) a shared access signature generated from the credentials of a policy with the appropriate registry persmissions (read and/or write).]*/
function Http(config) {
  this._config = config;
  this._http = new Base();
}

var handleHttpResponse = function handleHttpResponse (done) {
  return function (err, body, response) {
    if (!err) {
      done(null, body, response);
    } else if (response) {
      done(translateError(body, response));
    } else {
      done(err);
    }
  };
};

Http.prototype.createDevice = function (path, deviceInfo, done) {
  var config = this._config;
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'iothub-name': config.hubName,
    'Content-Type': 'application/json; charset=utf-8',
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_002: [The createDevice method shall construct an HTTP request using information supplied by the caller, as follows:
  PUT <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  iothub-name: <config.hubName>
  Content-Type: application/json; charset=utf-8
  If-Match: *
  Host: <config.host>

  <deviceInfo>
  ]
  */
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var request = this._http.buildRequest('PUT', path, httpHeaders, config.host, handleHttpResponse(done));
  request.write(JSON.stringify(deviceInfo));
  request.end();
};

Http.prototype.updateDevice = function (path, deviceInfo, done) {
  var config = this._config;
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'iothub-name': config.hubName,
    'Content-Type': 'application/json; charset=utf-8',
    'If-Match': '*',
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_003: [The updateDevice method shall construct an HTTP request using information supplied by the caller, as follows:
  PUT <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  iothub-name: <config.hubName>
  Content-Type: application/json; charset=utf-8
  Host: <config.host>

  <deviceInfo>
  ]
  */
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var request = this._http.buildRequest('PUT', path, httpHeaders, config.host, handleHttpResponse (done));
  request.write(JSON.stringify(deviceInfo));
  request.end();
};

Http.prototype.getDevice = function (path, done) {
  var config = this._config;
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'iothub-name': config.hubName,
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_004: [The getDevice method shall construct an HTTP request using information supplied by the caller, as follows:
  GET <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  iothub-name: <config.hubName>
  Host: <config.host>
  ]
  */
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var request = this._http.buildRequest('GET', path, httpHeaders, config.host, handleHttpResponse (done));
  request.end();
};

Http.prototype.listDevices = function (path, done) {
  var config = this._config;
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'iothub-name': config.hubName,
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_005: [The listDevices method shall construct an HTTP request using information supplied by the caller, as follows:
  GET <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  iothub-name: <config.hubName>
  Host: <config.host>
  ]
  */
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var request = this._http.buildRequest('GET', path, httpHeaders, config.host, handleHttpResponse (done));
  request.end();
};

Http.prototype.deleteDevice = function (path, done) {
  var config = this._config;
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'iothub-name': config.hubName,
    'If-Match': '*',
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_006: [The deleteDevice method shall construct an HTTP request using information supplied by the caller, as follows:
  DELETE <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  iothub-name: <config.hubName>
  If-Match: *
  Host: <config.host>
  ]
  */
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var request = this._http.buildRequest('DELETE', path, httpHeaders, config.host, handleHttpResponse (done));
  request.end();
};

Http.prototype.importDevicesFromBlob = function (path, importRequest, done) {
  var config = this._config;
  var requestId = uuid.v4();
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'Content-Type': 'application/json; charset=utf-8',
    'Request-Id': requestId,
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };

  /*Codes_SRS_NODE_IOTHUB_HTTP_16_005: [The importDeviceFromBlob method shall construct an HTTP request using information supplied by the caller, as follows:
  POST <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  Content-Type: application/json; charset=utf-8
  Request-Id: <guid>
  <importRequest>]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js
  http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var requestBody = JSON.stringify(importRequest);
  var request = this._http.buildRequest('POST', path, httpHeaders, config.host, handleHttpResponse (done));
  request.write(requestBody);
  request.end();
};

Http.prototype.exportDevicesToBlob = function (path, exportRequest, done) {
  var config = this._config;
  var requestId = uuid.v4();
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'Content-Type': 'application/json; charset=utf-8',
    'Request-Id': requestId,
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };

  /*Codes_SRS_NODE_IOTHUB_HTTP_16_004: [The exportDevicesToBlob method shall construct an HTTP request using information supplied by the caller, as follows:
  POST <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  Content-Type: application/json; charset=utf-8
  Request-Id: <guid>
  <exportRequest>]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js
  http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var requestBody = JSON.stringify(exportRequest);
  var request = this._http.buildRequest('POST', path, httpHeaders, config.host, handleHttpResponse (done));
  request.write(requestBody);
  request.end();
};

Http.prototype.listJobs = function (path, done) {
  var config = this._config;
  var requestId = uuid.v4();
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'Request-Id': requestId,
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };

  /*Codes_SRS_NODE_IOTHUB_HTTP_16_002: [The listJobs method shall construct an HTTP request using information supplied by the caller, as follows:
  GET <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  Request-Id: <guid>]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js
  http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var request = this._http.buildRequest('GET', path, httpHeaders, config.host, handleHttpResponse (done));
  request.end();
};

Http.prototype.getJob = function (path, done) {
  var config = this._config;
  var requestId = uuid.v4();
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'Request-Id': requestId,
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };

  /*Codes_SRS_NODE_IOTHUB_HTTP_16_003: [The getJob method shall construct an HTTP request using information supplied by the caller, as follows:
  GET <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  Request-Id: <guid>]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js
  http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var request = this._http.buildRequest('GET', path, httpHeaders, config.host, handleHttpResponse (done));
  request.end();
};

Http.prototype.cancelJob = function (path, done) {
  var config = this._config;
  var requestId = uuid.v4();
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'Request-Id': requestId,
    'User-Agent': 'azure-iothub/' + PackageJson.version
  };

  /*Codes_SRS_NODE_IOTHUB_HTTP_16_001: [The cancelJob method shall construct an HTTP request using information supplied by the caller as follows:
  DELETE <path>?api-version=<version> HTTP/1.1
  Authorization: <config.sharedAccessSignature>
  Request-Id: <guid>]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_007: [If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_008: [When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
  err - the standard JavaScript Error object, with the Node.js
  http.ServerResponse object attached as the property response.]*/
  /*Codes_SRS_NODE_IOTHUB_HTTP_05_009: [When any registry operation receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
  err - null
  body – the body of the HTTP response
  response - the Node.js http.ServerResponse object returned by the transport]*/
  var request = this._http.buildRequest('DELETE', path, httpHeaders, config.host, handleHttpResponse (done));
  request.end();
};

/* Codes_SRS_NODE_IOTHUB_HTTP_07_001: [The queryDeviceByTags method shall construct an HTTP request using information supplied by the caller as follows:
POST [path]?api-version=[version]&tags=[tagList]&top=[maxCount] HTTP/1.1
Authorization: [config.sharedAccessSignature]
Content-Type: application/json; charset=utf-8
Host: [host-name]]*/
Http.prototype.queryDevicesByTags = function (queryString, tagList, maxCount, done) {
  var config = this._config;
  var httpHeaders = {
    'Content-Type': 'application/json; charset=utf-8',
    'Authorization': config.sharedAccessSignature,
    'Accept': 'application/json',
    'Host': config.host,
    'User-Agent': PackageJson.name + '/' + PackageJson.version
  };

  var tagPath = tagList.join('%2C');

  var path = '/devices/query' + queryString + "&tags=" + tagPath + "&top=" + maxCount;
  var request = this._http.buildRequest('POST', path, httpHeaders, config.host, handleHttpResponse (done));
  request.end();
};

/*Codes_SRS_NODE_IOTHUB_HTTP_16_006: [The `queryDevices` method shall construct an HTTP request using information supplied by the caller as follows:
POST [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
Content-Type: application/json; charset=utf-8 
Host: [host-name]
[query]]*/
Http.prototype.queryDevices = function (queryString, query, done) {
  var config = this._config;
  var httpHeaders = {
    'Content-Type': 'application/json; charset=utf-8',
    'Authorization': config.sharedAccessSignature,
    'Accept': 'application/json',
    'Host': config.host,
    'User-Agent': PackageJson.name + '/' + PackageJson.version
  };

  var path = '/devices/query' + queryString;
  var body = JSON.stringify(query);

  var request = this._http.buildRequest('POST', path, httpHeaders, config.host, handleHttpResponse (done));
  request.write(body);
  request.end();
};

Http.prototype.setServiceProperties = function (path, serviceProperties, done) {
  var config = this._config;

  /*Codes_SRS_NODE_IOTHUB_HTTP_16_007: [The `setServiceProperties` method shall construct an HTTP request using information supplied by the caller as follows:
  ```
  PUT [path]?api-version=[version] HTTP/1.1
  Authorization: [config.sharedAccessSignature]
  Content-Type: application/json; charset=utf-8 
  Host: [host-name]
  User-Agent: 'azure-iothub/' + version
  [serviceProperties]
  ```]*/

  var body = JSON.stringify(serviceProperties);

  var httpHeaders = {
    'If-Match': '"' + serviceProperties.etag + '"',
    'Content-Type': 'application/json; charset=utf-8',
    'Authorization': config.sharedAccessSignature,
    'Accept': 'application/json',
    'Host': config.host,
    'User-Agent': PackageJson.name + '/' + PackageJson.version,
    'Content-Length': body.length
  };

  var request = this._http.buildRequest('PUT', path, httpHeaders, config.host, handleHttpResponse (done));
  request.write(body);
  request.end();
};

Http.prototype.getRegistryStatistics = function (path, done) {
  var config = this._config;
  /*Tests_SRS_NODE_IOTHUB_HTTP_16_007: [The `getRegistryStatistics` method shall construct an HTTP request using information supplied by the caller as follows:
  ```
  GET [path]?api-version=[version] HTTP/1.1
  Authorization: [config.sharedAccessSignature]
  Accept: application/json,
  Host: [host-name]
  [serviceProperties]
  ```]*/
  
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature,
    'Accept': 'application/json',
    'Host': config.host,
    'User-Agent': PackageJson.name + '/' + PackageJson.version
  };

  var request = this._http.buildRequest('GET', path, httpHeaders, config.host, handleHttpResponse (done));
  request.end();
};

module.exports = Http;