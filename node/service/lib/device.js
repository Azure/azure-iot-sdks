// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var DevicePropInfo = function (deviceProp) {
    var devProp = deviceProp;
    Object.defineProperties(this, {
        'batteryLevel': {
            enumerable: true,
            get: function () {
                var batteryLevel;
                if (devProp) {
                    batteryLevel = devProp.batteryLevel;
                }
                return batteryLevel;
            }
        },
        'batteryStatus': {
            enumerable: true,
            get: function () {
                var batteryStatus;
                if (devProp) {
                    batteryStatus = devProp.batteryStatus;
                }
                return batteryStatus;
            }
        },
        'currentTime': {
            enumerable: true,
            get: function () {
                var currentTime;
                if (devProp) {
                    currentTime = devProp.currentTime;
                }
                return currentTime;
            }
        },
        'defaultMaxPeriod': {
            enumerable: true,
            get: function () {
                var defaultMaxPeriod;
                if (devProp) {
                    defaultMaxPeriod = devProp.defaultMaxPeriod;
                }
                return defaultMaxPeriod;
            }
        },
        'defaultMinPeriod': {
            enumerable: true,
            get: function () {
                var defaultMinPeriod;
                if (devProp) {
                    defaultMinPeriod = devProp.defaultMinPeriod;
                }
                return defaultMinPeriod;
            }
        },
        'deviceDescription': {
            enumerable: true,
            get: function () {
                var deviceDescription;
                if (devProp) {
                    deviceDescription = devProp.deviceDescription;
                }
                return deviceDescription;
            }
        },
        'firmwarePackage': {
            enumerable: true,
            get: function () {
                var firmwarePackage;
                if (devProp) {
                    firmwarePackage = devProp.firmwarePackage;
                }
                return firmwarePackage;
            }
        },
        'firmwarePackageName': {
            enumerable: true,
            get: function () {
                var firmwarePackageName;
                if (devProp) {
                    firmwarePackageName = devProp.firmwarePackageName;
                }
                return firmwarePackageName;
            }
        },
        'firmwarePackageUri': {
            enumerable: true,
            get: function () {
                var firmwarePackageUri;
                if (devProp) {
                    firmwarePackageUri = devProp.firmwarePackageUri;
                }
                return firmwarePackageUri;
            }
        },
        'firmwarePackageVersion': {
            enumerable: true,
            get: function () {
                var firmwarePackageVersion;
                if (devProp) {
                    firmwarePackageVersion = devProp.firmwarePackageVersion;
                }
                return firmwarePackageVersion;
            }
        },
        'firmwareUpdateResult': {
            enumerable: true,
            get: function () {
                var firmwareUpdateResult;
                if (devProp) {
                    firmwareUpdateResult = devProp.firmwareUpdateResult;
                }
                return firmwareUpdateResult;
            }
        },
        'firmwareUpdateState': {
            enumerable: true,
            get: function () {
                var firmwareUpdateState;
                if (devProp) {
                    firmwareUpdateState = devProp.firmwareUpdateState;
                }
                return firmwareUpdateState;
            }
        },
        'firmwareVersion': {
            enumerable: true,
            get: function () {
                var firmwareVersion;
                if (devProp) {
                    firmwareVersion = devProp.firmwareVersion;
                }
                return firmwareVersion;
            }
        },
        'hardwareVersion': {
            enumerable: true,
            get: function () {
                var hardwareVersion;
                if (devProp) {
                    hardwareVersion = devProp.hardwareVersion;
                }
                return hardwareVersion;
            }
        },
        'manufacturer': {
            enumerable: true,
            get: function () {
                var manufacturer;
                if (devProp) {
                    manufacturer = devProp.manufacturer;
                }
                return manufacturer;
            }
        },
        'memoryFree': {
            enumerable: true,
            get: function () {
                var memoryFree;
                if (devProp) {
                    memoryFree = devProp.memoryFree;
                }
                return memoryFree;
            }
        },
        'memoryTotal': {
            enumerable: true,
            get: function () {
                var memoryTotal;
                if (devProp) {
                    memoryTotal = devProp.memoryTotal;
                }
                return memoryTotal;
            }
        },
        'modelNumber': {
            enumerable: true,
            get: function () {
                var modelNumber;
                if (devProp) {
                    modelNumber = devProp.modelNumber;
                }
                return modelNumber;
            }
        },
        'registrationLifetime': {
            enumerable: true,
            get: function () {
                var regLifetime;
                if (devProp) {
                    regLifetime = devProp.registrationLifetime;
                }
                return regLifetime;
            }
        },
        'serialNumber': {
            enumerable: true,
            get: function () {
                var serialNumber;
                if (devProp) {
                    serialNumber = devProp.serialNumber;
                }
                return serialNumber;
            }
        },
        'timezone': {
            enumerable: true,
            get: function () {
                var timezone;
                if (devProp) {
                    timezone = devProp.timezone;
                }
                return timezone;
            }
        },
        'utcOffset': {
           enumerable: true,
           get: function () {
               var utcOffset;
               if (devProp) {
                   utcOffset = devProp.utcOffset;
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
 * @prop {Object}   deviceProperties Contains the Service Properties
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
      deviceProperties: {
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
  
  Object.defineProperty(this, 'deviceProperties', {
     enumerable: true,
     get: function() {
         var devProp;
         if (body && body.deviceProperties) {
             devProp = new DevicePropInfo(body.deviceProperties);
         }
         else {
             devProp = new DevicePropInfo(null);
         }
         return devProp;         
     } 
  }); 
};
