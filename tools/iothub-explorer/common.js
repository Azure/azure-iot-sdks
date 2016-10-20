// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

// node native modules
var fs = require('fs');

// external dependencies
var colorsTmpl = require('colors-tmpl');
var prettyjson = require('prettyjson');
var uuid = require('uuid');

// sdk dependencies
var errors = require('azure-iot-common').errors;
var Message = require('azure-iot-common').Message;
var ConnectionString = require('azure-iothub').ConnectionString;
var SharedAccessSignature = require('azure-iothub').SharedAccessSignature;

function createDeviceConnectionString(deviceInfo, hubHostName) {
  var cs = 'HostName=' + hubHostName + ';DeviceId=' + deviceInfo.deviceId;
  if (deviceInfo.authentication.SymmetricKey.primaryKey) {
    cs += ';SharedAccessKey=' + deviceInfo.authentication.SymmetricKey.primaryKey;
  } else if (deviceInfo.authentication.SymmetricKey.secondaryKey) {
    cs += ';SharedAccessKey=' + deviceInfo.authentication.SymmetricKey.secondaryKey;
  } else if (deviceInfo.authentication.x509Thumbprint.primaryThumbprint || deviceInfo.authentication.x509Thumbprint.secondaryThumbprint) {
    cs += ';x509=true';
  } else {
    cs = null;
  }

  return cs;
}

function createMessageFromArgument(messageArg, ack) {
  var message;
  function createMessage (body) {
    var msg = new Message(body);
    msg.messageId = uuid.v4();
    if (ack) {
      msg.ack = ack;
    }
    return msg;
  }

  /**
   * The message passed on the command line can either be the payload, or the message itself.
   * To try and figure this out, we try to parse the message as JSON. If it works, we then look for the messageId property.
   * If it can't be parse or if there is no messageId property then we treat the argument as a payload. If the argument is
   * proper JSON and has a messageId property, we consider that the argument is a full message object.
   */
  try {
    var tmpMessage = JSON.parse(messageArg);
    if (tmpMessage.messageId) {
      message = tmpMessage;
      message.ack = tmpMessage.ack || ack;
    } else {
      message = createMessage(tmpMessage);
    }
  } catch(e) {
    if (e instanceof SyntaxError) {
      message = createMessage(messageArg);
    } else {
      throw e;
    }
  }

  return message;
}

function inputError(message) {
  printErrorAndExit(message, 'Input Error:');
}

function printErrorAndExit(message, prefix) {
  if (!prefix) {
    prefix = 'Error:';
  }

  console.error(colorsTmpl('\n{bold}{red}' + prefix + '{/red}{/bold} ' + message));
  process.exit(1);
}

function serviceError(err) {
  var message = err.toString();
  if (message.lastIndexOf('Error:', 0) === 0) {
    message = message.slice('Error:'.length);
  }
  printErrorAndExit(message);
}

function printSuccess(message) {
  console.log(colorsTmpl('{green}' + message + '{/green}'));
}

/**
 * printDevice will display a device either pretty-printed or as raw JSON.
 * 
 * @param {any} device                  The device object received from the IoT hub registry.
 * @param {any} hubHostName             used in case showConnectionString is true.
 * @param {any} propertyFilter          Filter the properties that should be displayed.
 * @param {any} showConnectionString    Boolean indicating whether the connection string should be printed out (requires hubHostName)
 * @param {any} rawOutput               Boolean indicating whether the output should be pretty-printed or displayed as raw JSON.
 */
function printDevice(device, hubHostName, propertyFilter, showConnectionString, rawOutput) {
  var filtered = {};
  if (propertyFilter) {
    var props = propertyFilter.split(',');
    props.forEach(function (prop) {
      prop = prop.trim();
      var parts = prop.split('.');
      var src = device;
      var dst = filtered;
      for (var i = 0; i < parts.length; ++i) {
        var part = parts[i];
        if (src[part]) {
          if (i + 1 === parts.length) {
            dst[part] = src[part];
          }
          else {
            dst[part] = {};
            src = src[part];
            dst = dst[part];
          }
        }
      }
    });
  }
  else {
    filtered = device;
  }

  var result = filtered;
  if (showConnectionString) {
    result.connectionString = createDeviceConnectionString(device, hubHostName);
  }

  var output = rawOutput ? JSON.stringify(result) : prettyjson.render(result);
  console.log(output);
}

function configLoc() {
  if (process.platform === 'darwin') {
    return {
      dir: process.env.HOME + '/Library/Application Support/iothub-explorer',
      file: 'config'
    };
  }
  else if (process.platform === 'linux') {
    return {
      dir: process.env.HOME,
      file: '.iothub-explorer'
    };
  }
  else if (process.platform === 'win32') {
    return {
      dir: process.env.LOCALAPPDATA + '/iothub-explorer',
      file: 'config'
    };
  }
  else {
    inputError('\'login\' not supported on this platform');
  }
}

function loadSasFromUserFile() {
  var sas;
  var loc = configLoc();
  try {
    sas = fs.readFileSync(loc.dir + '/' + loc.file, 'utf8');
  }
  catch (err) { // swallow file not found exception
    if (err.code !== 'ENOENT') throw err;
  }

  return sas;
}

function getSas(connectionString) {
  var sas;
  if (connectionString) {
    var cn;
    try {
      cn = ConnectionString.parse(connectionString);
    } catch (e) {
      if (e instanceof errors.ArgumentError) {
        inputError('Could not parse connection string: ' + connectionString);
      } else {
        throw e;
      }
    }
    var expiry = Math.floor(Date.now() / 1000) + 3600;
    sas = SharedAccessSignature.create(cn.HostName, cn.SharedAccessKeyName, cn.SharedAccessKey, expiry).toString();
  } else {
    sas = loadSasFromUserFile();
  }

  if (!sas) {
    inputError('You must either use the login command or the --login argument for iothub-explorer to authenticate with your IoT Hub instance');
  }

  return sas;
}

function getHostFromSas(sas) {
  return SharedAccessSignature.parse(sas).sr;
}

module.exports = {
  inputError: inputError,
  serviceError: serviceError,
  printSuccess: printSuccess,
  printErrorAndExit: printErrorAndExit,
  printDevice: printDevice,
  getHostFromSas: getHostFromSas,
  getSas: getSas,
  configLoc: configLoc,
  createMessageFromArgument: createMessageFromArgument,
  createDeviceConnectionString: createDeviceConnectionString
};