#!/usr/bin/env node

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var nopt = require("nopt");
var colorsTmpl = require('colors-tmpl');
var prettyjson = require('prettyjson');
var Registry = require('azure-iothub').Registry;
var Https = require('azure-iothub').Https;

var expected = {
  "connection-string": Boolean,
  "display": String,
  "raw": Boolean
};

var parsed = nopt(expected, null, process.argv, 2);

if (!parsed.argv.remain.length) {
  if (!parsed.raw) {
    usage();
  }
  process.exit(1);
}

var connString, command, arg;

if ('HostName=' !== parsed.argv.remain[0].substr(0, 'HostName='.length)) {
  command = parsed.argv.remain[0];
  arg = parsed.argv.remain[1];
}
else {
  connString = parsed.argv.remain[0];
  command = parsed.argv.remain[1];
  arg = parsed.argv.remain[2];
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
  if (!arg) inputError('No device information given');
  var info;
  try {
    // 'create' command expects either deviceId or JSON device description
    info = (arg.charAt(0) !== '{') ? { "deviceId": arg } : JSON.parse(arg);
  }
  catch(e) {
    if (e instanceof SyntaxError) inputError('Device information isn\'t valid JSON');
    else throw e;
  }

  registry.create(info, function (err, res, device) {
    if (err) serviceError(err);
    else {
      if (!parsed.raw) {
        console.log(colorsTmpl('\n{green}Created device ' + arg + '{/green}'));
      }
      printDevice(device);
    }
  });
}
else if (command === 'get') {
  if (!arg) inputError('No device ID given');
  registry.get(arg, function (err, res, device) {
    if (err) serviceError(err);
    else printDevice(device);
  });
}
else if (command === 'delete') {
  if (!arg) inputError('No device ID given');
  registry.delete(arg, function (err, res) {
    if (err) serviceError(err);
    else if (!parsed.raw) {
      console.log(colorsTmpl('\n{green}Deleted device ' + arg + '{/green}'));
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
    '  {green}iothub-explorer{/green} {white}help{/white}',
    '    {grey}Displays this help message.{/grey}',
    '',
    '  {grey}Use the {white}--display{/white} option to show only the given properties from the azure-iothub.Device object.{/grey}',
    '  {grey}Use the {white}--connection-string{/white} option to generate a connection string for the device(s).{/grey}',
    '  {grey}Add the {white}--raw{/white} option to any command (except {white}help{/white}) to minimize output and format results as JSON.{/grey}'
  ].join('\n')));
}