# JSON encoder

## Overview
JSON encoder is a module that produces JSON objects from a multi-tree given as input in the context of an internal handle.
Each node value shall be converted to string by using a conversion function that can be supplied to the JSON encoder.

Example 1.
From a tree with only one node, containing a name/value of "Truck":"42" the following JSON object shall be created:
```json
{
	"Truck":42
}
```
Example 2.
From a tree with 2 nodes, containing a name/value of : "A": "\"someString\"" and name/value "BBB":"23" the following JSON object shall be created:
```json
{
	"A":"someString",
	"BBB": 23
}
```
Example 3.
 
The above tree shall produce the following JSON object:
```json
{
	"Child1":"a",
	"Child2":"b",
	"Child3":
    {
        "Child4":"c",
        "Child5":"d"
    }
}
```
## Public API
**SRS_JSON_ENCODER_99_029:[**
```c
typedef enum JSON_ENCODER_RESULT_TAG
{
    JSON_ENCODER_OK,
    JSON_ENCODER_INVALID_ARG,
    JSON_ENCODER_MULTITREE_ERROR,
    JSON_ENCODER_TOSTRING_FUNCTION_ERROR,
    JSON_ENCODER_ERROR
} JSON_ENCODER_RESULT;
 
 
typedef enum JSON_ENCODER_TOSTRING_RESULT_TAG
{
    JSON_ENCODER_TOSTRING_OK,
    JSON_ENCODER_TOSTRING_INVALID_ARG,
    JSON_ENCODER_TOSTRING_ERROR
} JSON_ENCODER_TOSTRING_RESULT;

typedef JSON_ENCODER_TOSTRING_RESULT(*JSON_ENCODER_TOSTRING_FUNC)(char* destination,
size_t destinationSize, const void* value);

extern JSON_ENCODER_TOSTRING_RESULT JSONEncoder_CharPtr_ToString(char* destination,
size_t destinationSize, const void* value);
extern JSON_ENCODER_RESULT JSONEncoder_EncodeTree(MULTITREE_HANDLE treeHandle,
    char* buffer, size_t* byteCount, JSON_ENCODER_TOSTRING_FUNC toStringFunc);]
```
**]**

### JSONEncoder_EncodeTree

**SRS_JSON_ENCODER_99_030: [** JSONEncoder_EncodeTree shall produce a string containing the JSON object and store that string in the buffer argument. **]**

**SRS_JSON_ENCODER_99_031: [** On success, JSONEncoder_EncodeTree shall return JSON_ENCODER_OK. **]**

**SRS_JSON_ENCODER_99_032: [** If any of the arguments passed to JSONEncoder_EncodeTree is NULL then JSON_ENCODER_INVALID_ARG shall be returned. **]**

**SRS_JSON_ENCODER_99_033: [** If any MultiTree function call fails JSONEncoder_EncodeTree shall return JSON_ENCODER_MULTITREE_ERROR. **]**

JSON encoder shall take the following steps in order to produce a JSON representation of the tree given:
**SRS_JSON_ENCODER_99_035: [**  JSON encoder shall inquire the number of child nodes (further called childCount) of the current node (given by parameter treeHandle. **]**

The following actions shall take place:
**SRS_JSON_ENCODER_99_036: [**  The string "{" shall be added to the output; **]**

**SRS_JSON_ENCODER_99_037: [**  For every child, the following actions shall take place in order to produce name:value **]**
:
The name part of the JSON name:value shall be constructed like the following:
**SRS_JSON_ENCODER_99_038: [**  A "\"" (single quote) shall be added to the output. **]**

**SRS_JSON_ENCODER_99_039: [**  The child name shall be retrieved by a call to MultiTree_GetChild followed by a MultiTree_GetName. **]**

**SRS_JSON_ENCODER_99_040: [**  After retrieval, it shall be added to the output. **]**

**SRS_JSON_ENCODER_99_041: [**  A "\":" (single quote followed by colon) shall be added to the output. **]**

The value part of the of the JSON name:value shall be constructed like the following:
**SRS_JSON_ENCODER_99_042: [**  If the child has zero children, then the function toStringFunc shall be invoked for the value (as computed by MultiTree_GetValue) of that child **]**

**SRS_JSON_ENCODER_99_043: [**  If the child has children, then JSONEncoder_EncodeTree  shall be invoked for every of these children. **]**

**SRS_JSON_ENCODER_99_044: [**  A "," shall be added for every child, except the last one. **]**

**SRS_JSON_ENCODER_99_045: [**  The string "}" shall be added to the output **]**

**SRS_JSON_ENCODER_99_046: [**  If any other error occurs during the construction of the output, JSON_ENCODER_ERROR shall be returned. **]**

### JSONEncoder_CharPtr_ToString

JSONEncoder_CharPtr_ToString is a predefined function that should be passed to JSONEncoder_EncodeTree when the tree stores char* data.

**SRS_JSON_ENCODER_99_047: [**  JSONEncoder_CharPtr_ToString shall return JSON_ENCODER_TOSTRING_INVALID_ARG if destination or value parameters passed to it are NULL. **]**


**SRS_JSON_ENCODER_99_048: [**  JSONEncoder_CharPtr_ToString shall use strcpy_s to copy from value to destination. **]**


**SRS_JSON_ENCODER_99_049: [**  If strcpy_s fails then JSONEncoder_CharPtr_ToString shall return JSON_ENCODER_TOSTRING_ERROR. **]**


**SRS_JSON_ENCODER_99_050: [**  If strcpy_s doesn't fail, then JSONEncoder_CharPtr_ToString shall return JSON_ENCODER_TOSTRING_OK **]**




