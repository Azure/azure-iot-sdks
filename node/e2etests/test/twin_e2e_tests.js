// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Registry = require('azure-iothub').Registry;
var ConnectionString = require('azure-iothub').ConnectionString;
var deviceSdk = require('azure-iot-device');
var deviceMqtt = require('azure-iot-device-mqtt').Mqtt;
var uuid = require('uuid');
var _ = require('lodash');
var assert = require('chai').assert;
var async = require('async');

var newProps = {
  foo : 1,
  bar : {
    baz : 2,
    tweedle : {
      dee : 3
    } 
  }
};

var moreNewProps = {
  bar : {
    baz : 3
  }
};

var mergeResult =  _.merge(JSON.parse(JSON.stringify(newProps)), moreNewProps);

var nullIndividualProps = {
  bar : {
    tweedle: null
  }
};

var nullMergeResult = JSON.parse(JSON.stringify(newProps));
delete nullMergeResult.tweedle; 

var runTests = function (hubConnectionString) {
  describe('Twin', function() {

    this.timeout(10000);
    var deviceClient, deviceTwin;
    var serviceTwin;

    var deviceDescription;

    beforeEach(function (done) {
      this.timeout(10000);

      var host = ConnectionString.parse(hubConnectionString).HostName;
      var pkey = new Buffer(uuid.v4()).toString('base64');
      var deviceId = '0000e2etest-delete-me-twin-e2e-' + uuid.v4();
      
      deviceDescription = {
        deviceId:  deviceId,
        status: 'enabled',
          authentication: {
          symmetricKey: {
            primaryKey: pkey,
            secondaryKey: new Buffer(uuid.v4()).toString('base64')
          }
        },
        connectionString: 'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=' + pkey
      };

      var registry = Registry.fromConnectionString(hubConnectionString);

      registry.create(deviceDescription, function (err) {
        if (err) return done(err);

        deviceClient = deviceSdk.Client.fromConnectionString(deviceDescription.connectionString, deviceMqtt);

        deviceClient.open(function(err) {
          if (err) return done(err);
          deviceClient.getTwin(function(err, twin) {
            if (err) return done(err);
            deviceTwin = twin;

            registry.getTwin(deviceDescription.deviceId, function(err, twin) {
              if (err) return done(err);
              serviceTwin = twin;
              done();
            });
          });
        });
      });
    });

    afterEach(function (done) {
      this.timeout(10000);
      if (deviceClient) {
        deviceClient.close(function(err) {
          if (err) return done(err);
            
          var registry = Registry.fromConnectionString(hubConnectionString);
          registry.delete(deviceDescription.deviceId, function(err) {
            if (err) return done(err);
            done();
          });
        });
      } else {
        done();
      }
    });

    var assertObjectIsEmpty = function(props) {
      _.every(_.keys(props), function(key) {
        if (typeof props[key] !== 'function') {
          assert(key.startsWith('$'), 'key ' + key + ' should not be in empty twin');
        }
      });
    };

    var assertObjectsAreEqual = function(left, right) {
      var compare = function(left, right) {
        _.every(_.keys(right), function(key) {
          if (typeof right[key] !== 'function' && !key.startsWith('$')) {
            assert.equal(left[key], right[key], 'key ' + key + ' not matched between service and device'); 
          }
        });
      };
      compare(left, right);
      compare(right, left);
    };

    it('relies on $version starting at 1 and incrementing by 1 each time', function(done) {
      assert.equal(deviceTwin.properties.desired.$version,1);
      serviceTwin.update( { properties : { desired : newProps } }, function(err) {
        if (err) return done(err);
        deviceTwin.on('properties.desired', function() {
          if (deviceTwin.properties.desired.$version === 1) {
            // ignore $update === 1.  assert needed to make jshint happy
            assert(true);
          } else if (deviceTwin.properties.desired.$version === 2) {
            done();
          } else  {
            done(new Error('incorrect property version received - ' + deviceTwin.properties.desired.$version));
          }
        });
      });
    });

    var sendsAndReceiveReportedProperties  = function(done) {
      assertObjectIsEmpty(serviceTwin.properties.reported);
      assertObjectIsEmpty(deviceTwin.properties.reported);

      deviceTwin.properties.reported.update(newProps, function(err) {
        if (err) return done(err);
        serviceTwin.get(function(err) {
          if (err) return done(err);
          assertObjectsAreEqual(newProps, serviceTwin.properties.reported);
          done();
        });
      });
    };

    it('sends and receives reported properties', sendsAndReceiveReportedProperties);

    var mergeReportedProperties =  function(first, second, result, done) {
      deviceTwin.properties.reported.update(first, function(err) {
        if (err) return done(err);
        deviceTwin.properties.reported.update(second, function(err) {
          if (err) return done(err);
          serviceTwin.get(function(err) {
            if (err) return done(err);
            assertObjectsAreEqual(serviceTwin.properties.reported, result);
            done();
          });
        });
      });
    };

    it('sends and receives merged reported properties', function(done) {
      mergeReportedProperties(newProps, moreNewProps, mergeResult, done);
    });

    var sendsAndReceivesDesiredProperties = function(done) {
      assertObjectIsEmpty(deviceTwin.properties.desired);

      serviceTwin.update( { properties : { desired : newProps } }, function(err) {
        if (err) return done(err);

        deviceTwin.on('properties.desired', function(props) {
          if (props.$version === 1) {
            // ignore
            assert(true);
          } else if (props.$version === 2) {
            assertObjectsAreEqual(newProps, deviceTwin.properties.desired);
            done();
          } else {
            done(new Error('incorrect property version received - ' + props.$version));
          }
        });
      });
    };

    it('sends and receives desired properties', sendsAndReceivesDesiredProperties);
      
    var mergeDesiredProperties = function(first, second, newEtag, result, done) {
      serviceTwin.update( { properties : { desired : first } }, function(err) {
        if (err) return done(err);

        if (newEtag) {
          assert.isDefined(serviceTwin.etag);
          assert.notEqual(serviceTwin.etag, "*");
          serviceTwin.etag = newEtag;
        }

        serviceTwin.update( { properties : { desired : second } }, function(err) {
          if (err) return done(err);

          deviceTwin.on('properties.desired', function(props) {
            if (props.$verion === 1 || props.$version === 2) {
              // ignore
              assert(true);
            } else if (props.$version === 3) {
              assertObjectsAreEqual(deviceTwin.properties.desired, result);
              done();
            } else {
              done(new Error('incorrect property version received - ' + props.$version));
            }
          });
        });
      });
    };

    it('sends and receives merged desired properties', function(done) {
      mergeDesiredProperties(newProps, moreNewProps, null, mergeResult, done);
    });

    it('sends and receives merged desired properties using etag *', function(done) {
      mergeDesiredProperties(newProps, moreNewProps, "*", mergeResult, done);
    });

    it('can get and set tags', function(done) {
      assertObjectIsEmpty(serviceTwin.tags);

      serviceTwin.update( { tags : newProps }, function(err) {
        if (err) return done(err);

        assertObjectsAreEqual(newProps, serviceTwin.tags);
        done();
      });
    });

    var mergeTags =  function(first, second, newEtag, result, done) {
      assertObjectIsEmpty(serviceTwin.tags);

      serviceTwin.update( { tags : first }, function(err) {
        if (err) return done(err);

        if (newEtag) {
          assert.isDefined(serviceTwin.etag);
          assert.notEqual(serviceTwin.etag, "*");
          serviceTwin.etag = newEtag;
        }

        serviceTwin.update( { tags: second }, function(err) {
          if (err) return done(err);
          assertObjectsAreEqual(serviceTwin.tags, result);
          done();
        });
      });
    };

    it('can merge tags', function(done) {
      mergeTags(newProps, moreNewProps, null, mergeResult, done);
    });

    it('can merge tags using etag *', function(done) {
      mergeTags(newProps, moreNewProps, "*", mergeResult, done);
    });

    it('can send reported properties to the service after renewing the sas token', function(done) {
      deviceClient.on('_sharedAccessSignatureUpdated', function() {
        // service bug -- delay before sending properties.  Will be fixed as part of some "clock skew" fix.
        setTimeout(function() {
          sendsAndReceiveReportedProperties(done);
        },5000);
      });
      deviceClient._renewSharedAccessSignature();
    });

    it('can receive desired properties from the service after renewing the sas token', function(done) {
      deviceClient.on('_sharedAccessSignatureUpdated', function() {
        sendsAndReceivesDesiredProperties(done);
      });
      deviceClient._renewSharedAccessSignature();
    });

    it.skip('call null out all reported properties', function(done) {
      mergeReportedProperties(newProps, null, {}, done);
    });

    it('can null out individual reported properties', function(done) {
      mergeReportedProperties(newProps, nullIndividualProps, nullMergeResult, done);
    });

    it('can null out all desired properties', function(done) {
      mergeDesiredProperties(newProps, null, null, {}, done);
    });

    it('can null out individual desired properties', function(done) {
      mergeDesiredProperties(newProps, nullIndividualProps, null, nullMergeResult, done);
    });

    it('can null out all desired properties with etag *', function(done) {
      mergeDesiredProperties(newProps, null, "*", {}, done);
    });

    it('can null out individual desired properties with etag *', function(done) {
      mergeDesiredProperties(newProps, nullIndividualProps, "*", nullMergeResult, done);
    });

    it('can null out all tags', function(done) {
      mergeTags(newProps, null, null, {}, done);
    });

    it('can null out individual tags', function(done) {
      mergeTags(newProps, nullIndividualProps, null, nullMergeResult, done);
    });

    it('can null out all tags with etag *', function(done) {
      mergeTags(newProps, null, "*", {}, done);
    });

    it('can null out individual tags with etag *', function(done) {
      mergeTags(newProps, nullIndividualProps, "*", nullMergeResult, done);
    });

    it('can renew SAS 20 times without failure', function(done) 
    {
      this.timeout(60000);
      var iteration = 0;
      var doItAgain = function() {
        iteration++;
        if (iteration === 20) {
          done();
        } else {
          deviceClient._renewSharedAccessSignature();
          setTimeout(doItAgain, 1000);
        }
      };

      doItAgain();
    });

    it('can set desired properties while the client is disconnected', function(done) {
      async.series([
        function setDesiredProperties(callback) {
          serviceTwin.update( { properties : { desired : newProps } }, callback);
        },
        function closeDeviceClient(callback) {
          deviceClient.close(callback);
        },
        function setDesiredPropertiesAgain(callback) {
          serviceTwin.update( { properties : { desired : moreNewProps } }, callback);
        },
        function openDeviceClientAgain(callback) {
          deviceClient = deviceSdk.Client.fromConnectionString(deviceDescription.connectionString, deviceMqtt);
          deviceClient.open(callback);
        },
        function getDeviceTwin(callback) {
          deviceClient.getTwin(function(err, twin) {
              if (err) return callback(err);
              deviceTwin = twin;
              callback();
          });
        },
        function validateProperties(callback) {
            assertObjectsAreEqual(deviceTwin.properties.desired, mergeResult);
            callback();
        }
      ], done);
      
    });

  });
};

module.exports = runTests;
