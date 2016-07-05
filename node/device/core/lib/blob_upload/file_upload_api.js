// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var endpoint = require('azure-iot-common').endpoint;
var packageJson = require('../../package.json');
var DefaultHttpTransport = require('azure-iot-http-base').Http;


/**
 * @class         module:azure-iot-device.FileUploadApi
 * @classdesc     Provides methods to use Azure IoT Hub APIs that enable simple upload to a blob.
 * 
 * @params        {String}  deviceId   Device identifier.
 * @params        {String}  hostname   Hostname of the Azure IoT Hub instance.
 * @params        {Object}  transport  Transport layer that shall be used (HTTP or mock).
 * 
 * @throws        {ReferenceError}     Thrown if one of the parameters is falsy.
 */
/*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_002: [`FileUploadApi` shall throw a `ReferenceError` if `deviceId` is falsy.]*/
/*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_003: [`FileUploadApi` shall throw a `ReferenceError` if `hostname` is falsy.]*/
function FileUploadApi(deviceId, hostname, httpTransport) {
  if (!deviceId) throw new ReferenceError('deviceId cannot be \'' + deviceId + '\'');
  if (!hostname) throw new ReferenceError('hostname cannot be \'' + hostname + '\'');
  
  this.deviceId = deviceId;
  this.hostname = hostname;
  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_018: [`FileUploadApi` shall instantiate the default `azure-iot-http-base.Http` transport if `transport` is not specified, otherwise it shall use the specified transport.]*/
  this.http = httpTransport ? httpTransport : new DefaultHttpTransport();
}

FileUploadApi.prototype.getBlobSharedAccessSignature = function (blobName, auth, done) {
  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_004: [`getBlobSharedAccessSignature` shall throw a `ReferenceError` if `blobName` is falsy.]*/
  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_005: [`getBlobSharedAccessSignature` shall throw a `ReferenceError` if `auth` is falsy.]*/
  if (!blobName) throw new ReferenceError('blobName cannot be \'' + blobName + '\'');
  if (!auth) throw new ReferenceError('auth cannot be \'' + auth + '\'');

  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_006: [`getBlobSharedAccessSignature` shall create a `GET` HTTP request to a path formatted as the following:`/devices/<deviceId>/files/<filename>?api-version=<api-version>]*/
  var path = endpoint.devicePath(this.deviceId) + '/files/' + blobName + endpoint.versionQueryString();

  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_007: [The `GET` HTTP request shall have the following headers:
  ```
  Accept: 'application/json'
  Host: <hostname>
  Authorization: <iotHubSas>
  'User-Agent': <sdk name and version>,
  ```]*/
  var headers = {
    Accept: 'application/json',
    Host: this.hostname,
    'User-Agent': packageJson.name + '/' + packageJson.version,
  };

  var x509Opts = null;
  if (typeof auth === 'string') {
    headers.Authorization  = auth;
  } else {
    x509Opts = auth;
  }

  var req = this.http.buildRequest('GET', path, headers, this.hostname, x509Opts, function (err, body, response) {
    if (err) {
      err.responseBody = body;
      err.response = response;
      /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_008: [`getBlobSasUri` shall call the `done` callback with an `Error` object if the request fails.]*/
      done(err);
    } else {
      /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_009: [`getBlobSasUri` shall call the `done` callback with a `null` error object and a result object containing a correlation ID and a SAS parameters if the request succeeds]*/
      var result = JSON.parse(body);
      done(null, result);
    }
  });
  req.end();
};

FileUploadApi.prototype.notifyUploadComplete = function (correlationId, auth, uploadResult, done) {
  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_010: [`notifyUploadComplete` shall throw a `ReferenceError` if `correlationId` is falsy.]*/
  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_011: [`notifyUploadComplete` shall throw a `ReferenceError` if `auth` is falsy.]*/
  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_012: [`notifyUploadComplete` shall throw a `ReferenceError` if `uploadResult` is falsy.]*/
  if (!correlationId) throw new ReferenceError('correlationId cannot be \'' + correlationId + '\'');
  if (!auth) throw new ReferenceError('auth cannot be \'' + auth + '\'');
  if (!uploadResult) throw new ReferenceError('uploadResult cannot be \'' + uploadResult + '\'');

  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_013: [`notifyUploadComplete` shall create a `POST` HTTP request to a path formatted as the following:`/devices/<deviceId>/files/<correlationId>?api-version=<api-version>`]*/
  var path = endpoint.devicePath(this.deviceId) + '/files/notifications/' + encodeURIComponent(correlationId) + endpoint.versionQueryString();
  var body = JSON.stringify(uploadResult);
  
  /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_014: [The `POST` HTTP request shall have the following headers:
  ```
  Host: <hostname>,
  Authorization: <sharedAccessSignature>,
  'User-Agent': <version>,
  'Content-Type': 'application/json; charset=utf-8',
  'Content-Length': <content length>,
  'iothub-name': <hub name>
  ```]*/
  var headers = {
    Host: this.hostname,
    'User-Agent': packageJson.name + '/' + packageJson.version,
    'Content-Type': 'application/json; charset=utf-8',
    'Content-Length': body.length,
    'iothub-name': this.hostname.split('.')[0]
  };

  var x509Opts = null;
  if (typeof auth === 'string') {
    headers.Authorization  = auth;
  } else {
    x509Opts = auth;
  }

  var req = this.http.buildRequest('POST', path, headers, this.hostname, x509Opts, function (err) {
    if (err) {
      /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_016: [** `notifyUploadComplete` shall call the `done` callback with an `Error` object if the request fails.]*/
      done(err);
    } else {
      /*Codes_SRS_NODE_FILE_UPLOAD_ENDPOINT_16_017: [** `notifyUploadComplete` shall call the `done` callback with no parameters if the request succeeds.]*/
      done();
    }
  });
  req.write(body);
  req.end();
};

module.exports = FileUploadApi;