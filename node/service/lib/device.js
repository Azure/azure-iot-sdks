// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var SymmetricKey = function(symmetricKeyInfo) {
  var symmKeys = symmetricKeyInfo;
    Object.defineProperties(this, {
    'primaryKey':{
      enumerable: true,
      get: function() {
        var primaryKey;
        if (symmKeys) {
          primaryKey = symmKeys.primaryKey;
        }
        return primaryKey;
      },
    },
    'secondaryKey': {
      enumerable: true,
      get: function() {
        var secondaryKey;
        if (symmKeys) {
          secondaryKey = symmKeys.secondaryKey;
        }
        return secondaryKey;
      },
    }
  });
};

var AuthenticationMechanism = function(authInfo)
{
  Object.defineProperties(this, {
    'SymmetricKey':{
      enumerable: true,
      get: function() {
        var symmKeys;
        if (authInfo) {
          if (authInfo.symmetricKey) {
            symmKeys = new SymmetricKey(authInfo.symmetricKey);
          }
        }
        return symmKeys;
      },
    }
  });
};

module.exports = function Device(jsonData) {
  var body;
  if (jsonData) {
    body = JSON.parse(jsonData);
    if (!body.deviceId) {
      throw new ReferenceError('Argument \'deviceId\' is ' + body.deviceId);
    }
  }
  else {
    body = {
      deviceId: null,
      generationId: null,
      etag: null,
      connectionState: 'Disconnected',
      status: 'Enabled',
      statusReason: null,
      connectionStateUpdatedTime: null,
      statusUpdatedTime: null,
      lastActivityTime: null,
      cloudToDeviceMessageCount: 0,
      authentication: {
        symmetricKey: {
          primaryKey: null,
          secondaryKey: null
        }
      }
    };
  }

  Object.defineProperties(this, {
    'deviceId':{
      enumerable: true,
      get: function() {
        return body.deviceId;
      },
      set: function(value) {
        body.deviceId = value;
      }
    },
    'generationId': {
      enumerable: true,
      get: function() {
        return body.generationId;
      },
    },
    'etag': {
      enumerable: true,
      get: function() {
        return body.etag;
      }
    },
    'connectionState': {
      enumerable: true,
      get: function() {
        return body.connectionState;
      }
    },
    'status': {
      enumerable: true,
      get: function() {
        return body.status;
      },
      set: function(value) {
        if (value.toLowerCase() !== 'enabled' && value.toLowerCase() !== 'disabled') {
          throw new RangeError('status is neither Enabled or Disabled');
        }
        body.status = value;
      }
    },
    'statusReason': {
      enumerable: true,
      get: function() {
        return body.statusReason;
      },
      set: function(value) {
        body.statusReason = value;
      }
    },
    'connectionStateUpdatedTime': {
      enumerable: true,
      get: function() {
        return body.connectionStateUpdatedTime;
      }
    },
    'statusUpdatedTime': {
      enumerable: true,
      get: function() {
        return body.statusUpdatedTime;
      }
    },
    'lastActivityTime': {
      enumerable: true,
      get: function() {
        return body.lastActivityTime;
      }
    },
    'cloudToDeviceMessageCount': {
      enumerable: true,
      get: function() {
        return body.cloudToDeviceMessageCount;
      }
    }
  });
  
  Object.defineProperty(this, 'authentication', {
    enumerable: true,
    get: function() {
      var key;
      if (body.authentication) {
        key = new AuthenticationMechanism(body.authentication);
      }
      return key;
    }
  });
};
