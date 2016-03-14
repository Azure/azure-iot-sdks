#!/usr/bin/env python

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import random
import time
from iothub_client import *

timeout = 241000
minimumPollingTime = 9
receiveContext = 0
avgWindSpeed = 10.0
message_count = 5
received_count = 0

# global counters
receive_callbacks = 0
send_callbacks = 0

# transport protocol
Protocol = IoTHubTransportProvider.HTTP

# String containing Hostname, Device Id & Device Key in the format:
# "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
connectionString = "[device connection string]"
msgTxt = "{\"deviceId\": \"myFirstDevice\",\"windSpeed\": %.2f}"


def receive_message_callback(message, counter):
    global receive_callbacks
    buffer = message.get_bytearray()
    size = len(buffer)
    print "Received Message [%d]:" % counter
    print "    Data: <<<%s>>> & Size=%d" % (buffer[:size], size)
    mapProperties = message.properties()
    keyValuePair = mapProperties.get_internals()
    print "    Properties: %s" % keyValuePair
    counter += 1
    receive_callbacks += 1
    print "    Total calls received: %d" % receive_callbacks
    return IoTHubMessageDispositionResult.ACCEPTED


def send_confirmation_callback(message, result, userContext):
    global send_callbacks
    print "Confirmation[%d] received for message with result = %s" % (userContext, result)
    mapProperties = message.properties()
    keyValuePair = mapProperties.get_internals()
    print "    message_id: %s" % message.message_id
    print "    correlation_id: %s" % message.correlation_id
    print "    Properties: %s" % keyValuePair
    send_callbacks += 1
    print "    Total calls confirmed: %d" % send_callbacks


def iothub_client_init():
    # prepare iothub client
    iotHubClient = IoTHubClient(connectionString, Protocol)
    if iotHubClient.protocol == IoTHubTransportProvider.HTTP:
        iotHubClient.set_option("timeout", timeout)
        iotHubClient.set_option("MinimumPollingTime", minimumPollingTime)
    iotHubClient.set_message_callback(receive_message_callback, receiveContext)
    return iotHubClient


def print_last_message_time(iotHubClient):
    try:
        print "Actual time : %s" % time.localtime()
        lastMessage = iotHubClient.get_last_message_receive_time()
        print "Last Message: %s" % time.localtime(lastMessage)
    except IoTHubClientError as e:
        if e.result == IoTHubClientResult.INDEFINITE_TIME:
            print "No message received"
        else:
            print e


def iothub_client_sample_run():

    try:

        iotHubClient = iothub_client_init()

        while True:
            # send a few messages every minute
            print "IoTHubClient sending %d messages" % message_count

            for i in xrange(0, message_count):
                msgTxtFormatted = msgTxt % (
                    avgWindSpeed + (random.random() * 4 + 2))
                message = IoTHubMessage(msgTxtFormatted)
                message.message_id = "message_%d" % i
                message.correlation_id = "correlation_%d" % i
                propMap = message.properties()
                propText = "PropMsg_%d" % i
                propMap.add_or_update("PropName", propText)
                iotHubClient.send_event_async(
                    message, send_confirmation_callback, i)
                print "IoTHubClient.send_event_async accepted message [%d] for transmission to IoT Hub." % i

            # Wait for Commands or exit
            print "IoTHubClient waiting for commands, press Ctrl-C to exit"

            time.sleep(60)

    except (IoTHubClientError, IoTHubMessageError, IoTHubMapError) as e:
        print "Unexpected error %s from IoTHub" % e
        return
    except KeyboardInterrupt:
        print "IoTHubClient sample stopped"

    print_last_message_time(iotHubClient)

print "Starting the IoTHubClient sample using protocol %s..." % Protocol

iothub_client_sample_run()
