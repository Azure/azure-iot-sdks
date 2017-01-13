#!/usr/bin/env node

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var fs = require('fs');
var nopt = require('nopt');
var uuid = require('uuid');
var prettyjson = require('prettyjson');
var colorsTmpl = require('colors-tmpl');

var Https = require('azure-iothub').Https;
var Client = require('azure-iothub').Client;
var Registry = require('azure-iothub').Registry;
var Message = require('azure-iot-common').Message;
var EventHubClient = require('azure-event-hubs').Client;
var ConnectionString = require('azure-iothub').ConnectionString;
var SharedAccessSignature = require('azure-iothub').SharedAccessSignature;
var anHourFromNow = require('azure-iot-common').anHourFromNow;
var DeviceSAS = require('azure-iot-device').SharedAccessSignature;

function Count(val) {
  this.val = +val;
}
Count.prototype = {
  toString: function () { return '' + this.val; },
  valueOf: function () { return this.val; }
};

nopt.typeDefs.Count = {
  type: Count,
  validate: function validateCount(data, key, val) {
    val = new Count(val);
    if (isNaN(val) || !val.valueOf() || val < 0 || val % 1 !== 0) return false;
    data[key] = val;
  }
};

var expected = {
  'ack': ['none', 'negative', 'positive', 'full'],
  'connection-string': Boolean,
  'duration': Count,
  'display': String,
  'messages': Count,
  'raw': Boolean
};

var parsed = nopt(expected, null, process.argv, 2);

if (!parsed.argv.remain.length) {
  if (!parsed.raw) {
    usage();
  }
  process.exit(1);
}

var connString, sas, command, arg1, arg2;

