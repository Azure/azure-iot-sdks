// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

module.exports = {
  anHourFromNow: require('./lib/authorization.js').anHourFromNow,
  encodeUriComponentStrict: require('./lib/authorization.js').encodeUriComponentStrict,
  ConnectionString: require('./lib/connection_string.js'),
  endpoint: require('./lib/endpoint.js'),
  errors: require('./lib/errors.js'),
  Message: require('./lib/message.js'),
  SharedAccessSignature: require('./lib/shared_access_signature.js')
};