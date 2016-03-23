# Microsoft Azure IoT device SDK for Python 2.7

Build devices that communicate with Azure IoT Hub.

## Features

Use the device SDK to:
* Send event data to Azure IoT Hub.
* Receive messages from IoT Hub.
* Communicate with the service via AMQP, MQTT or HTTP.

## Application development guides
For more information on how to use this library refer to the documents below:
- [Prepare your Python development environment](../../doc/get_started/python-devbox-setup.md)
- [Setup IoT Hub](../../doc/setup_iothub.md)
- [Provision devices](../../doc/manage_iot_hub.md)
- [Run a Python sample application](../../doc/get_started/python-run-sample.md)

## Directory structure

Device SDK subfolders under **python/device**:

### /iothub_client_python

C Source of the Python extension module. This module wraps the IoT Hub C SDK as extension module for Python. The C extension interface is specific to CPython and it does not work on other implementations.

### /samples

Sample Python applications excercising basic features using AMQP, MQTT and HTTP.

### /tests

Python C extension module unit tests. The unit tests exercise a mocked Python extension module to test the Python interface. 
