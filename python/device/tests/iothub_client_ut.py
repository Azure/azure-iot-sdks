#!/usr/bin/env python

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import sys
import unittest
from iothub_client_mock import *

# connnection strings for mock testing
connectionString = "HostName=mockhub.azure-devices.net;DeviceId=mockdevice;SharedAccessKey=Sj3IdHgrRmvWFzIwCMu0MKrSChIg8d244DOeFDAO7+k="

callback_key = ""
callback_value = ""
callback_message = ""


def map_callback_ok(key, value):
    global callback_key
    global callback_value
    callback_key = key
    callback_value = value
    return IoTHubMapResult.OK


def map_callback_reject(key, value):
    global callback_key
    global callback_value
    callback_key = key
    callback_value = value
    return IoTHubMapResult.FILTER_REJECT


def receive_message_callback(message, counter):
    return IoTHubMessageDispositionResult.ACCEPTED


def send_confirmation_callback(message, result, userContext):
    return


class TestExceptionDefinitions(unittest.TestCase):

    def test_IoTHubMapError(self):
        with self.assertRaises(Exception):
            error = IoTHubMapError()
        with self.assertRaises(Exception):
            error = IoTHubMapError(__name__)
        with self.assertRaises(Exception):
            error = IoTHubMapError(__name__, "function")
        with self.assertRaises(Exception):
            error = IoTHubMapError(IoTHubMapResult.ERROR)
        error = IoTHubMapError("function", IoTHubMapResult.ERROR)
        with self.assertRaises(TypeError):
            raise IoTHubMapError("function", IoTHubMapResult.ERROR)

    def test_IoTHubMessageError(self):
        with self.assertRaises(Exception):
            error = IoTHubMessageError()
        with self.assertRaises(Exception):
            error = IoTHubMessageError(__name__)
        with self.assertRaises(Exception):
            error = IoTHubMessageError(__name__, "function")
        with self.assertRaises(Exception):
            error = IoTHubMessageError(IoTHubMapResult.ERROR)
        error = IoTHubMessageError("function", IoTHubMessageResult.ERROR)
        with self.assertRaises(TypeError):
            raise IoTHubMessageError("function", IoTHubMessageResult.ERROR)

    def test_IoTHubClientError(self):
        with self.assertRaises(Exception):
            error = IoTHubClientError()
        with self.assertRaises(Exception):
            error = IoTHubClientError(__name__)
        with self.assertRaises(Exception):
            error = IoTHubClientError(__name__, "function")
        with self.assertRaises(Exception):
            error = IoTHubClientError(IoTHubMapResult.ERROR)
        error = IoTHubClientError("function", IoTHubClientResult.ERROR)
        with self.assertRaises(TypeError):
            raise IoTHubClientError("function", IoTHubClientResult.ERROR)


