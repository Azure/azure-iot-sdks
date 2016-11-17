# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import getopt
from iothub_client import IoTHubTransportProvider


class OptionError(Exception):

    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


def get_iothub_opt(
        argv,
        connection_string,
        protocol=IoTHubTransportProvider.MQTT):
    if len(argv) > 0:
        try:
            opts, args = getopt.getopt(
                argv, "hp:c:", [
                    "protocol=", "connectionstring="])
        except getopt.GetoptError as e:
            raise OptionError("Error: %s" % e.msg)
        for opt, arg in opts:
            if opt == '-h':
                raise OptionError("Help:")
            elif opt in ("-p", "--protocol"):
                protocol_string = arg.lower()

                if (protocol_string == "http"):
                    if hasattr(IoTHubTransportProvider, "HTTP"):
                        protocol = IoTHubTransportProvider.HTTP
                    else:
                        raise OptionError("Error: HTTP protocol is not supported")

                elif (protocol_string == "amqp"):
                    if hasattr(IoTHubTransportProvider, "AMQP"):
                        protocol = IoTHubTransportProvider.AMQP
                    else:
                        raise OptionError("Error: AMQP protocol is not supported")

                elif (protocol_string == "amqp_ws"):
                    if hasattr(IoTHubTransportProvider, "AMQP_WS"):
                        protocol = IoTHubTransportProvider.AMQP_WS
                    else:
                        raise OptionError("Error: AMQP_WS protocol is not supported")

                elif (protocol_string == "mqtt"):
                    if hasattr(IoTHubTransportProvider, "MQTT"):
                        protocol = IoTHubTransportProvider.MQTT
                    else:
                        raise OptionError("Error: MQTT protocol is not supported")

                elif hasattr(IoTHubTransportProvider, "MQTT_WS"):
                    if hasattr(IoTHubTransportProvider, "MQTT_WS"):
                        protocol = IoTHubTransportProvider.MQTT_WS
                    else:
                        raise OptionError("Error: MQTT_WS protocol is not supported")
                else:
                    raise OptionError(
                        "Error: unknown protocol %s" %
                        protocol_string)
            elif opt in ("-c", "--connectionstring"):
                connection_string = arg

    if (connection_string.find("HostName") < 0):
        raise OptionError(
            "Error: Hostname not found, not a valid connection string")

    return connection_string, protocol
