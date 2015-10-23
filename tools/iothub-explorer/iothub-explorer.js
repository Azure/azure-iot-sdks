#!/usr/bin/env node

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var nopt = require("nopt");
var uuid = require('uuid');
var colorsTmpl = require('colors-tmpl');
var prettyjson = require('prettyjson');
var Message = require('azure-iot-common').Message;
var Client = require('azure-iothub').Client;
var Registry = require('azure-iothub').Registry;
var Https = require('azure-iothub').Https;

var expected = {
  'connection-string': Boolean,
  'display': String,
  'raw': Boolean,
  'ack': ['none', 'negative', 'positive', 'full']
};

var parsed = nopt(expected, null, process.argv, 2);

if (!parsed.argv.remain.length) {
  if (!parsed.raw) {
    usage();
  }
  process.exit(1);
}

var connString, command, arg1, arg2;

if ('HostName=' !== parsed.argv.remain[0].substr(0, 'HostName='.length)) {
  command = parsed.argv.remain[0];
}
else {
  connString = parsed.argv.remain[0];
  command = parsed.argv.remain[1];
  arg1 = parsed.argv.remain[2];
  arg2 = parsed.argv.remain[3];
}

if (command === 'help') {
  usage();
  process.exit(0);
}

if (!connString)
{
  if (!parsed.raw) {
    inputError('Missing connection string');
    usage();
  }
  process.exit(1);
}

var registry = new Registry(connString, new Https());

if (command === 'list') {
  registry.list(function (err, res, list) {
    if (err) serviceError(err);
    else {
      list.forEach(function (device) {
        printDevice(device);
      });
    }
  });
}
else if (command === 'create') {
  if (!arg1) inputError('No device information given');
  var info;
  try {
    // 'create' command expects either deviceId or JSON device description
    info = (arg1.charAt(0) !== '{') ? { "deviceId": arg1 } : JSON.parse(arg1);
  }
  catch(e) {
    if (e instanceof SyntaxError) inputError('Device information isn\'t valid JSON');
    else throw e;
  }

  registry.create(info, function (err, res, device) {
    if (err) serviceError(err);
    else {
      if (!parsed.raw) {
        console.log(colorsTmpl('\n{green}Created device ' + arg1 + '{/green}'));
      }
      printDevice(device);
    }
  });
}
else if (command === 'get') {
  if (!arg1) inputError('No device ID given');
  registry.get(arg1, function (err, res, device) {
    if (err) serviceError(err);
    else printDevice(device);
  });
}
else if (command === 'delete') {
  if (!arg1) inputError('No device ID given');
  registry.delete(arg1, function (err, res) {
    if (err) serviceError(err);
    else if (!parsed.raw) {
      console.log(colorsTmpl('\n{green}Deleted device ' + arg1 + '{/green}'));
    }
  });
}
else if (command === 'send') {
  if (!arg1) inputError('No device ID given');
  if (!arg2) inputError('No message given');

  var message = new Message(arg2);

  if (parsed.ack) {
    message.messageId = uuid.v4();
    message.ack = parsed.ack;
  }

  var client = Client.fromConnectionString(connString);
  client.send(arg1, message, function (err, res) {
    if (err) serviceError(err);
    else {
      if (!parsed.raw) {
        var id = '';
        if (parsed.ack && parsed.ack !== 'none') {
          id = ' (id: ' + message.messageId + ')';
        }
        console.log(colorsTmpl('\n{green}Message sent{/green}' + id));
      }
      client.close();
    }
  });
}
else {
  inputError('\'' + command + '\' is not a valid command');
  usage();
}

function inputError(message) {
  if (!parsed.raw) {
    console.log(colorsTmpl('\n{bold}{red}Error{/red}{/bold} ' + message));
    usage();
  }
  process.exit(1);
}

function serviceError(err) {
  if (!parsed.raw) {
    var message = err.toString();
    if (message.lastIndexOf('Error:', 0) === 0) {
      message = message.slice('Error:'.length);
    }
    console.log(colorsTmpl('\n{bold}{red}Error{/red}{/bold}' + message));
  }
  process.exit(1);
}

function connectionString(device) {
  return 'HostName=' + registry.config.host + ';' +
    'DeviceId=' + device.deviceId + ';' +
    'SharedAccessKey=' + device.authentication.SymmetricKey.primaryKey;
}

function printDevice(device) {
  var filtered = {};
  if (parsed.display) {
    var props = parsed.display.split(',');
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

  var result = [filtered];
  if (parsed['connection-string']) {
    result.push({connectionString: connectionString(device)});
  }

  var output = parsed.raw ? JSON.stringify(result) : '\n' + prettyjson.render(result);
  console.log(output);
}

function usage() {
  console.log(colorsTmpl([
    '',
    '{yellow}Usage{/yellow}',
    '  {green}iothub-explorer{/green} {white}<connection-string> list [--display="<property>,..."] [--connection-string]{/white}',
    '    {grey}Returns a list of (at most 1000) devices',
    '    Can optionally display only selected properties and/or connection strings.{/grey}',
    '  {green}iothub-explorer{/green} {white}<connection-string> get <device-id> [--display="<property>,..."] [--connection-string]{/white}',
    '    {grey}Returns information about the given device',
    '    Can optionally display just the selected properties and/or the connection string.{/grey}',
    '  {green}iothub-explorer{/green} {white}<connection-string> create <device-id|device-json> [--display="<property>,..."] [--connection-string]{/white}',
    '    {grey}Adds the given device to the IoT Hub and displays information about it',
    '    Can optionally display just the selected properties and/or the connection string.{/grey}',
    '  {green}iothub-explorer{/green} {white}<connection-string> delete <device-id>{/white}',
    '    {grey}Deletes the given device from the IoT Hub.{/grey}',
    '  {green}iothub-explorer{/green} {white}<connection-string> send <device-id> <msg> [--ack="none|positive|negative|full"]{/white}',
    '    {grey}Sends a cloud-to-device message to the given device, optionally with acknowledgment of receipt{/grey}',
    '  {green}iothub-explorer{/green} {white}help{/white}',
    '    {grey}Displays this help message.{/grey}',
    '',
    '  {grey}Use the {white}--display{/white} option to show only the given properties from the azure-iothub.Device object.{/grey}',
    '  {grey}Use the {white}--connection-string{/white} option to generate a connection string for the device(s).{/grey}',
    '  {grey}Add the {white}--raw{/white} option to any command (except {white}help{/white}) to minimize output and format results as JSON.{/grey}'
  ].join('\n')));
}