# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

import random
import os
from time import sleep
import sys

path = sys.path
mods = sys.modules
sys.path += ["./Debug"]

from iothub import *

timeout = 241000
minimumPollingTime = 9
receiveContext = 0
avgWindSpeed = 10.0
message_count = 5
#messageHandle = [c_void_p] * message_count 

connectionString = "HostName=mregen-python-ne-iothub.azure-devices.net;DeviceId=test;SharedAccessKey=/TKAG9mKQvzcKYS23QKDnp9L0a2ptBPQ0Da6CVGN8h0="
msgTxt =  "{\"deviceId\": \"myFirstDevice\",\"windSpeed\": %.2f}"
Protocol = transport_provider.amqp

# IotHub error exception
class IotHubError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

# create objects for calls in IotHub dll
IOTHUB_CLIENT_OK = 0
IOTHUB_MESSAGE_OK = 0
IOTHUBMESSAGE_ACCEPTED = 0
MAP_OK = 0
NULL = 0

def ReceiveMessageCallback( message, counter):
    size = c_int(0)
    buffer = c_char_p()
    result = IoTHubMessage_GetByteArray(message, byref(buffer), byref(size))
    if result != IOTHUB_MESSAGE_OK:
        print ("unable to retrieve the message data")
    else:
        print ("Received Message [%d] with Data: <<<%s>>> & Size=%d" % (counter.contents.value, buffer.value[:size.value], size.value))
    mapProperties = IoTHubMessage_Properties(message);
    propertyCount = c_int(0)
    if mapProperties != NULL:
        keys = c_void_p()
        values = c_void_p()
        result = Map_GetInternals(mapProperties, byref(keys), byref(values), byref(propertyCount))
        if (result == MAP_OK) & (propertyCount.value > 0):
            print ("propertyCount: %d" % propertyCount.value)
            keyArray = c_char_p_array(propertyCount.value).from_address(keys.value)
            valArray = c_char_p_array(propertyCount.value).from_address(values.value)
            for i in xrange(0,propertyCount.value):
                print ("(%s,%s)" % (keyArray[i].value, valArray[i].value))
    counter.contents.value += 1
    return IOTHUBMESSAGE_ACCEPTED

def SendConfirmationCallback( result, messageId):
    print ("Confirmation received for message tracking id = %d with result = %d" % (messageId, result))
    return IoTHubMessage_Destroy(messageHandle[messageId]);

def iothub_client_sample_http_run():

    # callbacks for c dll
    #message_callback = cast(IOTHUBCALLBACK(ReceiveMessageCallback), c_void_p)
    #send_callback = cast(SENDCALLBACK(SendConfirmationCallback), c_void_p)

    # iothub prep code 
    iotHubClient = IoTHubClient.CreateFromConnectionString(connectionString, Protocol)
    if iotHubClient==None:
       raise IotHubError("ERROR: iotHubClient is None!")

    if Protocol == transport_provider.http:
        result = iotHubClient.SetOption( "timeout", timeout)
        if result != iothub_client_result.ok:
            print "failure to set option \"timeout\""

        result = iotHubClient.SetOption( "MinimumPollingTime", minimumPollingTime)
        if result != iothub_client_result.ok:
            print "failure to set option \"MinimumPollingTime\""

    #result = iotHubClient.SetMessageCallback( message_callback, receiveContext);
    #if result != IOTHUB_CLIENT_OK:
    #    raise IotHubError("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!")

    # iothub send a few messages
    for i in xrange(0, 0):#message_count):
        msgTxtFormatted = msgTxt % (avgWindSpeed + (random.random() * 4 + 2))
        messageHandle[i] = IoTHubMessage_CreateFromString(c_char_p(msgTxtFormatted))
        if messageHandle[i] == NULL:
            raise IotHubError("ERROR: iotHubMessageHandle is NULL!");
        propMap = IoTHubMessage_Properties(messageHandle[i])
        propText = "PropMsg_%d" % i
        result = Map_AddOrUpdate( propMap, c_char_p("PropName"), c_char_p(propText))
        if result != MAP_OK:
            raise IotHubError("ERROR: Map_AddOrUpdate Failed!");
        result = iotHubClient.SendEventAsync(messageHandle[i], send_callback, i)
        if result != IOTHUB_CLIENT_OK:
            raise IotHubError("ERROR: IoTHubClient_LL_SendEventAsync..........FAILED!")
        else:
            print ("IoTHubClient_LL_SendEventAsync accepted message [%d] for transmission to IoT Hub." % i);

    #result = iotHubClient.GetSendStatus()
    # Wait for Commands 
    m = 0
    while m < 100: # *0.1s = 1000s
        #IoTHubClient_LL_DoWork(iotHubClientHandle)
        sleep(0.1) # sleep 100ms
        m = m + 1

    # done!
    iotHubClient.Destroy()

# default is AMQP, uncomment next line for HTTP
Protocol = transport_provider.http

if Protocol == transport_provider.amqp:
    print "Starting the IoTHub client sample AMQP..."
else:
    print "Starting the IoTHub client sample HTTP..."

iothub_client_sample_http_run()