class TestEnumDefinitions(unittest.TestCase):

    def test_IoTHubMapResult(self):
        self.assertEqual(IoTHubMapResult.OK, 0)
        self.assertEqual(IoTHubMapResult.ERROR, 1)
        self.assertEqual(IoTHubMapResult.INVALIDARG, 2)
        self.assertEqual(IoTHubMapResult.KEYEXISTS, 3)
        self.assertEqual(IoTHubMapResult.KEYNOTFOUND, 4)
        self.assertEqual(IoTHubMapResult.FILTER_REJECT, 5)
        lastEnum = IoTHubMapResult.FILTER_REJECT + 1
        with self.assertRaises(AttributeError):
            self.assertEqual(IoTHubMapResult.ANY, 0)
        mapResult = IoTHubMapResult()
        self.assertEqual(mapResult, 0)
        self.assertEqual(len(mapResult.names), lastEnum)
        self.assertEqual(len(mapResult.values), lastEnum)

    def test_IoTHubMessageResult(self):
        self.assertEqual(IoTHubMessageResult.OK, 0)
        self.assertEqual(IoTHubMessageResult.INVALID_ARG, 1)
        self.assertEqual(IoTHubMessageResult.INVALID_TYPE, 2)
        self.assertEqual(IoTHubMessageResult.ERROR, 3)
        lastEnum = IoTHubMessageResult.ERROR + 1
        with self.assertRaises(AttributeError):
            self.assertEqual(IoTHubMessageResult.ANY, 0)
        messageResult = IoTHubMessageResult()
        self.assertEqual(messageResult, 0)
        self.assertEqual(len(messageResult.names), lastEnum)
        self.assertEqual(len(messageResult.values), lastEnum)

    def test_IoTHubClientResult(self):
        self.assertEqual(IoTHubClientResult.OK, 0)
        self.assertEqual(IoTHubClientResult.INVALID_ARG, 1)
        self.assertEqual(IoTHubClientResult.ERROR, 2)
        self.assertEqual(IoTHubClientResult.INVALID_SIZE, 3)
        self.assertEqual(IoTHubClientResult.INDEFINITE_TIME, 4)
        lastEnum = IoTHubClientResult.INDEFINITE_TIME + 1
        with self.assertRaises(AttributeError):
            self.assertEqual(IoTHubClientResult.ANY, 0)
        clientResult = IoTHubClientResult()
        self.assertEqual(clientResult, 0)
        self.assertEqual(len(clientResult.names), lastEnum)
        self.assertEqual(len(clientResult.values), lastEnum)

    def test_IoTHubClientStatus(self):
        self.assertEqual(IoTHubClientStatus.IDLE, 0)
        self.assertEqual(IoTHubClientStatus.BUSY, 1)
        lastEnum = IoTHubClientStatus.BUSY + 1
        with self.assertRaises(AttributeError):
            self.assertEqual(IoTHubClientStatus.ANY, 0)
        clientStatus = IoTHubClientStatus()
        self.assertEqual(clientStatus, 0)
        self.assertEqual(len(clientStatus.names), lastEnum)
        self.assertEqual(len(clientStatus.values), lastEnum)

    def test_IoTHubClientConfirmationResult(self):
        self.assertEqual(IoTHubClientConfirmationResult.OK, 0)
        self.assertEqual(IoTHubClientConfirmationResult.BECAUSE_DESTROY, 1)
        self.assertEqual(IoTHubClientConfirmationResult.MESSAGE_TIMEOUT, 2)
        self.assertEqual(IoTHubClientConfirmationResult.ERROR, 3)
        lastEnum = IoTHubClientConfirmationResult.ERROR + 1
        with self.assertRaises(AttributeError):
            self.assertEqual(IoTHubClientConfirmationResult.ANY, 0)
        confirmationResult = IoTHubClientConfirmationResult()
        self.assertEqual(confirmationResult, 0)
        self.assertEqual(len(confirmationResult.names), lastEnum)
        self.assertEqual(len(confirmationResult.values), lastEnum)

    def test_IoTHubMessageDispositionResult(self):
        self.assertEqual(IoTHubMessageDispositionResult.ACCEPTED, 0)
        self.assertEqual(IoTHubMessageDispositionResult.REJECTED, 1)
        self.assertEqual(IoTHubMessageDispositionResult.ABANDONED, 2)
        lastEnum = IoTHubMessageDispositionResult.ABANDONED + 1
        with self.assertRaises(AttributeError):
            self.assertEqual(IoTHubMessageDispositionResult.ANY, 0)
        dispositionResult = IoTHubMessageDispositionResult()
        self.assertEqual(dispositionResult, 0)
        self.assertEqual(len(dispositionResult.names), lastEnum)
        self.assertEqual(len(dispositionResult.values), lastEnum)

    def test_IoTHubMessageContent(self):
        self.assertEqual(IoTHubMessageContent.BYTEARRAY, 0)
        self.assertEqual(IoTHubMessageContent.STRING, 1)
        self.assertEqual(IoTHubMessageContent.UNKNOWN, 2)
        lastEnum = IoTHubMessageContent.UNKNOWN + 1
        with self.assertRaises(AttributeError):
            self.assertEqual(IoTHubMessageContent.ANY, 0)
        dispositionResult = IoTHubMessageContent()
        self.assertEqual(dispositionResult, 0)
        self.assertEqual(len(dispositionResult.names), lastEnum)
        self.assertEqual(len(dispositionResult.values), lastEnum)

    def test_IoTHubTransportProvider(self):
        self.assertEqual(IoTHubTransportProvider.HTTP, 0)
        self.assertEqual(IoTHubTransportProvider.AMQP, 1)
        self.assertEqual(IoTHubTransportProvider.MQTT, 2)
        lastEnum = IoTHubTransportProvider.MQTT + 1
        with self.assertRaises(AttributeError):
            self.assertEqual(IoTHubTransportProvider.ANY, 0)
        transportProvider = IoTHubTransportProvider()
        self.assertEqual(transportProvider, 0)
        self.assertEqual(len(transportProvider.names), lastEnum)
        self.assertEqual(len(transportProvider.values), lastEnum)


