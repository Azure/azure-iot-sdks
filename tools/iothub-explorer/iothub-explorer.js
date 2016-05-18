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
var JobClient = require('azure-iothub').JobClient;
var Message = require('azure-iot-common').Message;
var EventHubClient = require('./lib/eventhubclient.js');
var ConnectionString = require('azure-iothub').ConnectionString;
var SharedAccessSignature = require('azure-iothub').SharedAccessSignature;
var anHourFromNow = require('azure-iot-common').anHourFromNow;

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
  'async': Boolean,
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
else if (command === 'registry-stats') {
  var registry = connString ? Registry.fromConnectionString(connString) : Registry.fromSharedAccessSignature(sas.toString());
  registry.getRegistryStatistics(function (err, statistics) {
    if (err) serviceError(err);
    else {
      printResult(statistics);
    }
  });
}
else if (command === 'monitor-events') {
  if (!connString) inputError('\'monitor-events\' requires <connection-string> (for now)');
  if (!arg1) inputError('No device ID given');

  console.log(colorsTmpl('\n{grey}Monitoring events from device {green}' + arg1 + '{/green}{/grey}'));

  var startTime = Date.now();

  var ehClient = new EventHubClient(connString, 'messages/events/');
  ehClient.GetPartitionIds().then(function (partitionIds) {
    partitionIds.forEach(function (partitionId) {
      ehClient.CreateReceiver('$Default', partitionId).then(function (receiver) {
        // start receiving
        receiver.StartReceive(startTime).then(function () {
          receiver.on('error', function (error) {
            serviceError(error.description);
          });
          receiver.on('eventReceived', function (eventData) {
            if ((eventData.SystemProperties['iothub-connection-device-id'] === arg1) &&
              (eventData.SystemProperties['x-opt-enqueued-time'] >= startTime)) {
              console.log('Event received: ');
              console.log(eventData.Bytes);
              console.log('');
            }
          });
        });
        return receiver;
      });
    });
    return partitionIds;
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
          client.close();
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
          var output = {
            'iothub-enqueuedtime': feedbackRecords.data[0].enqueuedTimeUtc,
            body: feedbackRecords.data[0].description
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
else if (command === 'get-job') {
  if (!arg1) inputError('No job ID given');
  
  var jobClient = connString ? 
    JobClient.fromConnectionString(connString) :
    JobClient.fromSharedAccessSignature(sas.toString());
    
    jobClient.getJob(arg1, function (err, job) {
      if (err) serviceError(err);
      printResult(job);
    });
}
else if (command === 'read') {
  if (!arg1) inputError('No device IDs given');
  if (!arg2) inputError('No device properties given');

  var jobId = uuid.v4();
  var devices = arrayFromCommaDelimitedList(arg1);
  var properties = arrayFromCommaDelimitedList(arg2);
  var jobClient = connString ? 
    JobClient.fromConnectionString(connString) :
    JobClient.fromSharedAccessSignature(sas.toString());

  jobClient.scheduleDevicePropertyRead(jobId, devices, properties, function (err, job) {
    handleJobScheduled(err, job, 'Read', function () {
      var registry = connString ?
        Registry.fromConnectionString(connString) :
        Registry.fromSharedAccessSignature(sas.toString());
      devices.forEach(function (deviceId) {
        registry.get(deviceId, function (err, twin) {
          if (err) serviceError(err);
          var result = {
            deviceId: deviceId,
            deviceProperties: {}
          };
          properties.forEach(function (value) {
            result.deviceProperties[value] = twin.deviceProperties[value];
          });
          printResult(result);
        });
      });
    });
  });
}
else if (command === 'write') {
  if (!arg1) inputError('No device IDs given');
  if (!arg2) inputError('No device properties given');

  var jobId = uuid.v4();
  var devices = arrayFromCommaDelimitedList(arg1);
  var properties;
  
  try {
    properties = JSON.parse(arg2);
  }
  catch (e) {
    if (e instanceof SyntaxError) inputError('Properties information isn\'t valid JSON');
    else throw e;
  }

  var jobClient = connString ? 
    JobClient.fromConnectionString(connString) :
    JobClient.fromSharedAccessSignature(sas.toString());

  jobClient.scheduleDevicePropertyWrite(jobId, devices, properties, function (err, job) {
    handleJobScheduled(err, job, 'Write');
  });
}
else if (command === 'firmware-update')
{
  if (!arg1) inputError('No device IDs given');
  if (!arg2) inputError('No firmware image URI given');

  var jobId = uuid.v4();
  var devices = arrayFromCommaDelimitedList(arg1);
  var timeout = parsed.timeout || 60; // default is one hour

  var jobClient = connString ?
    JobClient.fromConnectionString(connString) :
    JobClient.fromSharedAccessSignature(sas.toString());

  jobClient.scheduleFirmwareUpdate(jobId, devices, arg2, timeout, function (err, job) {
    handleJobScheduled(err, job, 'Firmware update');
  });
}
else if (command === 'factory-reset')
{
  if (!arg1) inputError('No device IDs given');

  var jobId = uuid.v4();
  var devices = arrayFromCommaDelimitedList(arg1);

  var jobClient = connString ?
    JobClient.fromConnectionString(connString) :
    JobClient.fromSharedAccessSignature(sas.toString());

  jobClient.scheduleFactoryReset(jobId, devices, function (err, job) {
    handleJobScheduled(err, job, 'Factory reset');
  });
}
else if (command === 'reboot')
{
  if (!arg1) inputError('No device IDs given');

  var jobId = uuid.v4();
  var devices = arrayFromCommaDelimitedList(arg1);

  var jobClient = connString ?
    JobClient.fromConnectionString(connString) :
    JobClient.fromSharedAccessSignature(sas.toString());

  jobClient.scheduleReboot(jobId, devices, function (err, job) {
    handleJobScheduled(err, job, 'Reboot');
  });
}
else {
  inputError('\'' + command + '\' is not a valid command');
  usage();
}

function handleJobScheduled(err, job, opName, onJobSucceeded) {
  if (err) serviceError(err);
  if (parsed.async) {
    printResult(job);
  }
  else {
    var interval = setInterval(function () {
      jobClient.getJob(jobId, function (err, job) {
        if (err) serviceError(err);
        if (job.status === 'completed') {
          clearInterval(interval);
          
          var status = {};
          job.statusMessage.split(';').forEach(function (elem) {
            if (elem.length) {
              var pair = elem.split(':');
              status[pair[0].trim()] = parseInt(pair[1]);
            }
          });
          if (!parsed.raw && status.Succeeded > 0) {
            console.log(colorsTmpl('\n' + '{green}' + opName + ' succeeded for ' + status.Succeeded + ' device' + (status.Succeeded > 1 ? 's' : '') + '{/green}'));
          }
          if (!parsed.raw && status.Failed > 0) {
            console.log(colorsTmpl('\n' + '{red}' + opName + ' failed for ' + status.Failed + ' device' + (status.Failed > 1 ? 's' : '') + '{/red}'));
          }
          if (onJobSucceeded && status.Succeeded > 0) {
            onJobSucceeded();
          }
        }
      });
    }, 2000);
  }
}

function arrayFromCommaDelimitedList(str) {
  return str.split(',').map(function (value) { return value.trim(); });
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
    console.log(colorsTmpl('\n{bold}{red}Error{/red}{/bold} ' + message));
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
    'SharedAccessKey=' + device.authentication.symmetricKey.primaryKey;
}

function printDevice(device) {
  var filtered = {};
  if (parsed.display) {
    var props = arrayFromCommaDelimitedList(parsed.display);
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

  printResult(result);
}

function printResult(job) {
  var output = parsed.raw ?
    JSON.stringify(job) :
    '\n' + prettyjson.render(job);
  console.log(output);
}

function usage() {
  console.log(colorsTmpl([
    '',
    '{yellow}Usage{/yellow}',
    '{magenta}iothub-explorer{/magenta} {green}<command>{/green} {white}<options> --raw{/white}',
    '  {grey}Add the{/grey} {white}--raw{/white} {grey}option to any command (except{/grey} {green}help{/green}{grey}) to minimize output and format results as JSON.{/grey}',
    '',
    '{yellow}Session commands{/yellow}',
    '  {green}login{/green} {white}<connection-string> [--duration=<num-seconds>]{/white}',
    '    {grey}Creates a session lasting <num-seconds>; commands during the session can omit <connection-string>',
    '    Default duration is 3600 (one hour).{/grey}',
    '  {green}logout{/green}',
    '    {grey}Cancels any session started by \'login\'{/grey}',
    '',
    '{yellow}Device twin commands{/yellow}',
    '  {white}[<connection-string>] {green}list{/green} [--display="<property>,..."] [--connection-string]{/white}',
    '    {grey}Displays a list of (at most 1000) device twins.',
    '    Can optionally reduce output to selected properties and/or connection strings.{/grey}',
    '  {white}[<connection-string>] {green}get{/green} <device-id> [--display="<property>,..."] [--connection-string]{/white}',
    '    {grey}Displays the given device twin.',
    '    Can optionally reduce output to selected properties and/or the connection string.{/grey}',
    '  {white}[<connection-string>] {green}create{/green} <device-id|device-json> [--display="<property>,..."] [--connection-string]{/white}',
    '    {grey}Creates a device twin on the IoT Hub. Also displays the twin as if you called{/grey} {green}get{/green}{grey}.',
    '    Can optionally reduce output to selected properties and/or the connection string.{/grey}',
    '  {white}[<connection-string>] {green}delete{/green} <device-id>{/white}',
    '    {grey}Deletes the device twin from the IoT Hub.{/grey}',
    '  {white}[<connection-string>] {green}registry-stats{/green}{/white}',
    '    {grey}Displays statistics about all devices registered with the IoT Hub.{/grey}',
    '',
    '  {grey}Use the{/grey} {white}--display{/white} {grey}option to show only the given properties from the azure-iothub.Device object.{/grey}',
    '  {grey}Use the{/grey} {white}--connection-string{/white} {grey}option to generate a connection string for the device(s).{/grey}',
    '',
    '{yellow}Device operation commands{/yellow}',
    '  {white}<connection-string> {green}monitor-events{/green} <device-id>{/white}',
    '    {grey}Monitors and displays the events received from a specific device.{/grey}',
    '  {white}[<connection-string>] {green}send{/green} <device-id> <msg> [--ack="none|positive|negative|full"]{/white}',
    '    {grey}Sends a cloud-to-device message to the given device, optionally with acknowledgment of receipt.{/grey}',
    '  {white}[<connection-string>] {green}receive{/green} [--messages=n]{/white}',
    '    {grey}Receives feedback about the delivery of cloud-to-device messages; optionally exits after receiving {white}n{/white} messages.{/grey}',
    '',
    '{yellow}Device management commands{/yellow}',
    '  {white}[<connection-string>] {green}get-job{/green} <job-id>{/white}',
    '    {grey}Displays information about the given job.{/grey}',
    '  {white}[<connection-string>] {green}read{/green} <device-ids> <device-properties> [--async]{/white}',
    '    {grey}Reads and displays properties, given as a comma-delimted list of names, from one or more devices (aka "deep read").{/grey}',
    '  {white}[<connection-string>] {green}write{/green} <device-ids> <device-properties> [--async]{/white}',
    '    {grey}Writes properties, given as a JSON object, to one or more devices (aka "deep write").{/grey}',
    '  {white}[<connection-string>] {green}firmware-update{/green} <device-ids> <firmware-uri> [--async] [--timeout=<num-minutes>]{/white}',
    '    {grey}Issues a command to one or more devices to update their firmware to the specified image.',
    '    The job will fail if it does not complete within the given timeout period; default timeout is one hour.{/grey}',
    '  {white}[<connection-string>] {green}factory-reset{/green} <device-ids> [--async]{/white}',
    '    {grey}Issues a command to one or more devices to reset their firmware to the factory default image.{/grey}',
    '  {white}[<connection-string>] {green}reboot{/green} <device-ids> [--async]{/white}',
    '    {grey}Issues a command to one or more devices to reboot.{/grey}',
    '',
    '  {white}<device-ids>{/white} {grey}is comma-delimited.{/grey}',
    '  {grey}Use the{/grey} {white}--async{/white} {grey}option to display the job ID and return immediately. The job ID can be given to{/grey} {green}get-job{/green}{grey}.{/grey}',
    '',
    '{yellow}Miscellaneous commands{/yellow}',
    '  {green}help{/green}',
    '    {grey}Displays this help message.{/grey}',
  ].join('\n')));
}