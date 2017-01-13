// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Registry = require('azure-iothub').Registry;
var chalk = require('chalk');

var device_teardown = function (hubConnectionString, provisionedDevices) {
  describe('Clean up e2e test', function () {
    provisionedDevices.forEach(function(deviceToDelete) {
      it('delete provisioned e2e device ' + deviceToDelete.deviceId, function (done) {
        this.timeout(120000);
        var registry = Registry.fromConnectionString(hubConnectionString);
        registry.delete(deviceToDelete.deviceId, function(err) {
          if (err) {
            console.log(chalk.red('Device ' + deviceToDelete.deviceId + ' could not be deleted: ' + err.constructor.name));
            done(err);
          } else {
            done();
          }
        });
      });
    });
  });
};
module.exports = device_teardown;