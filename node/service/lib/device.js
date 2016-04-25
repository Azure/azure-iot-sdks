// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var SystemPropInfo = function (systemProp) {
    var sysProp = systemProp;
    Object.defineProperties(this, {
        'batteryLevel': {
            enumerable: true,
            get: function () {
                var batteryLevel;
                if (sysProp) {
                    batteryLevel = sysProp.batteryLevel;
                }
                return batteryLevel;
            }
        },
        'batteryStatus': {
            enumerable: true,
            get: function () {
                var batteryStatus;
                if (sysProp) {
                    batteryStatus = sysProp.batteryStatus;
                }
                return batteryStatus;
            }
        },
        'currentTime': {
            enumerable: true,
            get: function () {
                var currentTime;
                if (sysProp) {
                    currentTime = sysProp.currentTime;
                }
                return currentTime;
            }
        },
        'defaultMaxPeriod': {
            enumerable: true,
            get: function () {
                var defaultMaxPeriod;
                if (sysProp) {
                    defaultMaxPeriod = sysProp.defaultMaxPeriod;
                }
                return defaultMaxPeriod;
            }
        },
        'defaultMinPeriod': {
            enumerable: true,
            get: function () {
                var defaultMinPeriod;
                if (sysProp) {
                    defaultMinPeriod = sysProp.defaultMinPeriod;
                }
                return defaultMinPeriod;
            }
        },
        'deviceDescription': {
            enumerable: true,
            get: function () {
                var deviceDescription;
                if (sysProp) {
                    deviceDescription = sysProp.deviceDescription;
                }
                return deviceDescription;
            }
        },
        'firmwarePackage': {
            enumerable: true,
            get: function () {
                var firmwarePackage;
                if (sysProp) {
                    firmwarePackage = sysProp.firmwarePackage;
                }
                return firmwarePackage;
            }
        },
        'firmwarePackageName': {
            enumerable: true,
            get: function () {
                var firmwarePackageName;
                if (sysProp) {
                    firmwarePackageName = sysProp.firmwarePackageName;
                }
                return firmwarePackageName;
            }
        },
        'firmwarePackageUri': {
            enumerable: true,
            get: function () {
                var firmwarePackageUri;
                if (sysProp) {
                    firmwarePackageUri = sysProp.firmwarePackageUri;
                }
                return firmwarePackageUri;
            }
        },
        'firmwarePackageVersion': {
            enumerable: true,
            get: function () {
                var firmwarePackageVersion;
                if (sysProp) {
                    firmwarePackageVersion = sysProp.firmwarePackageVersion;
                }
                return firmwarePackageVersion;
            }
        },
        'firmwareUpdateResult': {
            enumerable: true,
            get: function () {
                var firmwareUpdateResult;
                if (sysProp) {
                    firmwareUpdateResult = sysProp.firmwareUpdateResult;
                }
                return firmwareUpdateResult;
            }
        },
        'firmwareUpdateState': {
            enumerable: true,
            get: function () {
                var firmwareUpdateState;
                if (sysProp) {
                    firmwareUpdateState = sysProp.firmwareUpdateState;
                }
                return firmwareUpdateState;
            }
        },
        'firmwareVersion': {
            enumerable: true,
            get: function () {
                var firmwareVersion;
                if (sysProp) {
                    firmwareVersion = sysProp.firmwareVersion;
                }
                return firmwareVersion;
            }
        },
        'hardwareVersion': {
            enumerable: true,
            get: function () {
                var hardwareVersion;
                if (sysProp) {
                    hardwareVersion = sysProp.hardwareVersion;
                }
                return hardwareVersion;
            }
        },
        'manufacturer': {
            enumerable: true,
            get: function () {
                var manufacturer;
                if (sysProp) {
                    manufacturer = sysProp.manufacturer;
                }
                return manufacturer;
            }
        },
        'memoryFree': {
            enumerable: true,
            get: function () {
                var memoryFree;
                if (sysProp) {
                    memoryFree = sysProp.memoryFree;
                }
                return memoryFree;
            }
        },
        'memoryTotal': {
            enumerable: true,
            get: function () {
                var memoryTotal;
                if (sysProp) {
                    memoryTotal = sysProp.memoryTotal;
                }
                return memoryTotal;
            }
        },
        'modelNumber': {
            enumerable: true,
            get: function () {
                var modelNumber;
                if (sysProp) {
                    modelNumber = sysProp.modelNumber;
                }
                return modelNumber;
            }
        },
        'registrationLifetime': {
            enumerable: true,
            get: function () {
                var regLifetime;
                if (sysProp) {
                    regLifetime = sysProp.registrationLifetime;
                }
                return regLifetime;
            }
        },
        'serialNumber': {
            enumerable: true,
            get: function () {
                var serialNumber;
                if (sysProp) {
                    serialNumber = sysProp.serialNumber;
                }
                return serialNumber;
            }
        },
        'timezone': {
            enumerable: true,
            get: function () {
                var timezone;
                if (sysProp) {
                    timezone = sysProp.timezone;
                }
                return timezone;
            }
        },
        'utcOffset': {
           enumerable: true,
           get: function () {
               var utcOffset;
               if (sysProp) {
                   utcOffset = sysProp.utcOffset;
               }
               return utcOffset;
           }
       }         
    });
};

