azure-iot-common.properties Requirements
========================================

Overview
--------

Properties is a collection of user defined properties. Values can only be
strings.

Public Interface
----------------

| **Member**                           | **Type**       | **Description**                                                |
|--------------------------------------|----------------|----------------------------------------------------------------|
| `Properties.add(itemKey, itemValue)` | void           | Adds the key-value pair to the collection.                     |
| `Properties.getItem(index)`          | Key/value Pair | Returns the key/value pair corresponding to the given index.   |
| `Properties.count`                   | Number         | Returns the number of items in the collection.                 |

Requirements
------------

### Properties.add(itemKey, itemValue)

**SRS_NODE_IOTHUB_PROPERTIES_07_003: [** The add function shall push the supplied `itemKey` and `itemValue` to the property object map.  **]**

**SRS_NODE_IOTHUB_PROPERTIES_07_004: [** If `itemKey` contains any of the reserved key names then the `add` function will return `false`. **]** 

### Properties.getItem(index)

**SRS_NODE_IOTHUB_PROPERTIES_07_001: [** If the supplied index is greater or equal to zero and is less than property map length, then it shall return the property object. **]**

**SRS_NODE_IOTHUB_PROPERTIES_13_001: [** If the supplied index is less than zero or greater than or equal to the property map length then it shall return `undefined`. **]**

### Properties.count()

**SRS_NODE_IOTHUB_PROPERTIES_07_002: [** Properties.Count shall return the number of items in the Properties map. **]**