class TestClassDefinitions(unittest.TestCase):

    def test_IoTHubMap(self):
        # constructor
        map = IoTHubMap(map_callback_ok)
        self.assertIsInstance(map, IoTHubMap)
        map = IoTHubMap()
        self.assertIsInstance(map, IoTHubMap)
        with self.assertRaises(Exception):
            map1 = IoTHubMap(1)
        # add
        map = IoTHubMap()
        with self.assertRaises(AttributeError):
            map.Add()
        with self.assertRaises(Exception):
            map.add()
        with self.assertRaises(Exception):
            map.add("key")
        with self.assertRaises(Exception):
            map.add(["key", "value"])
        result = map.add("key", "value")
        self.assertIsNone(result)
        # add_or_update
        with self.assertRaises(AttributeError):
            map.AddOrUpdate()
        with self.assertRaises(Exception):
            map.add_or_update()
        with self.assertRaises(Exception):
            map.add_or_update("key")
        with self.assertRaises(Exception):
            map.add_or_update(["key", "value"])
        result = map.add_or_update("key", "value")
        self.assertIsNone(result)
        # add, cannot change existing key
        with self.assertRaises(IoTHubMapError):
            result = map.add("key", "value")
        # contains_key
        with self.assertRaises(AttributeError):
            map.ContainsKey()
        with self.assertRaises(Exception):
            map.contains_key()
        with self.assertRaises(Exception):
            map.contains_key("key", "value")
        with self.assertRaises(Exception):
            map.contains_key(["key", "value"])
        self.assertTrue(map.contains_key("key"))
        self.assertFalse(map.contains_key("anykey"))
        # contains_value
        with self.assertRaises(AttributeError):
            map.ContainsValue()
        with self.assertRaises(Exception):
            map.contains_value()
        with self.assertRaises(Exception):
            map.contains_value("key", "value")
        with self.assertRaises(Exception):
            map.contains_value(["key", "value"])
        self.assertTrue(map.contains_value("value"))
        self.assertFalse(map.contains_value("anyvalue"))
        # get_value_from_key
        with self.assertRaises(AttributeError):
            map.GetValueFromKey()
        with self.assertRaises(Exception):
            map.get_value_from_key()
        with self.assertRaises(Exception):
            map.get_value_from_key("key", "value")
        with self.assertRaises(Exception):
            map.get_value_from_key(["key", "value"])
        self.assertEqual(map.get_value_from_key("key"), "value")
        with self.assertRaises(IoTHubMapError):
            result = map.get_value_from_key("anykey")
        # get_internals
        with self.assertRaises(AttributeError):
            map.GetInternals()
        with self.assertRaises(Exception):
            map.get_internals("key")
        with self.assertRaises(Exception):
            map.get_internals("key", "value")
        with self.assertRaises(Exception):
            map.get_internals(["key", "value"])
        keymap = map.get_internals()
        self.assertEqual(len(keymap), 1)
        self.assertEqual(keymap, {"key": "value"})
        # delete
        with self.assertRaises(AttributeError):
            map.Delete()
        with self.assertRaises(Exception):
            map.delete()
        with self.assertRaises(Exception):
            map.delete("key", "value")
        with self.assertRaises(Exception):
            map.delete(["key", "value"])
        result = map.delete("key")
        self.assertIsNone(result)
        with self.assertRaises(IoTHubMapError):
            result = map.delete("key")
        # get_internals
        keymap = map.get_internals()
        self.assertIsNotNone(keymap)
        self.assertEqual(len(keymap), 0)
        self.assertEqual(keymap, {})
        # test filter callback
        global callback_key
        global callback_value
        callback_key = ""
        callback_value = ""
        map = IoTHubMap(map_callback_ok)
        map.add("key", "value")
        self.assertEqual(len(map.get_internals()), 1)
        self.assertEqual(callback_key, "key")
        self.assertEqual(callback_value, "value")
        callback_key = ""
        callback_value = ""
        map = IoTHubMap(map_callback_reject)
        with self.assertRaises(IoTHubMapError):
            map.add("key", "value")
        self.assertEqual(len(map.get_internals()), 0)
        self.assertEqual(callback_key, "key")
        self.assertEqual(callback_value, "value")

    def test_IoTHubMessage(self):
        # constructor
        messageString = "myMessage"
        with self.assertRaises(Exception):
            message = IoTHubMessage()
        with self.assertRaises(Exception):
            message = IoTHubMessage(1)
        with self.assertRaises(Exception):
            message = IoTHubMessage(buffer(messageString))
        message = IoTHubMessage(messageString)
        self.assertIsInstance(message, IoTHubMessage)
        # get_bytearray
        message = IoTHubMessage(bytearray(messageString))
        self.assertIsInstance(message, IoTHubMessage)
        with self.assertRaises(AttributeError):
            message.GetByteArray()
        with self.assertRaises(Exception):
            message.get_bytearray(1)
        with self.assertRaises(Exception):
            message.get_bytearray("key")
        with self.assertRaises(Exception):
            message.get_bytearray(["key", "value"])
        result = message.get_bytearray()
        self.assertEqual(result, "myMessage")
        # get_string
        message = IoTHubMessage(messageString)
        self.assertIsInstance(message, IoTHubMessage)
        with self.assertRaises(AttributeError):
            message.GetString()
        with self.assertRaises(Exception):
            message.get_string(1)
        with self.assertRaises(Exception):
            message.get_string("key")
        with self.assertRaises(Exception):
            message.get_string(["key", "value"])
        result = message.get_string()
        self.assertEqual(result, "myMessage")
        # get_content_type
        with self.assertRaises(AttributeError):
            message.GetContentType()
        with self.assertRaises(Exception):
            message.get_content_type(1)
        with self.assertRaises(Exception):
            message.get_content_type("key")
        with self.assertRaises(Exception):
            message.get_content_type(["key", "value"])
        message = IoTHubMessage(messageString)
        result = message.get_content_type()
        self.assertEqual(result, IoTHubMessageContent.STRING)
        message = IoTHubMessage(bytearray(messageString))
        result = message.get_content_type()
        self.assertEqual(result, IoTHubMessageContent.BYTEARRAY)
        # properties
        with self.assertRaises(AttributeError):
            message.Properties()
        with self.assertRaises(Exception):
            message.properties(1)
        with self.assertRaises(Exception):
            message.properties("key")
        with self.assertRaises(Exception):
            message.properties(["key", "value"])
        map = message.properties()
        self.assertIsInstance(map, IoTHubMap)
        keymap = map.get_internals()
        # get message_id
        with self.assertRaises(AttributeError):
            message.GetMessageId()
        with self.assertRaises(Exception):
            result = message.message_id()
        with self.assertRaises(Exception):
            result = message.message_id("key")
        with self.assertRaises(Exception):
            result = message.message_id(["key", "value"])
        result = message.message_id
        self.assertEqual(result, None)
        #  set message_id
        with self.assertRaises(AttributeError):
            message.SetMessageId()
        with self.assertRaises(Exception):
            message.message_id()
        with self.assertRaises(Exception):
            message.message_id = 1
        with self.assertRaises(Exception):
            message.message_id = ["key", "value"]
        result = message.message_id = "messageId"
        self.assertEqual(result, "messageId")
        # get message_id & set message_id
        result = message.message_id = "xyz"
        self.assertEqual(result, "xyz")
        result = message.message_id
        self.assertEqual(result, "xyz")
        # get correlation_id
        with self.assertRaises(AttributeError):
            message.GetCorrelationId()
        with self.assertRaises(Exception):
            result = message.correlation_id(1)
        with self.assertRaises(Exception):
            result = message.correlation_id("key")
        with self.assertRaises(Exception):
            result = message.correlation_id(["key", "value"])
        result = message.correlation_id
        self.assertIsNone(result)
        # set correlation_id
        with self.assertRaises(AttributeError):
            message.SetCorrelationId()
        with self.assertRaises(Exception):
            message.correlation_id()
        with self.assertRaises(Exception):
            message.correlation_id(1)
        with self.assertRaises(Exception):
            message.correlation_id(["key", "value"])
        result = message.correlation_id = "correlation_id"
        self.assertEqual(result, "correlation_id")
        # get & set correlation_id
        result = message.correlation_id = "xyz"
        self.assertEqual(result, "xyz")
        result = message.correlation_id
        self.assertEqual(result, "xyz")

    def test_IoTHubClient(self):
        # constructor
        with self.assertRaises(Exception):
            client = IoTHubClient()
        with self.assertRaises(Exception):
            client = IoTHubClient(1)
        with self.assertRaises(Exception):
            client = IoTHubClient(connectionString)
        with self.assertRaises(Exception):
            client = IoTHubClient(connectionString, 1)
        client = IoTHubClient(connectionString, IoTHubTransportProvider.HTTP)
        self.assertIsInstance(client, IoTHubClient)
        self.assertEqual(client.protocol, IoTHubTransportProvider.HTTP)
        with self.assertRaises(AttributeError):
            client.protocol = IoTHubTransportProvider.AMQP
        client = IoTHubClient(connectionString, IoTHubTransportProvider.AMQP)
        self.assertIsInstance(client, IoTHubClient)
        self.assertEqual(client.protocol, IoTHubTransportProvider.AMQP)
        with self.assertRaises(AttributeError):
            client.protocol = IoTHubTransportProvider.AMQP
        client = IoTHubClient(connectionString, IoTHubTransportProvider.MQTT)
        self.assertIsInstance(client, IoTHubClient)
        self.assertEqual(client.protocol, IoTHubTransportProvider.MQTT)
        with self.assertRaises(AttributeError):
            client.protocol = IoTHubTransportProvider.AMQP
        with self.assertRaises(AttributeError):
            client.protocol = IoTHubTransportProvider.AMQP
        with self.assertRaises(AttributeError):
            client.protocol = 1
        # set_message_callback
        counter = 1
        context = {"a": "b"}
        with self.assertRaises(AttributeError):
            client.SetMessageCallback()
        with self.assertRaises(Exception):
            client.set_message_callback()
        with self.assertRaises(Exception):
            client.set_message_callback(receive_message_callback)
        with self.assertRaises(Exception):
            client.set_message_callback(counter, receive_message_callback)
        with self.assertRaises(Exception):
            client.set_message_callback(
                receive_message_callback, counter, context)
        result = client.set_message_callback(receive_message_callback, counter)
        self.assertIsNone(result)
        result = client.set_message_callback(receive_message_callback, context)
        self.assertIsNone(result)
        # send_event_async
        counter = 1
        message = IoTHubMessage("myMessage")
        with self.assertRaises(AttributeError):
            client.SendEventAsync()
        with self.assertRaises(Exception):
            client.send_event_async()
        with self.assertRaises(Exception):
            client.send_event_async(send_confirmation_callback)
        with self.assertRaises(Exception):
            client.send_event_async(message, send_confirmation_callback)
        with self.assertRaises(Exception):
            client.send_event_async(
                send_confirmation_callback, message, counter)
        result = client.send_event_async(
            message, send_confirmation_callback, counter)
        self.assertIsNone(result)
        # get_send_status
        with self.assertRaises(AttributeError):
            client.GetSendStatus()
        with self.assertRaises(Exception):
            client.get_send_status(1)
        with self.assertRaises(Exception):
            client.get_send_status(counter)
        result = client.get_send_status()
        self.assertIsNotNone(result)
        # get_last_message_receive_time
        with self.assertRaises(AttributeError):
            result = client.GetLastMessageReceiveTime()
        with self.assertRaises(Exception):
            result = client.get_last_message_receive_time(1)
        with self.assertRaises(Exception):
            client.get_last_message_receive_time(counter)
        with self.assertRaises(IoTHubClientError):
            result = client.get_last_message_receive_time()
        # set_option
        timeout = 241000
        with self.assertRaises(AttributeError):
            client.SetOption()
        with self.assertRaises(Exception):
            client.set_option(1)
        with self.assertRaises(Exception):
            client.set_option(timeout)
        with self.assertRaises(TypeError):
            client.set_option("timeout", "241000")
        result = client.set_option("timeout", timeout)
        self.assertIsNone(result)

if __name__ == '__main__':
    unittest.main(verbosity=2)
