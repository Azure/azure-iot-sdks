'use strict';

var iotHubInfo = require('./config');
iotHubInfo.connectionString = 'HostName=' + iotHubInfo.hostName +';SharedAccessKeyName=' + iotHubInfo.sharedAccessKeyName + ';SharedAccessKey=' + iotHubInfo.sharedAccessKey;

const consoleLogLevel = "info";
const fileLogLevel = "debug";
const logFileName = "trace.log";
const testCommand = 'Terminate yourself';
const pingUrl = 'www.microsoft.com';

module.exports = {
  consoleLogLevel: consoleLogLevel,
  fileLogLevel: fileLogLevel,
  logFileName: logFileName,
  pingUrl: pingUrl,
  testCommand: testCommand,
  iotHubInfo: iotHubInfo
};
