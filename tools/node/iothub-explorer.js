// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var nopt = require("nopt");
var colorsTmpl = require('colors-tmpl');
var Registry = require('azure-iothub').Registry;
var Https = require('azure-iothub').Https;

var expected = {
  "id": Boolean
};

var parsed = nopt(expected, null, process.argv, 2);

var connString = parsed.argv.remain[0];
var command = parsed.argv.remain[1];
var arg = parsed.argv.remain[2];

if (!connString) {
  usage();
  process.exit(1);
}
var registry = new Registry(connString, new Https());

if (command == 'help') {
  usage();
}
else if (command == 'list') {
  registry.list(function (err, res, list) {
    if (err) serviceError(err);
    else {
      list.forEach(function (device) {
        printDevice(device);
      });
    }
  });
}
else if (command == 'create') {
  if (!arg) inputError('No device information given');
  var info;
  try {
    // 'create' command expects either deviceId or JSON device description
    info = (arg.charAt(0) != '{') ? { "deviceId": arg } : JSON.parse(arg);
  }
  catch(e) {
    if (e instanceof SyntaxError) inputError('Device information isn\'t valid JSON');
    else throw e;
  }

  registry.create(info, function (err, res, device) {
    if (err) serviceError(err);
    else {
      console.log(colorsTmpl('\n{green}Created device ' + arg + '{/green}\n'));
      printDevice(device);
    }
  });
}
else if (command == 'get') {
  if (!arg) inputError('No device ID given');
  registry.get(arg, function (err, res, device) {
    if (err) serviceError(err);
    else printDevice(device);
  });
}
else if (command == 'delete') {
  if (!arg) inputError('No device ID given');
  registry.delete(arg, function (err, res) {
    if (err) serviceError(err);
    else console.log(colorsTmpl('\n{green}Deleted device ' + arg + '{/green}'));
  });
}
else {
  inputError('\'' + command + '\' is not a valid command');
  usage();
}

function inputError(message) {
  console.log(colorsTmpl('\n{bold}{red}Error{/red}{/bold} ' + message));
  usage();
  process.exit(1);
}

function serviceError(err) {
  var message = err.toString();
  if (message.lastIndexOf('Error:', 0) === 0) {
    message = message.slice('Error:'.length);
  }
  console.log(colorsTmpl('\n{bold}{red}Error{/red}{/bold}' + message));
}

function printDevice(device) {
  console.log(parsed.id ? device.deviceId : JSON.stringify(device));
}

function usage() {
  console.log(colorsTmpl([
    '',
    '{yellow}Usage{/yellow}',
    '  {green}iothub-explorer{/green} {white}<connection-string> list [--id]{/white}',
    '    {grey}Returns a list of (at most 1000) devices, optionally displaying only identifiers.{/grey}',
    '  {green}iothub-explorer{/green} {white}<connection-string> get <device-id> [--id]{/white}',
    '    {grey}Returns information about the given device, optionally displaying only the identifier.{/grey}',
    '  {green}iothub-explorer{/green} {white}<connection-string> create <device-id|device-json> [--id]{/white}',
    '    {grey}Adds the given device to the IoT Hub and displays information about it, optionally displaying only the identifier.{/grey}',
    '  {green}iothub-explorer{/green} {white}<connection-string> delete <device-id>{/white}',
    '    {grey}Deletes the given device from the IoT Hub.{/grey}',
    '  {green}iothub-explorer{/green} {white}help{/white}',
    '    {grey}Displays this help message.{/grey}'
  ].join('\n')));
}