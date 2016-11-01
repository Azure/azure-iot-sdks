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

# messageTimeout - the maximum time in milliseconds until a message times out.
# The timeout period starts at IoTHubClient.send_event_async. 
# By default, messages do not expire.
message_timeout = 10000

receive_context = 0
avg_wind_speed = 10.0
message_count = 5
received_count = 0

# global counters
receive_callbacks = 0
send_callbacks = 0
blob_callbacks = 0

# It is NOT a good practice to put device credentials in the code as shown below.
# This is done in this sample for simplicity purposes.
# Good practices such as the use of TPM or other secure storage on the device are recommended to store device credentials.

# String containing Hostname, Device Id & Device Key in the format:
# "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
connection_string = "[device connection string]"

msg_txt = "{\"deviceId\": \"myPythonDevice\",\"windSpeed\": %.2f}"


class HubManager(object):

    def __init__(
            self,
            connection_string,
            protocol=IoTHubTransportProvider.AMQP):
        self.client_protocol = protocol
        self.client = IoTHubClient(connection_string, protocol)
        if protocol == IoTHubTransportProvider.HTTP:
            self.client.set_option("timeout", timeout)
            self.client.set_option("MinimumPollingTime", minimum_polling_time)
        # set the time until a message times out
        self.client.set_option("messageTimeout", message_timeout)
        # some embedded platforms need certificate information
        # self.set_certificates()
        self.client.set_message_callback(self._receive_message_callback, receive_context)

    def set_certificates(self):
        from iothub_client_cert import certificates
        try:
            self.client.set_option("TrustedCerts", certificates)
            print("set_option TrustedCerts successful")
        except IoTHubClientError as e:
            print("set_option TrustedCerts failed (%s)" % e)

    def _receive_message_callback(self, message, counter):
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

    def _send_confirmation_callback(self, message, result, user_context):
        global send_callbacks
        print(
            "Confirmation[%d] received for message with result = %s" %
            (user_context, result))
        map_properties = message.properties()
        key_value_pair = map_properties.get_internals()
        print("    Properties: %s" % key_value_pair)
        send_callbacks += 1
        print("    Total calls confirmed: %d" % send_callbacks)


    def _blob_upload_confirmation_callback(self, result, user_context):
        global blob_callbacks
        print("Blob upload confirmation[%d] received for message with result = %s" % (user_context, result))
        blob_callbacks += 1
        print("    Total calls confirmed: %d" % blob_callbacks)


    def send_event(self, event, properties, send_context):
        if not isinstance(event, IoTHubMessage):
            event = IoTHubMessage(bytearray(event, 'utf8'))

        if len(properties) > 0:
            prop_map = event.properties()
            for key in properties:
                prop_map.add_or_update(key, properties[key])

        self.client.send_event_async(
            event, self._send_confirmation_callback, send_context)


    def upload_to_blob(self, destinationfilename, source, size, usercontext):
        self.client.upload_blob_async(
            destinationfilename, source, size,
            self._blob_upload_confirmation_callback, usercontext)



def main(connection_string, protocol):
    try:
        print("\nPython %s\n" % sys.version)
        print(
            "IoT Hub for Python SDK Version: %s\n" %
            iothub_client.__version__)

        hub_manager = HubManager(connection_string, protocol)

        print(
            "Starting the IoT Hub Python sample using protocol %s..." %
            hub_manager.client_protocol)

        filename= "hello_python_blob.txt"
        content = "Hello World from Python Blob APi"
        hub_manager.upload_to_blob(filename, content, len(content), 1001)

        while True:
            # send a few messages every minute
            print("IoTHubClient sending %d messages" % message_count)

            for i in range(0, message_count):
                msg_txt_formatted = msg_txt % (
                    avg_wind_speed + (random.random() * 4 + 2))
                msg_properties = {
                    "Property": "PropMsg_%d" % i
                }
                hub_manager.send_event(msg_txt_formatted, msg_properties, i)
                print(
                    "IoTHubClient.send_event_async accepted message [%d]"
                    " for transmission to IoT Hub." %
                    i)

            # Wait for Commands or exit
            print("IoTHubClient waiting for commands, press Ctrl-C to exit")

            n = 0
            while n < 6:
                status = hub_manager.client.get_send_status()
                print("Send status: %s" % status)
                time.sleep(10)
                n += 1

    except IoTHubError as e:
        print("Unexpected error %s from IoTHub" % e)
        return
    except KeyboardInterrupt:
        print("IoTHubClient sample stopped")


def usage():
    print("Usage: iothub_client_sample_class.py -p <protocol> -c <connectionstring>")
    print("    protocol        : <amqp, http, mqtt>")
    print("    connectionstring: <HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>>")

if __name__ == '__main__':
    try:
        (connection_string, protocol) = get_iothub_opt(sys.argv[1:], connection_string)
    except OptionError as o:
        print(o)
        usage()
        sys.exit(1)

    main(connection_string, protocol)
