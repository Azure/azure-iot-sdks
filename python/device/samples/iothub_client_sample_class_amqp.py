#!/usr/bin/env python

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import random, time, sys
import iothub_client
from iothub_client import *

receiveContext = 0
avgWindSpeed = 20.0
message_count = 5
received_count = 0

# global counters
receive_callbacks = 0
send_callbacks = 0

# String containing Hostname, Device Id & Device Key in the format:
# "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
connectionString = "[device connection string]"

msgTxt = "{\"deviceId\": \"myAMQPDevice\",\"windSpeed\": %.2f}"

class HubManager(object):
    def __init__(self, connection_string, protocol=IoTHubTransportProvider.AMQP):
        self.client_protocol = protocol
        self.client = IoTHubClient(connection_string, protocol)
        self.client.set_message_callback(self._receive_message_callback, receiveContext)

    def _receive_message_callback(self, message, counter):
        global receive_callbacks
        buffer = message.get_bytearray()
        size = len(buffer)
        print ("Received Message [%d]:" % counter)
        print ("    Data: <<<%s>>> & Size=%d" % (buffer[:size].decode('utf-8') , size))
        mapProperties = message.properties()
        keyValuePair = mapProperties.get_internals()
        print ("    Properties: %s" % keyValuePair)
        counter += 1
        receive_callbacks += 1
        print ("    Total calls received: %d" % receive_callbacks)
        return IoTHubMessageDispositionResult.ACCEPTED


    def _send_confirmation_callback(self, message, result, userContext):
        global send_callbacks
        print ("Confirmation[%d] received for message with result = %s" % (userContext, result))
        mapProperties = message.properties()
        keyValuePair = mapProperties.get_internals()
        print ("    Properties: %s" % keyValuePair)
        send_callbacks += 1
        print ("    Total calls confirmed: %d" % send_callbacks)
        
    def send_event(self, event, properties, send_context):
        if not isinstance(event, IoTHubMessage):
            event = IoTHubMessage(bytearray(event, 'utf8'))
        
        if len(properties) > 0:
            propMap = event.properties()
            for key in properties:
                propMap.add(key, properties[key])
                
        self.client.send_event_async(event, self._send_confirmation_callback, send_context)
        

def main():
    try:
        print ("\nPython %s\n" % sys.version)
        print ("IoT Hub for Python SDK Version: %s\n" % iothub_client.__version__)
    
        hub_manager = HubManager(connectionString)
        
        print ("Starting the IoT Hub Python sample using protocol %s..." % hub_manager.client_protocol)
        
        while True:
            # send a few messages every minute
            print ("IoTHubClient sending %d messages" % message_count)

            for i in range(0, message_count):
                msgTxtFormatted = msgTxt % (
                    avgWindSpeed + (random.random() * 4 + 2))
                msg_properties = {
                    "Property": "PropMsg_%d" % i 
                }
                hub_manager.send_event(msgTxtFormatted, msg_properties, i)
                print ("IoTHubClient.send_event_async accepted message [%d] for transmission to IoT Hub." % i)

            # Wait for Commands or exit
            print ("IoTHubClient waiting for commands, press Ctrl-C to exit")

            n = 0
            while n < 6:
                status = hub_manager.client.get_send_status()
                print ("Send status: %s" % status)
                time.sleep(10)
                n += 1
    except IoTHubError as e:
       print ("Unexpected error %s from IoTHub" % e)
       return
    except KeyboardInterrupt:
        print ("IoTHubClient sample stopped")


if __name__ == '__main__':
    main()
