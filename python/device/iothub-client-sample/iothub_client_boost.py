# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

import random
from time import sleep

# for debugging in VS2015 it is convenient 
# to let the path point to the Debug build
# of the iothub import library
import sys
path = sys.path
sys.path += ["./Debug"]

from iothub import *

timeout = 241000
minimumPollingTime = 9
receiveContext = 0
avgWindSpeed = 10.0
message_count = 5

# String containing Hostname, Device Id & Device Key in the format:
# "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
connectionString = "[device connection string]"
msgTxt =  "{\"deviceId\": \"myFirstDevice\",\"windSpeed\": %.2f}"
Protocol = iothub_transport_provider.amqp

# IotHub error exception
class IotHubError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

def ReceiveMessageCallback( message, counter):
    buffer = message.GetByteArray()
    if buffer == None:
        print ("unable to retrieve the message data")
    else:
        size = len(buffer)
        print ("Received Message [%d] with Data: <<<%s>>> & Size=%d" % (counter, buffer[:size], size))
    mapProperties = message.Properties();
    propertyCount = 0
    if mapProperties != None:
        keyValuePair = mapProperties.GetInternals()
        propertyCount = len(keyValuePair)
        if (propertyCount > 0):
            print ("propertyCount: %d" % propertyCount)
            print (str(keyValuePair))
    counter += 1
    return iothubmessage_disposition_result.accepted

def SendConfirmationCallback( message, result, userContext):
    print ("Confirmation[%d] received for message with result = %d" % (userContext, result))
    return message.Destroy()

def iothub_client_sample_http_run():

    # iothub prep code 
    iotHubClient = IoTHubClient.CreateFromConnectionString(connectionString, Protocol)
    if iotHubClient == None:
       raise IotHubError("ERROR: iotHubClient is None!")

    if Protocol == iothub_transport_provider.http:
        result = iotHubClient.SetOption( "timeout", timeout)
        if result != iothub_client_result.ok:
            print "failure to set option \"timeout\""

        result = iotHubClient.SetOption( "MinimumPollingTime", minimumPollingTime)
        if result != iothub_client_result.ok:
            print "failure to set option \"MinimumPollingTime\""

    result = iotHubClient.SetMessageCallback( ReceiveMessageCallback, receiveContext);
    if result != iothub_client_result.ok:
        raise IotHubError("ERROR: SetMessageCallback..........FAILED!")

    # iothub send a few messages
    for i in xrange(0, message_count):
        msgTxtFormatted = msgTxt % (avgWindSpeed + (random.random() * 4 + 2))
        message = IoTHubMessage.CreateFromString(msgTxtFormatted)
        if message == None:
            raise IotHubError("ERROR: iotHubMessageHandle is NULL!");
        propMap = message.Properties()
        propText = "PropMsg_%d" % i
        result = propMap.AddOrUpdate( "PropName", propText)
        if result != map_result.ok:
            raise IotHubError("ERROR: AddOrUpdate Failed!");
        result = iotHubClient.SendEventAsync(message, SendConfirmationCallback, i)
        if result != iothub_client_result.ok:
            raise IotHubError("ERROR: SendEventAsync..........FAILED!")
        else:
            print ("IoTHubClient_SendEventAsync accepted message [%d] for transmission to IoT Hub." % i);

    # Wait for Commands 
    m = 0
    while m < 1000: # *0.1s = 1000s
        sleep(0.1)  # sleep 100ms
        # todo: workaround until C wrapper works with thread safe iothub APIs
        iotHubClient.DoWork();
        m = m + 1

    # done!
    iotHubClient.Destroy()

# default is AMQP, uncomment next line for HTTP
# Protocol = iothub_transport_provider.http

if Protocol == iothub_transport_provider.amqp:
    print "Starting the IoTHub client sample AMQP..."
else:
    print "Starting the IoTHub client sample HTTP..."

iothub_client_sample_http_run()