var ServicePropInfo = function (serviceProp) {
    var svcProp = serviceProp;
    Object.defineProperties(this, {
       'etag': {
           enumerable: true,
           get: function () {
               var etag;
               if (svcProp) {
                   etag = svcProp.etag;
               }
               else {
                   etag = '';
               }
               return etag;
           },
           set: function (value) {
               svcProp.etag = value;
           }
       },
       'properties': {
           enumerable: true,
           get: function () {
               var prop;
               if (svcProp) {
                   prop = svcProp.properties;
               }
               else {
                   prop = [];
               }
               return prop;
           }
       },
       'tags': {
           enumerable: true,
           get: function () {
               var tags;
               if (svcProp) {
                   tags = svcProp.tags;
               } else {
                   tags = [];
               }
               return tags;
           }
       }
    });
};

var SymmetricKey = function (symmetricKeyInfo) {
  var symmKeys = symmetricKeyInfo;
  Object.defineProperties(this, {
    'primaryKey': {
      enumerable: true,
      get: function () {
        var primaryKey;
        if (symmKeys) {
          primaryKey = symmKeys.primaryKey;
        }
        return primaryKey;
      },
    },
    'secondaryKey': {
      enumerable: true,
      get: function () {
        var secondaryKey;
        if (symmKeys) {
          secondaryKey = symmKeys.secondaryKey;
        }
        return secondaryKey;
      },
    }
  });
};

