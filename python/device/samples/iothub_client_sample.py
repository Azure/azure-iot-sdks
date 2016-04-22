#!/usr/bin/env python

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import random
import time
import sys
import iothub_client
from iothub_client import *
from iothub_client_args import *

# HTTP options
# Because it can poll "after 9 seconds" polls will happen effectively
# at ~10 seconds.
# Note that for scalabilty, the default value of minimumPollingTime
# is 25 minutes. For more information, see:
# https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
timeout = 241000
minimum_polling_time = 9

receive_context = 0
avg_wind_speed = 10.0
message_count = 5
received_count = 0

# global counters
receive_callbacks = 0
send_callbacks = 0

# chose HTTP, AMQP or MQTT as transport protocol
protocol = IoTHubTransportProvider.AMQP

# String containing Hostname, Device Id & Device Key in the format:
# "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
connection_string = "[device connection string]"

msg_txt = "{\"deviceId\": \"myPythonDevice\",\"windSpeed\": %.2f}"

# some embedded platforms need certificate information


def set_certificates(iotHubClient):
    from iothub_client_cert import certificates
    try:
        iotHubClient.set_option("TrustedCerts", certificates)
        print("set_option TrustedCerts successful")
    except IoTHubClientError as e:
        print("set_option TrustedCerts failed (%s)" % e)


def receive_message_callback(message, counter):
    global receive_callbacks
    buffer = message.get_bytearray()
    size = len(buffer)
    print("Received Message [%d]:" % counter)
    print("    Data: <<<%s>>> & Size=%d" %
          (buffer[:size].decode('utf-8'), size))
    map_properties = message.properties()
    key_value_pair = map_properties.get_internals()
    print("    Properties: %s" % key_value_pair)
    counter += 1
    receive_callbacks += 1
    print("    Total calls received: %d" % receive_callbacks)
    return IoTHubMessageDispositionResult.ACCEPTED


def send_confirmation_callback(message, result, user_context):
    global send_callbacks
    print(
        "Confirmation[%d] received for message with result = %s" %
        (user_context, result))
    map_properties = message.properties()
    print("    message_id: %s" % message.message_id)
    print("    correlation_id: %s" % message.correlation_id)
    key_value_pair = map_properties.get_internals()
    print("    Properties: %s" % key_value_pair)
    send_callbacks += 1
    print("    Total calls confirmed: %d" % send_callbacks)


def iothub_client_init():
    # prepare iothub client
    iotHubClient = IoTHubClient(connection_string, protocol)
    if iotHubClient.protocol == IoTHubTransportProvider.HTTP:
        iotHubClient.set_option("timeout", timeout)
        iotHubClient.set_option("MinimumPollingTime", minimum_polling_time)
    # some embedded platforms need certificate information
    # set_certificates(iotHubClient)
    # to enable MQTT logging set to 1
    if iotHubClient.protocol == IoTHubTransportProvider.MQTT:
        iotHubClient.set_option("logtrace", 0)
    iotHubClient.set_message_callback(
        receive_message_callback, receive_context)
    return iotHubClient


def print_last_message_time(iotHubClient):
    try:
        last_message = iotHubClient.get_last_message_receive_time()
        print("Last Message: %s" % time.asctime(time.localtime(last_message)))
        print("Actual time : %s" % time.asctime())
    except IoTHubClientError as e:
        if (e.args[0].result == IoTHubClientResult.INDEFINITE_TIME):
            print("No message received")
        else:
            print(e)


def iothub_client_sample_run():

    try:

        iotHubClient = iothub_client_init()

        while True:
            # send a few messages every minute
            print("IoTHubClient sending %d messages" % message_count)

            for i in range(0, message_count):
                msg_txt_formatted = msg_txt % (
                    avg_wind_speed + (random.random() * 4 + 2))
                # messages can be encoded as string or bytearray
                if (i & 1) == 1:
                    message = IoTHubMessage(
                        bytearray(msg_txt_formatted, 'utf8'))
                else:
                    message = IoTHubMessage(msg_txt_formatted)
                # optional: assign ids
                message.message_id = "message_%d" % i
                message.correlation_id = "correlation_%d" % i
                # optional: assign properties
                prop_map = message.properties()
                prop_text = "PropMsg_%d" % i
                prop_map.add("Property", prop_text)
                iotHubClient.send_event_async(
                    message, send_confirmation_callback, i)
                print(
                    "IoTHubClient.send_event_async accepted message [%d]"
                    " for transmission to IoT Hub." %
                    i)

            # Wait for Commands or exit
            print("IoTHubClient waiting for commands, press Ctrl-C to exit")

            n = 0
            while n < 6:
                status = iotHubClient.get_send_status()
                print("Send status: %s" % status)
                time.sleep(10)
                n += 1

    except IoTHubError as e:
        print("Unexpected error %s from IoTHub" % e)
        return
    except KeyboardInterrupt:
        print("IoTHubClient sample stopped")

    print_last_message_time(iotHubClient)


def usage():
    print("Usage: iothub_client_sample.py -p <protocol> -c <connectionstring>")
    print("    protocol        : <amqp, http, mqtt>")
    print("    connectionstring: <HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>>")


if __name__ == '__main__':
    print("\nPython %s" % sys.version)
    print("IoT Hub for Python SDK Version: %s" % iothub_client.__version__)

    try:
        (connection_string, protocol) = get_iothub_opt(
            sys.argv[1:], connection_string, protocol)
    except OptionError as o:
        print(o)
        usage()
        sys.exit(1)

    print("Starting the IoT Hub Python sample...")
    print("    Protocol %s" % protocol)
    print("    Connection string=%s" % connection_string)

    iothub_client_sample_run()
