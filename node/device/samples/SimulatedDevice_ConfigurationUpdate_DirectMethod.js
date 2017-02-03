'use strict';
var Client = require('azure-iot-device').Client;
var Protocol = require('azure-iot-device-mqtt').Mqtt;

var connectionString = process.argv[2];
var client = Client.fromConnectionString(connectionString, Protocol);
var ConnectionString = require('azure-iot-device').ConnectionString;
var Message = require('azure-iot-device').Message;
var deviceId = ConnectionString.parse(connectionString).DeviceId;

// Sensors data
var temperature = 50;
var humidity = 50;
var currentTime = new Date().toISOString();
var sendFrequency = 4000;

// Helper function to generate random number between min and max
function generateRandomIncrement() {
  return ((Math.random() * 2) - 1);
}

// Helper function to print results for an operation
function printErrorFor(op) {
  return function printError(err) {
    if (err) console.log(op + ' error: ' + err.toString());
  };
}

// Send device meta data
var deviceMetaData = {
  'IotHub' : {
    'ObjectType': 'DeviceInfo',
    'IsSimulatedDevice': 1,
    'Version': '1.0',
    'DeviceMetadata': {
      'DeviceID': deviceId,
      'HubEnabledState': 1,
      'DeviceState': 'normal',
      'UpdatedTime': null,
      'Manufacturer': 'Contoso',
      'ModelNumber': 'DX 430',
      'SerialNumber': '1234',
      'FirmwareVersion': '1.0',
      'Platform': 'node.js',
      'Processor': 'ARM',
      'InstalledRAM': '64MB',
      'Latitude': 47.617025,
      'Longitude': -122.191285
    },
    'Commands': {
      'SetTemperature' : {
        'Parameters' : {
          'Temperature' : {
            'Type' : 'double'
          }
        }
      },
      'SetHumidity' : {
        'Parameters' : {
          'Humidity' : {
            'Type' : 'double'
          }
        }
      }
    }  
  }
};

var sendIntervalId = null;

// Send telemetry data
var SendTelemetry = function(client) {
    temperature += generateRandomIncrement();
    humidity += generateRandomIncrement();
    currentTime = new Date().toISOString();

    var data = JSON.stringify({
      'DeviceID': deviceId,
      'Temperature': temperature,
      'Humidity': humidity,
      'time': currentTime,
    });

    console.log('Sending device event data:\n' + data);
    client.sendEvent(new Message(data), printErrorFor('send event'));
}

// Open connection to get twin, send device metadata
client.open(function(err) {
    if (err) {
        console.error('could not open IotHub client');
    } else {

      // Creates a Twin object and establishes a connection with the Twin service
      client.getTwin(function(err, twin) {
        if (err) {
          console.error('could not get twin - ' + err.message);
        } else {
          console.log('retrieved device twin');
          
          // Send device metadata     
          console.log('Setting device metadata on device twin:\n' + JSON.stringify(deviceMetaData));
          twin.properties.reported.update(deviceMetaData, function(err){
            if (err) { 
              console.error('Error updating twin');
            } 
          })
          
          // Register callback for when new sendFrequency is changed
          twin.on('properties.desired.telemetryConfig.sendFrequency', function(newSendFrequency) {
            console.log("Received new sendFrequency: "+ newSendFrequency);
            sendFrequency = newSendFrequency;
            if (sendIntervalId != null) {
              clearInterval(sendIntervalId);
              sendIntervalId = setInterval( function() { 
                SendTelemetry(client);
              }, sendFrequency);
            }
              
            var patch = {
              telemetryConfig: {
                sendFrequency : newSendFrequency
              }
            };
            
            // Update twin with this new sendFrequency patch
            twin.properties.reported.update(patch, function(err) {
                if (err) {
                    console.log('Could not report properties');
                } 
            });
              
          });
      }       
    });
    
    client.on('message', function (msg) {
      console.log('receive data: ' + msg.getData());

      try {
        var command = JSON.parse(msg.getData());
        if (command.Name === 'SetTemperature') {
          temperature = command.Parameters[0].Value;
          console.log('New temperature set to :' + temperature + 'F');
        }

        client.complete(msg, printErrorFor('complete'));
      }
      catch (err) {
        printErrorFor('parse received message')(err);
        client.reject(msg, printErrorFor('reject'));
      }
    });


    // Start event data send routing
    sendIntervalId = setInterval( function() { 
      SendTelemetry(client);
    }, sendFrequency);

    // Reboot method
    client.onDeviceMethod('reboot', function onReboot(request, response) {
      response.send(200, 'Reboot started', function(err) {
        if (err) {
          console.error('An error occured when sending a method response:\n' + err.toString());
        } else {
          console.log('Response to method \'' + request.methodName + '\' sent successfully.');
        }
      });
        
      // Get device Twin
      client.getTwin(function(err, twin) {
        if (err) {
          console.error('could not get twin - ' + err.message);
        } else {
          console.log('twin acquired');
          
          // Update the reported properties for this device through the 
          // twin.  This enables the back end app to query for all device that
          // have completed a reboot based on the lastReboot property.
          twin.properties.reported.update({
            iothubDM : {
             reboot : {
                startedRebootTime : new Date().toISOString(),
              }
            }
          }, function(err) {
            if (err) { 
              console.error('Error updating twin');
            } else {
              console.log('Device reboot twin state reported')
            }
          });  
        }
      });
      
      // Add your device's reboot API for physical restart.
      console.log('Rebooting!');
      process.exit(1);
    });
      
    console.log('Client connected to IoT Hub.  Waiting for reboot device method.');
  }
});