var AuthenticationMechanism = function (authInfo) {
  Object.defineProperties(this, {
    'symmetricKey': {
      enumerable: true,
      get: function () {
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

/**
 * @class           module:azure-iothub.Device
 * @classdesc       Creates a representation of a device for use in the
 *                  identity {@link module:azure-iothub.Registry} APIs.
 * @param {String}  jsonData        An optional JSON representation of the
 *                                  device, which will be mapped to properties
 *                                  in the object. If no argument is provided,
 *                                  Device roperties will be assigned default
 *                                  values.
 * @prop {String}   deviceId        Unique device identifier
 * @prop {String}   generationId    Used to disambiguate devices that have been
 *                                  deleted/recreated with the same `deviceId`
 * @prop {String}   etag            Weak entity tag assigned to this device
 *                                  identity description
 * @prop {String}   connectionState Whether the device is 'connected' or
 *                                  'disconnected'
 * @prop {String}   status          'enabled' (device authorized to connect,
 *                                  can send/receive messages) or 'disabled'
 * @prop {String}   statusReason    128-character string set when the device is
 *                                  disabled
 * @prop {String}   connectionStateUpdatedTime  Timestamp representing the last
 *                                  time `connectionState` changed
 * @prop {String}   statusUpdatedTime Timestamp representing the last time
 *                                  `status` changed
 * @prop {String}   lastActivityTime  Timestamp representing the last time the
 *                                  device authenticated, sent a message, or
 *                                  received a message
 * @prop {String}   cloudToDeviceMessageCount Number of c2d messages waiting to
 *                                  by delivered to the device
 * @prop {Object}   authentication  Contains the symmetric keys used to
 *                                  authenticate this device
 * @prop {Object}   serviceProperties Contains the Service Properties
 */
module.exports = function Device(sourceObj) {
  var body;
  if (sourceObj) {
    if (typeof sourceObj === 'string') {
      body = JSON.parse(sourceObj);
    } else {
      body = sourceObj;
    }
    
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
      serviceProperties: {
          etag: '',
          properties: [],
          tags: []
      },
      authentication: {
        symmetricKey: {
          primaryKey: null,
          secondaryKey: null
        }
      },
      systemProperties: {
          batteryLevel: '',
          batteryStatus: '',
          currentTime: '',
          defaultMaxPeriod: '',
          defaultMinPeriod: '',
          deviceDescription: '',
          firmwarePackage: '',
          firmwarePackageName: '',
          firmwarePackageUri: '',
          firmwarePackageVersion: '',
          firmwareUpdateResult: '',
          firmwareUpdateState: '',
          firmwareVersion: '',
          hardwareVersion: '',
          manufacturer: '',
          memoryFree: '',
          memoryTotal: '',
          modelNumber: '',
          registrationLifetime: '',
          serialNumber: '',
          timezone: '',
          utcOffset: ''
      }
    };
  }

  Object.defineProperties(this, {
    'deviceId': {
      enumerable: true,
      get: function () {
        return body.deviceId;
      },
      set: function (value) {
        body.deviceId = value;
      }
    },
    'generationId': {
      enumerable: true,
      get: function () {
        return body.generationId;
      },
    },
    'etag': {
      enumerable: true,
      get: function () {
        return body.etag;
      }
    },
    'connectionState': {
      enumerable: true,
      get: function () {
        return body.connectionState;
      }
    },
    'status': {
      enumerable: true,
      get: function () {
        return body.status;
      },
      set: function (value) {
        if (value.toLowerCase() !== 'enabled' && value.toLowerCase() !== 'disabled') {
          throw new RangeError('status is neither Enabled or Disabled');
        }
        body.status = value;
      }
    },
    'statusReason': {
      enumerable: true,
      get: function () {
        return body.statusReason;
      },
      set: function (value) {
        body.statusReason = value;
      }
    },
    'connectionStateUpdatedTime': {
      enumerable: true,
      get: function () {
        return body.connectionStateUpdatedTime;
      }
    },
    'statusUpdatedTime': {
      enumerable: true,
      get: function () {
        return body.statusUpdatedTime;
      }
    },
    'lastActivityTime': {
      enumerable: true,
      get: function () {
        return body.lastActivityTime;
      }
    },
    'cloudToDeviceMessageCount': {
      enumerable: true,
      get: function () {
        return body.cloudToDeviceMessageCount;
      }
    },
    'isManaged': {
        enumerable: true,
        get: function () {
            return body.isManaged;
        }
    }
  });

  Object.defineProperty(this, 'authentication', {
    enumerable: true,
    get: function () {
      var key;
      if (body.authentication) {
        key = new AuthenticationMechanism(body.authentication);
      }
      return key;
    }
  });
  
  Object.defineProperty(this, 'serviceProperties', {
     enumerable: true,
     get: function () {
         var svcProp;
         if (body && body.serviceProperties) {
             svcProp = new ServicePropInfo(body.serviceProperties);
         }
         else {
             svcProp = new ServicePropInfo(null);
         }
         return svcProp;
     }
  });  
  
  Object.defineProperty(this, 'systemProperties', {
     enumerable: true,
     get: function() {
         var sysProp;
         if (body && body.systemProperties) {
             sysProp = new SystemPropInfo(body.systemProperties);
         }
         else {
             sysProp = new SystemPropInfo(null);
         }
         return sysProp;         
     } 
  }); 
};
