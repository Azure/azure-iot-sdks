# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

import random
from time import sleep
from ctypes import *

timeout = c_uint(241000)
minimumPollingTime = c_uint(9)
receiveContext = c_uint(0)
avgWindSpeed = 10.0
message_count = 5
messageHandle = [c_void_p] * message_count 

IOTHUBCALLBACK = CFUNCTYPE(c_int, c_void_p, POINTER(c_int))
SENDCALLBACK = CFUNCTYPE(c_int, c_uint, c_uint)

# String containing Hostname, Device Id & Device Key in the format:
# "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
connectionString = "[device connection string]"
msgTxt =  "{\"deviceId\": \"myFirstDevice\",\"windSpeed\": %.2f}"

# IotHub error exception
class IotHubError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

# load dll (copy libraries to the same directory as this sample)
iothub = cdll.LoadLibrary("iothub_client_python")

# create objects for calls in IotHub dll
IOTHUB_CLIENT_OK = 0
IOTHUB_MESSAGE_OK = 0
IOTHUBMESSAGE_ACCEPTED = 0
MAP_OK = 0
NULL = 0
HTTP_Protocol = iothub.HTTP_Protocol
AMQP_Protocol = iothub.AMQP_Protocol
MQTT_Protocol = iothub.MQTT_Protocol 
Protocol = AMQP_Protocol
IoTHubClient_LL_CreateFromConnectionString = iothub.IoTHubClient_LL_CreateFromConnectionString
IoTHubClient_LL_CreateFromConnectionString.ResType = c_void_p
IoTHubClient_LL_SetOption = iothub.IoTHubClient_LL_SetOption
IoTHubClient_LL_SetOption.ResType = c_uint
IoTHubClient_LL_DoWork = iothub.IoTHubClient_LL_DoWork
IoTHubClient_LL_Destroy = iothub.IoTHubClient_LL_Destroy
IoTHubClient_LL_Destroy.ResType = c_uint
IoTHubClient_LL_SetMessageCallback = iothub.IoTHubClient_LL_SetMessageCallback
IoTHubClient_LL_SetMessageCallback.ResType = c_uint
IoTHubMessage_CreateFromByteArray = iothub.IoTHubMessage_CreateFromByteArray
IoTHubMessage_CreateFromByteArray.ResType = c_void_p
IoTHubMessage_CreateFromString = iothub.IoTHubMessage_CreateFromString
IoTHubMessage_CreateFromString.ResType = c_void_p
IoTHubMessage_GetByteArray = iothub.IoTHubMessage_GetByteArray
IoTHubMessage_GetByteArray.ResType = c_void_p
IoTHubMessage_Properties = iothub.IoTHubMessage_Properties
IoTHubMessage_Properties.ResType = c_void_p
IoTHubClient_LL_SendEventAsync = iothub.IoTHubClient_LL_SendEventAsync
IoTHubClient_LL_SendEventAsync.ResType = c_uint
IoTHubMessage_Destroy = iothub.IoTHubMessage_Destroy
IoTHubMessage_Destroy.ResType = c_uint
Map_AddOrUpdate = iothub.Map_AddOrUpdate
Map_AddOrUpdate.ResType = c_uint
Map_GetInternals = iothub.Map_GetInternals
Map_GetInternals.ResType = c_uint

# helper class to access C char**
class c_char_p_array(object):
    def __init__(self, size):
        self.data = [size]
        self.size = size
    def from_address(self, address):
        for i in xrange(0,self.size):
            self.data[i] = c_char_p.from_address(address+i*sizeof(c_char_p))
        return self
    def __getitem__(self, index):
        return self.data[index]

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
    message_callback = cast(IOTHUBCALLBACK(ReceiveMessageCallback), c_void_p)
    send_callback = cast(SENDCALLBACK(SendConfirmationCallback), c_void_p)

    # iothub prep code 
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(c_char_p(connectionString), Protocol)
    if iotHubClientHandle==NULL:
        raise IotHubError("ERROR: iotHubClientHandle is NULL!")

    if Protocol == HTTP_Protocol:
        result = IoTHubClient_LL_SetOption( iotHubClientHandle, c_char_p("timeout"), byref(timeout))
        if result != IOTHUB_CLIENT_OK:
            print "failure to set option \"timeout\""

        result = IoTHubClient_LL_SetOption( iotHubClientHandle, c_char_p("MinimumPollingTime"), byref(minimumPollingTime))
        if result != IOTHUB_CLIENT_OK:
            print "failure to set option \"MinimumPollingTime\""

    result = IoTHubClient_LL_SetMessageCallback( iotHubClientHandle, message_callback, byref(receiveContext));
    if result != IOTHUB_CLIENT_OK:
        raise IotHubError("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!")

    # iothub send a few messages
    for i in xrange(0, message_count):
        msgTxtFormatted = msgTxt % (avgWindSpeed + (random.random() * 4 + 2))
        messageHandle[i] = IoTHubMessage_CreateFromString(c_char_p(msgTxtFormatted))
        if messageHandle[i] == NULL:
            raise IotHubError("ERROR: iotHubMessageHandle is NULL!");
        propMap = IoTHubMessage_Properties(messageHandle[i])
        propText = "PropMsg_%d" % i
        result = Map_AddOrUpdate( propMap, c_char_p("PropName"), c_char_p(propText))
        if result != MAP_OK:
            raise IotHubError("ERROR: Map_AddOrUpdate Failed!");
        result = IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle[i], send_callback, i)
        if result != IOTHUB_CLIENT_OK:
            raise IotHubError("ERROR: IoTHubClient_LL_SendEventAsync..........FAILED!")
        else:
            print ("IoTHubClient_LL_SendEventAsync accepted message [%d] for transmission to IoT Hub." % i);

    # Wait for Commands 
    m = 0
    while m < 10000: # *0.1s = 1000s
        IoTHubClient_LL_DoWork(iotHubClientHandle)
        sleep(0.1) # sleep 100ms
        m = m + 1

    # done!
    IoTHubClient_LL_Destroy(iotHubClientHandle)

# default is AMQP, uncomment next line for HTTP
# Protocol = HTTP_Protocol

if Protocol == AMQP_Protocol:
    print "Starting the IoTHub client sample AMQP..."
else:
    print "Starting the IoTHub client sample HTTP..."

iothub_client_sample_http_run()

