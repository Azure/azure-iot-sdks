// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var serviceSdk = require('azure-iothub');
var serviceSas = require('azure-iothub').SharedAccessSignature;
var anHourFromNow = require('azure-iot-common').anHourFromNow;

var assert = require('chai').assert;

var runTests = function (hubConnectionString) {
  describe('Service client test:', function () {

    it('Service client can connect using sas', function(done) {
      this.timeout(60000);
      var connStr = serviceSdk.ConnectionString.parse(hubConnectionString);
      var sas = serviceSas.create(connStr.HostName, connStr.SharedAccessKeyName, connStr.SharedAccessKey, anHourFromNow()).toString();
      var serviceClient = serviceSdk.Client.fromSharedAccessSignature(sas);
      serviceClient.open(function(err, result) {
        if(err) {
          done(err);
        } else {
          assert.equal(result.constructor.name, 'Connected');
          serviceClient.close(function (err) {
            done(err);
          });
        }
      });
    });
  });
};

module.exports = runTests;