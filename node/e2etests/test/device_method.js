// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Registry = require('azure-iothub').Registry;
var ServiceClient = require('azure-iothub').Client;
var ConnectionString = require('azure-iothub').ConnectionString;
var deviceSdk = require('azure-iot-device');
var util = require('util');
var deviceMqtt = require('azure-iot-device-mqtt').Mqtt;
var uuid = require('uuid');
var assert = require('chai').assert;

module.exports = function(hubConnectionString) {
  describe('Device Method', function() {
    this.timeout(120000);
    var deviceClient;
    var deviceDescription = {
        deviceId:  '0000e2etest-delete-me-node-device-method-' + uuid.v4(),
        status: 'enabled',
          authentication: {
          symmetricKey: {
            primaryKey: new Buffer(uuid.v4()).toString('base64'),
            secondaryKey: new Buffer(uuid.v4()).toString('base64')
          }
        }
      };

    // create a new device for every test
    beforeEach(function (done) {
      this.timeout(20000);

      var registry = Registry.fromConnectionString(hubConnectionString);
      registry.create(deviceDescription, function (err) {
        if (err) return done(err);

        var host = ConnectionString.parse(hubConnectionString).HostName;
        deviceClient = deviceSdk.Client.fromConnectionString(
          util.format('HostName=%s;DeviceId=%s;SharedAccessKey=%s',
            host, deviceDescription.deviceId,
            deviceDescription.authentication.symmetricKey.primaryKey
          ),
          deviceMqtt
        );
        deviceClient.open(done);
      });
    });

    // nuke the test device after every test
    afterEach(function (done) {
      this.timeout(20000);
      if (!!deviceClient) {
        deviceClient.close(function(err) {
          if (!!err) {
            console.warn('Could not close connection to device ' +
              deviceDescription.deviceId + '. Deleting device anyway.'
            );
          }

          Registry
            .fromConnectionString(hubConnectionString)
            .delete(deviceDescription.deviceId, done);
        });
      } else {
        done();
      }
    });

    it('makes and receives a method call', function(done) {
      var methodName = 'method1';
      var methodResult = 200;
      var methodPayload = {
        'k1': 'v1',
        'k2': {
          'k3': 'v3'
        }
      };
      var responsePayload = {
        'k4': 'v4',
        'k5': {
          'k6': 'v6'
        }
      };

      // setup device to handle the method call
      deviceClient.onDeviceMethod(methodName, function(request, response) {
        // validate request
        assert.isNotNull(request);
        assert.strictEqual(request.methodName, methodName);
        assert.deepEqual(JSON.parse(request.body), methodPayload);

        // validate response
        assert.isNotNull(response);

        // send the response
        response.write(JSON.stringify(responsePayload));
        response.end(methodResult, function(err) {
          if(!!err) {
            console.error('An error ocurred when sending a method response:\n' +
                err.toString());
          }
        });
      });

      // make the method call via the service
      var methodParams = {
        methodName: methodName, 
        payload: methodPayload,
        timeoutInSeconds: 20
      };
      ServiceClient
          .fromConnectionString(hubConnectionString)
          .invokeDeviceMethod(
              deviceDescription.deviceId,
              methodParams,
              function(err, result) {
                if(!err) {
                  assert.strictEqual(result.status, methodResult);
                  assert.deepEqual(result.payload, responsePayload);
                }
                done(err);
              });
    });
  });
};