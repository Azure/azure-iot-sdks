#!/usr/bin/env python

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import random
import time
import sys
import iothub_client
from iothub_client import *

receiveContext = 0
avgWindSpeed = 10.0
message_count = 10
received_count = 0

# global counters
receive_callbacks = 0
send_callbacks = 0

# transport protocol
Protocol = IoTHubTransportProvider.MQTT

# String containing Hostname, Device Id & Device Key in the format:
# "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
connectionString = "[device connection string]"

msgTxt = "{\"deviceId\": \"myMQTTDevice\",\"windSpeed\": %.2f}"


# some embedded platforms need certificate information
def set_certificates(iotHubClient):
    from iothub_client_cert import certificates
    try:
        iotHubClient.set_option("TrustedCerts", certificates)
        print ("set_option TrustedCerts successful")
    except IoTHubClientError as e:
        print ("set_option TrustedCerts failed (%s)" % e)


def receive_message_callback(message, counter):
    global receive_callbacks
    buffer = message.get_bytearray()
    size = len(buffer)
    print ("Received Message [%d]:" % counter)
    print ("    Data: <<<%s>>> & Size=%d" %
           (buffer[:size].decode('utf-8'), size))
    mapProperties = message.properties()
    keyValuePair = mapProperties.get_internals()
    print ("    Properties: %s" % keyValuePair)
    counter += 1
    receive_callbacks += 1
    print ("    Total calls received: %d" % receive_callbacks)
    return IoTHubMessageDispositionResult.ACCEPTED


def send_confirmation_callback(message, result, userContext):
    global send_callbacks
    print (
        "Confirmation[%d] received for message with result = %s" %
        (userContext, result))
    mapProperties = message.properties()
    keyValuePair = mapProperties.get_internals()
    print ("    Properties: %s" % keyValuePair)
    send_callbacks += 1
    print ("    Total calls confirmed: %d" % send_callbacks)


def iothub_client_init():
    # prepare iothub client
    iotHubClient = IoTHubClient(connectionString, Protocol)
    # some embedded platforms need certificate information
    # set_certificates(iotHubClient)
    # to enable MQTT logging
    # iotHubClient.set_option("logtrace", 1);
    iotHubClient.set_message_callback(receive_message_callback, receiveContext)
    return iotHubClient


def print_last_message_time(iotHubClient):
    try:
        lastMessage = iotHubClient.get_last_message_receive_time()
        print ("Last Message: %s" % time.asctime(time.localtime(lastMessage)))
        print ("Actual time : %s" % time.asctime())
    except IoTHubClientError as e:
        if (e.args[0].result == IoTHubClientResult.INDEFINITE_TIME):
            print ("No message received")
        else:
            print (e)


def iothub_client_sample_run():

    try:

        iotHubClient = iothub_client_init()

        while True:
            # send a few messages every minute
            print ("IoTHubClient sending %d messages" % message_count)

            for i in range(0, message_count):
                msgTxtFormatted = msgTxt % (
                    avgWindSpeed + (random.random() * 4 + 2))
                message = IoTHubMessage(msgTxtFormatted)
                propMap = message.properties()
                propText = "PropMsg_%d" % i
                propMap.add_or_update("PropName", propText)
                iotHubClient.send_event_async(
                    message, send_confirmation_callback, i)
                print (
                    "IoTHubClient.send_event_async accepted message [%d] for transmission to IoT Hub." %
                    i)

            # Wait for Commands or exit
            print ("IoTHubClient waiting for commands, press Ctrl-C to exit")

            n = 0
            while n < 6:
                status = iotHubClient.get_send_status()
                print ("Send status: %s" % status)
                time.sleep(10)
                n += 1

    except (IoTHubClientError, IoTHubMessageError, IoTHubMapError) as e:
        print ("Unexpected error %s from IoTHub" % e)
        return
    except KeyboardInterrupt:
        print ("IoTHubClient sample stopped")

    print_last_message_time(iotHubClient)

print ("\nPython %s\n" % sys.version)
print ("IoT Hub for Python SDK Version: %s\n" % iothub_client.__version__)
print ("Starting the IoT Hub Python sample using protocol %s..." % Protocol)

iothub_client_sample_run()
