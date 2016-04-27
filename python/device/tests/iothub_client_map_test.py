#!/usr/bin/env python

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.

import random
import unittest
import sys
try:
    import iothub_client
except ImportError:
    sys.path.insert(0, "../samples")
    import iothub_client
from iothub_client import IoTHubMap, IoTHubMapError, IoTHubMapResult, IoTHubMapErrorArg

test_count = 100

last_mapProperty = ""
last_mapValue = ""

# in Python a mapFilterCallback can only be used as singleton


def map_filter_callback(mapProperty, mapValue):
    global last_mapProperty, last_mapValue
    last_mapProperty = mapProperty
    last_mapValue = mapValue
    # return OK or FILTER_REJECT
    return IoTHubMapResult.OK


class TestIotHubMap(unittest.TestCase):

    def test_IoTHubMap(self):
        # create and destroy maps
        for i in range(0, test_count):
            map = IoTHubMap()
            self.assertIsInstance(map, IoTHubMap)
            mapclone = map
            self.assertIsInstance(mapclone, IoTHubMap)

        # verify filter function can only be used once
        map = IoTHubMap(map_filter_callback)
        with self.assertRaises(Exception):
            map2 = IoTHubMap(map_filter_callback)
        map2 = IoTHubMap()
        map = IoTHubMap()
        map2 = IoTHubMap(map_filter_callback)
        map2 = IoTHubMap()
        map = IoTHubMap(map_filter_callback)

    def test_IoTHubMapAddDelete(self):

        # create map and fill
        map = IoTHubMap(map_filter_callback)
        for y in range(0, test_count):
            key = "Key%d" % y
            value = "Value%d" % y
            map.add(key, value)
            self.assertEqual(key, last_mapProperty)
            self.assertEqual(value, last_mapValue)
            with self.assertRaises(IoTHubMapError):
                map.add(key, value)

        # update all keys
        for y in range(0, test_count):
            key = "Key%d" % y
            value = "Value%d" % y
            map.add_or_update(key, value)
            self.assertEqual(key, last_mapProperty)
            self.assertEqual(value, last_mapValue)
            with self.assertRaises(IoTHubMapError):
                map.add(key, value)

        # delete and add key
        map.delete(key)
        with self.assertRaises(IoTHubMapError):
            map.delete(key)
        map.add(key, value)
        self.assertEqual(key, last_mapProperty)
        self.assertEqual(value, last_mapValue)

        # delete a non existing key
        key = "DeleteMe"
        with self.assertRaises(IoTHubMapError):
            map.delete(key)

        # compare content of map
        internals = map.get_internals()
        count = len(internals)
        self.assertEqual(count, test_count)

        # test map get_value_from_key functions
        for y in range(0, test_count):
            key = "Key%d" % y
            value = "Value%d" % y
            self.assertEqual(internals[key], value)
            self.assertEqual(internals[key], map.get_value_from_key(key))

        # test map contains functions
        for y in range(0, test_count):
            key = "Key%d" % y
            value = "Value%d" % y
            self.assertTrue(map.contains_key(key))
            self.assertTrue(map.contains_value(value))
        self.assertFalse(map.contains_key("Keyxxx"))
        self.assertFalse(map.contains_value("Valuexxx"))

        # update map with new values
        for y in range(0, 2 * test_count):
            key = "Key%d" % y
            value = "*Value%d*" % y
            result = map.add_or_update(key, value)

        # read all key value pairs and test
        internals = map.get_internals()
        count = len(internals)
        self.assertEqual(count, 2 * test_count)
        for y in range(0, 2 * test_count):
            key = "Key%d" % y
            value = "*Value%d*" % y
            self.assertEqual(internals[key], value)

        # test if keys were updated
        for y in range(0, test_count):
            key = "Key%d" % y
            value = "Value%d" % y
            value2 = "*Value%d*" % y
            self.assertTrue(map.contains_key(key))
            self.assertFalse(map.contains_value(value))
            self.assertTrue(map.contains_value(value2))

        # test cloned map
        mapclone = map
        for y in range(0, test_count):
            key = "Key%d" % y
            value = "Value%d" % y
            value2 = "*Value%d*" % y
            self.assertTrue(map.contains_key(key))
            self.assertFalse(map.contains_value(value))
            self.assertTrue(map.contains_value(value2))

        # delete keys
        for y in range(0, test_count):
            key = "Key%d" % y
            map.delete(key)

if __name__ == '__main__':
    unittest.main(verbosity=2)
