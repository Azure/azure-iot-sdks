#!/usr/bin/env python

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import random
import time
from iothub_client import *
import inspect

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
Protocol = IoTHubTransportProvider.AMQP

# String containing Hostname, Device Id & Device Key in the format:
# "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
connectionString = "[device connection string]"
msgTxt = "{\"deviceId\": \"myFirstDevice\",\"windSpeed\": %.2f}"


def receive_message_callback(message, counter):
    global receive_callbacks
    buffer = message.get_bytearray()
    size = len(buffer)
    print("Received Message [{}]:".format(counter))
    print("    Data: <<<{}>>> & Size={}".format((buffer[:size], size)))
    mapProperties = message.properties()
    keyValuePair = mapProperties.get_internals()
    print("    Properties: {}".format(keyValuePair))
    counter += 1
    receive_callbacks += 1
    print("    Total calls received: {}".format(receive_callbacks))
    return IoTHubMessageDispositionResult.ACCEPTED


def send_confirmation_callback(message, result, userContext):
    global send_callbacks
    print("Confirmation[{}] received for message with result = {}".format((userContext, result)))
    mapProperties = message.properties()
    keyValuePair = mapProperties.get_internals()
    print("    Properties: {}".format(keyValuePair))
    send_callbacks += 1
    print("    Total calls confirmed: {}".format(send_callbacks))


def iothub_client_init():
    # prepare iothub client
    iotHubClient = IoTHubClient(connectionString, Protocol)
    iotHubClient.set_message_callback(receive_message_callback, receiveContext)
    return iotHubClient


def print_last_message_time(iotHubClient):
    try:
        print("Actual time : {}".format(time.localtime()))
        lastMessage = iotHubClient.get_last_message_receive_time()
        print("Last Message: {}".format(time.localtime(lastMessage)))
    except Exception as e:
        if e.result == IoTHubClientResult.INDEFINITE_TIME:
            print("No message received")
        else:
            print(e)


def iothub_client_sample_run():

    try:

        iotHubClient = iothub_client_init()

        while True:
            # send a few messages every minute
            print("IoTHubClient sending {} messages".format(message_count))

            for i in range(message_count):
                msgTxtFormatted = msgTxt.format(
                    avgWindSpeed + (random.random() * 4 + 2))
                message = IoTHubMessage(bytearray(msgTxtFormatted, 'utf8'))
                propMap = message.properties()
                propText = "PropMsg_{}".format(i)
                propMap.add("Property", propText)
                iotHubClient.send_event_async(
                    message, send_confirmation_callback, i)
                print("IoTHubClient.send_event_async accepted message [{}] for transmission to IoT Hub.".format(i))

            # Wait for Commands or exit
            print("IoTHubClient waiting for commands, press Ctrl-C to exit")

            n = 0
            while n < 6:
                status = iotHubClient.get_send_status()
                print("Send status: {}".format(status))
                time.sleep(10)
                n += 1

    except Exception as e:
        print("exception type: {}".format(type(e)))
        # print(inspect.getmembers(e))
        print("Unexpected error {} from IoTHub".format(e))
        return
    except KeyboardInterrupt:
        print("IoTHubClient sample stopped")

    print_last_message_time(iotHubClient)

print("Starting the IoTHubClient sample using protocol {}...".format(Protocol))

iothub_client_sample_run()