if ('HostName=' !== parsed.argv.remain[0].substr(0, 'HostName='.length)) {
  command = parsed.argv.remain[0];
  arg1 = parsed.argv.remain[1];
  arg2 = parsed.argv.remain[2];
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
else if (command === 'logout') {
  var loc = configLoc();
  try {
    fs.unlinkSync(loc.dir + '/' + loc.file);
  }
  catch (err) { // swallow file not found exception
    if (err.code !== 'ENOENT') throw err;
  }
  process.exit(0);
}
else if (command === 'login' && !connString) {
  connString = arg1;
  if (!connString) {
    inputError('Missing connection string');
  }
}

if (!connString) {
  var loc = configLoc();
  try {
    sas = fs.readFileSync(loc.dir + '/' + loc.file, 'utf8');
  }
  catch (err) { // swallow file not found exception
    if (err.code !== 'ENOENT') throw err;
  }

  if (!sas) {
    if (!parsed.raw) {
      inputError('Missing connection string');
    }
    process.exit(1);
  }

  sas = SharedAccessSignature.parse(sas);
}

var hostname = sas ? sas.sr : ConnectionString.parse(connString).HostName;

if (command === 'login') {
  var end = endTime();
  var cn = ConnectionString.parse(connString);
  var sas = SharedAccessSignature.create(cn.HostName, cn.SharedAccessKeyName, cn.SharedAccessKey, end);
  var loc = configLoc();

  if (isNaN(new Date(end * 1000))) {
    inputError('Invalid duration');
  }

  fs.mkdir(loc.dir, function () {
    fs.writeFile(loc.dir + '/' + loc.file, sas.toString(), function (err) {
      if (err) inputError(err.toString());
      else if (!parsed.raw) {
        console.log(colorsTmpl('\n{green}Session started, expires ' + new Date(end * 1000) + ' {/green}'));
      }
    });
  });
}
else if (command === 'list') {
  var registry = connString ? Registry.fromConnectionString(connString) : Registry.fromSharedAccessSignature(sas.toString());
  registry.list(function (err, list) {
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
  catch (e) {
    if (e instanceof SyntaxError) inputError('Device information isn\'t valid JSON');
    else throw e;
  }

  var registry = connString ? Registry.fromConnectionString(connString) : Registry.fromSharedAccessSignature(sas.toString());
  registry.create(info, function (err, device) {
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
  var registry = connString ? Registry.fromConnectionString(connString) : Registry.fromSharedAccessSignature(sas.toString());
  registry.get(arg1, function (err, device) {
    if (err) serviceError(err);
    else printDevice(device);
  });
}
else if (command === 'delete') {
  if (!arg1) inputError('No device ID given');
  var registry = connString ? Registry.fromConnectionString(connString) : Registry.fromSharedAccessSignature(sas.toString());
  registry.delete(arg1, function (err) {
    if (err) serviceError(err);
    else if (!parsed.raw) {
      console.log(colorsTmpl('\n{green}Deleted device ' + arg1 + '{/green}'));
    }
  });
}
else if (command === 'monitor-events') {
  if (!connString) inputError('\'monitor-events\' requires <connection-string> (for now)');
  if (!arg1) inputError('No device ID given');

  console.log(colorsTmpl('\n{grey}Monitoring events from device {green}' + arg1 + '{/green}{/grey}'));

  var startTime = Date.now();

  var ehClient = EventHubClient.fromConnectionString(connString);
  ehClient.open()
          .then(ehClient.getPartitionIds.bind(ehClient))
          .then(function (partitionIds) {
            return partitionIds.map(function (partitionId) {
              return ehClient.createReceiver('$Default', partitionId, { 'startAfterTime' : startTime}).then(function(receiver) {
                receiver.on('errorReceived', function (error) {
                  serviceError(error.message);
                });
                receiver.on('message', function (eventData) {
                  if (eventData.systemProperties['iothub-connection-device-id'] === arg1) {
                    console.log('Event received: ');
                    console.log((typeof eventData.body === 'string') ? eventData.body : JSON.stringify(eventData.body, null, 2));
                    console.log('');
                  }
                });
              });
            });
          })
          .catch(function (error) {
            serviceError(error.message);
          });
}
else if (command === 'send') {
  if (!arg1) inputError('No device ID given');
  if (!arg2) inputError('No message given');

  var deviceId = arg1;
  var message = new Message(arg2);

  if (parsed.ack) {
    message.messageId = uuid.v4();
    message.ack = parsed.ack;
  }

  var client = connString ? Client.fromConnectionString(connString) : Client.fromSharedAccessSignature(sas.toString());
  client.open(function (err) {
    if (err) {
      inputError('Could not open the connection to the service: ' + err.message);
    } else {
      client.send(deviceId, message, function (err) {
        if (err) serviceError(err);
        else {
          if (!parsed.raw) {
            var id = '';
            if (parsed.ack && parsed.ack !== 'none') {
              id = ' (id: ' + message.messageId + ')';
            }
            console.log(colorsTmpl('\n{green}Message sent{/green}' + id));
          }
          client.close(function (err) {
            if (err) serviceError(err);
          });
        }
      });
    }
  });

}
else if (command === 'receive') {
  var client = connString ? Client.fromConnectionString(connString) : Client.fromSharedAccessSignature(sas.toString());
  client.open(function (err) {
    if (err) {
      inputError('Could not open the connection to the service: ' + err.message);
    } else {
      client.getFeedbackReceiver(function (err, receiver) {
        var messageCount = parsed.messages || 0;
        var forever = !parsed.messages;

        if (err) serviceError(err);
        if (!parsed.raw) {
          console.log(colorsTmpl('\n{yellow}Waiting for feedback...{/yellow} (Ctrl-C to quit)'));
        }

        receiver.on('errorReceived', function (err) { serviceError(err); });
        receiver.on('message', function (feedbackRecords) {
          var records = JSON.parse(feedbackRecords.data);
          var output = {
            originalMessageId: records[0].originalMessageId,
            'iothub-enqueuedtime': records[0].enqueuedTimeUtc,
            body: records[0].description
          };

          var rendered = parsed.raw ?
            JSON.stringify(output) :
            '\nFeedback message\n' + prettyjson.render(output);

          console.log(rendered);

          if (!forever && --messageCount === 0) process.exit(0);
        });
      });
    }
  });
}
else if (command === 'sas-token') {
  if (!arg1) inputError('No device ID given');
  var registry = connString ? Registry.fromConnectionString(connString) : Registry.fromSharedAccessSignature(sas.toString());
  registry.get(arg1, function (err, device) {
    if (err)
      serviceError(err);
    else {
      var key = device.authentication.SymmetricKey.primaryKey;
      var expiry = endTime();
      if (!parsed.raw)
        console.log(colorsTmpl('{green}SAS Token for device ' + arg1 + ' with expiry ' + (parsed.duration ? parsed.duration : '3600') + ' seconds from now:{/green}'));
      var sas = DeviceSAS.create(hostname, arg1, key, expiry);
      console.log(sas.toString());
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

function endTime() {
  return parsed.duration ?
    Math.ceil((Date.now() / 1000) + parsed.duration) :
    anHourFromNow();
}

function connectionString(device) {
  return 'HostName=' + hostname + ';' +
    'DeviceId=' + device.deviceId + ';' +
    constructAuthenticationString(device.authentication);
}

function constructAuthenticationString(authenticationMechanism) {
  var authString = '';

  if (authenticationMechanism.SymmetricKey.primaryKey) {
    authString = 'SharedAccessKey=' + authenticationMechanism.SymmetricKey.primaryKey;
  } else if (authenticationMechanism.x509Thumbprint.primaryThumbprint || authenticationMechanism.x509Thumbprint.secondaryThumbprint) {
    authString = 'x509=true';
  }

  return authString;
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
    result.push({ connectionString: connectionString(device) });
  }

  var output = parsed.raw ?
    JSON.stringify(result) :
    '\n' + prettyjson.render(result);
  console.log(output);
}

function usage() {
  console.log(colorsTmpl([
    '',
    '{yellow}Usage{/yellow}',
    '  {green}iothub-explorer{/green} {white}login <connection-string> [--duration=<num-seconds>]{/white}',
    '    {grey}Creates a session lasting <num-seconds>; commands during the session can omit <connection-string>',
    '    Default duration is 3600 (one hour).{/grey}',
    '  {green}iothub-explorer{/green} {white}logout{/white}',
    '    {grey}Cancels any session started by \'login\'{/grey}',
    '  {green}iothub-explorer{/green} {white}[<connection-string>] list [--display="<property>,..."] [--connection-string]{/white}',
    '    {grey}Returns a list of (at most 1000) devices',
    '    Can optionally display only selected properties and/or connection strings.{/grey}',
    '  {green}iothub-explorer{/green} {white}[<connection-string>] get <device-id> [--display="<property>,..."] [--connection-string]{/white}',
    '    {grey}Returns information about the given device',
    '    Can optionally display just the selected properties and/or the connection string.{/grey}',
    '  {green}iothub-explorer{/green} {white}[<connection-string>] create <device-id|device-json> [--display="<property>,..."] [--connection-string]{/white}',
    '    {grey}Adds the given device to the IoT Hub and displays information about it',
    '    Can optionally display just the selected properties and/or the connection string.{/grey}',
    '  {green}iothub-explorer{/green} {white}[<connection-string>] delete <device-id>{/white}',
    '    {grey}Deletes the given device from the IoT Hub.{/grey}',
    '  {green}iothub-explorer{/green} {white}<connection-string> monitor-events <device-id>{/white}',
    '    {grey}Monitors and displays the events received from a specific device.{/grey}',
    '  {green}iothub-explorer{/green} {white}[<connection-string>] send <device-id> <msg> [--ack="none|positive|negative|full"]{/white}',
    '    {grey}Sends a cloud-to-device message to the given device, optionally with acknowledgment of receipt{/grey}',
    '  {green}iothub-explorer{/green} {white}[<connection-string>] receive [--messages=n]{/white}',
    '    {grey}Receives feedback about the delivery of cloud-to-device messages; optionally exits after receiving {white}n{/white} messages.{/grey}',
    '  {green}iothub-explorer{/green} {white}[<connection-string>] sas-token <device-id> [--duration=<num-seconds>]{/white}',
    '    {grey}Generates a SAS Token for the given device with an expiry time <num-seconds> from now',
    '    Default duration is 3600 (one hour).{/grey}',
    '  {green}iothub-explorer{/green} {white}help{/white}',
    '    {grey}Displays this help message.{/grey}',
    '',
    '  {grey}Use the {white}--display{/white} option to show only the given properties from the azure-iothub.Device object.{/grey}',
    '  {grey}Use the {white}--connection-string{/white} option to generate a connection string for the device(s).{/grey}',
    '  {grey}Add the {white}--raw{/white} option to any command (except {white}help{/white}) to minimize output and format results as JSON.{/grey}'
  ].join('\n')));
}