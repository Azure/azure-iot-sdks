// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/**
 * Functions in the authorization module are used to generate shared-access
 * signature (SAS) tokens, which can be inserted into the `Authorization`
 * header of HTTP requests to Azure IoT Hub.
 *
 * @example
 * var auth = require('./src/common/authorization');
 *
 * var token = new auth.DeviceToken('hostname', deviceId, deviceKey, oneHourFromNow());
 * console.log(token.toString());
 *
 * token = new auth.ServiceToken('hostname', policyName, policyKey, oneHourFromNow());
 * console.log(token.toString());
 *
 * @module common/authorization
 * @see {@link https://msdn.microsoft.com/en-us/library/azure/dn170477.aspx|Shared Access Signature Authentication with Service Bus}
 */

'use strict';

var internal = require('./authorization_internals');

/**
 * @class Token
 * @classdesc Constructs a `Token` object.
 * @param {String}  resourceUri   The resource URI to encode into the token
 * @param {String}  keyName       An identifier associated with the key
 * @param {String}  key           A base64-encoded key value
 * @param {String}  expiry        An integer value representing the number
 *                                of seconds since the epoch 00:00:00 UTC
 *                                on 1 January 1970.
 */
/*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_001: [The Token constructor shall accept four arguments:
resourceUri - the resource URI to encode into the token
keyName - an identifier associated with the key 
key - a base64-encoded key value
expiry - an integer value representing the number of seconds since the epoch 00:00:00 UTC on 1 January 1970.]*/
function Token(resourceUri, keyName, key, expiry) {

  /*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_003: [The toString method shall return a shared-access signature token of the form:
  SharedAccessSignature sr=<resourceUri>&sig=<urlEncodedSignature>&se=<expiry>&skn=<urlEncodedKeyName>]*/
  this.toString = function () {
    var hash = internal.hmacHash(key, internal.stringToSign(resourceUri, expiry));
    return 'SharedAccessSignature ' +
      'sr=' + resourceUri +
      '&sig=' + internal.encodeUriComponentStrict(hash) +
      '&se=' + expiry +
      '&skn=' + internal.encodeUriComponentStrict(keyName);
  };
  
  function throwRef(name, value) {
    throw new ReferenceError('Argument \'' + name + '\' is ' + value);
  }

  /*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_002: [If resourceUri, key, or expiry are falsy (i.e., undefined, null, or empty), the Token constructor shall throw a reference exception.]  Note that keyName may be falsy.*/
  if (!resourceUri) throwRef('resourceUri', resourceUri);
  if (!key) throwRef('key', key);
  if (!expiry) throwRef('expiry', expiry);
  /*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_008: [If the keyName argument to the Token constructor was falsy, <urlEncodedKeyName> shall be empty.]*/
  keyName = keyName || '';
}

/**
 * @class DeviceToken
 * @classdesc The `DeviceToken` constructor function returns a token object
 *            that is suitable for device operations such as sending events
 *            or receiving messages. This constructor delegates to the
 *            {@linkcode Token} constructor.
 * @param {String}  host       The host part of the resource URI to encode
 *                             into the token
 * @param {String}  deviceId   The device identifier. This is URI encoded and
 *                             combined with `host` parameter and passed as the
 *                             `resourceUri` parameter to the `Token`
 *                             constructor like so:
 *                                 `<host>/devices/<uri encoded device id>`
 * @param {String}  key        A base64-encoded key value
 * @param {String}  expiry     An integer value representing the number
 *                             of seconds since the epoch 00:00:00 UTC
 *                             on 1 January 1970.
 */
/*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_009: [DeviceToken shall return an object created as if by passing the following arguments to the Token constructor:
resourceUri - <host>/devices/<urlEncodedDeviceId>
keyName - null
key - <key>
expiry - <expiry>]*/
function DeviceToken(host, deviceId, key, expiry) {
  var uri = internal.deviceResourceUri(host, deviceId);
  Token.call(this, uri, null, key, expiry);
}

DeviceToken.prototype = Object.create(Token.prototype);
DeviceToken.prototype.constructor = DeviceToken;

/**
 * The `toString` method returns a shared-access signature token of the form:
 * ```
 * SharedAccessSignature sr=<resourceUri>&sig=<urlEncodedSignature>&se=<expiry>&skn=<urlEncodedKeyName>
 * ```
 */
DeviceToken.prototype.toString = function toString() {
  Token.prototype.toString.call(this);
};

/**
 * @class ServiceToken
 * @classdesc The `ServiceToken` constructor function returns a token object
 *            that is suitable for service operations such as adding, removing,
 *            updating or reading device identities, or sending messages. This
 *            constructor delegates to the {@linkcode Token} constructor.
 * @param {String}  host       Used as the resource URI to encode into the
 *                             token
 * @param {String}  policy     An identifier associated with the key
 * @param {String}  key        A base64-encoded key value
 * @param {String}  expiry     An integer value representing the number
 *                             of seconds since the epoch 00:00:00 UTC
 *                             on 1 January 1970.
 */
/*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_010: [ServiceToken shall return an object created as if by passing the following arguments to the Token constructor:
resourceUri - <host>
keyName - <policy>
key - <key>
expiry - <expiry>]*/
function ServiceToken(host, policy, key, expiry) {
  var uri = internal.serviceResourceUri(host);
  Token.call(this, uri, policy, key, expiry);
}

ServiceToken.prototype = Object.create(Token.prototype);
ServiceToken.prototype.constructor = ServiceToken;

/**
 * The `toString` method returns a shared-access signature token of the form:
 * ```
 * SharedAccessSignature sr=<resourceUri>&sig=<urlEncodedSignature>&se=<expiry>&skn=<urlEncodedKeyName>
 * ```
 */
ServiceToken.prototype.toString = function toString() {
  Token.prototype.toString.call(this);
};


module.exports = {
  Token: Token,
  DeviceToken: DeviceToken,
  ServiceToken: ServiceToken
};
