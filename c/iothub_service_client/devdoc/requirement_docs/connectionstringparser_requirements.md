#connectionstringparser requirements
 
##Overview

connection_parser is module that parses key/value pairs from a connection string.
The format of the connection string is:
[key1]=[value1];[key2]=[value2]; …

##Exposed API

```c
extern MAP_HANDLE connectionstringparser_parse(STRING_HANDLE connection_string);
```

###connectionstringparser_parse

```c
extern MAP_HANDLE connectionstringparser_parse(STRING_HANDLE connection_string);
```

**SRS_CONNECTIONSTRINGPARSER_01_001: \[**connectionstringparser_parse shall parse all key value pairs from the connection_string passed in as argument and return a new map that holds the key/value pairs.**\]**
**SRS_CONNECTIONSTRINGPARSER_01_018: \[**If creating the result map fails, then connectionstringparser_parse shall return NULL.**\]** 
**SRS_CONNECTIONSTRINGPARSER_01_002: \[**If connection_string is NULL then connectionstringparser_parse shall fail and return NULL.**\]** 
**SRS_CONNECTIONSTRINGPARSER_01_003: \[**connectionstringparser_parse shall create a STRING tokenizer to be used for parsing the connection string, by calling STRING_TOKENIZER_create.**\]**
**SRS_CONNECTIONSTRINGPARSER_01_015: \[**If STRING_TOKENIZER_create fails, connectionstringparser_parse shall fail and return NULL.**\]** 
**SRS_CONNECTIONSTRINGPARSER_01_004: \[**connectionstringparser_parse shall start scanning at the beginning of the connection string.**\]** 
**SRS_CONNECTIONSTRINGPARSER_01_016: \[**2 STRINGs shall be allocated in order to hold the to be parsed key and value tokens.**\]**
**SRS_CONNECTIONSTRINGPARSER_01_017: \[**If allocating the STRINGs fails connectionstringparser_parse shall fail and return NULL.**\]** 
**SRS_CONNECTIONSTRINGPARSER_01_005: \[**The following actions shall be repeated until parsing is complete:**\]** 
- **SRS_CONNECTIONSTRINGPARSER_01_006: \[**connectionstringparser_parse shall find a token (the key of the key/value pair) delimited by the “=” character, by calling STRING_TOKENIZER_get_next_token.**\]** **SRS_CONNECTIONSTRINGPARSER_01_007: \[**If STRING_TOKENIZER_get_next_token fails, parsing shall be considered complete.**\]** 
- **SRS_CONNECTIONSTRINGPARSER_01_008: \[**connectionstringparser_parse shall find a token (the value of the key/value pair) delimited by the “;” character, by calling STRING_TOKENIZER_get_next_token.**\]** **SRS_CONNECTIONSTRINGPARSER_01_009: \[**If STRING_TOKENIZER_get_next_token fails, connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).**\]** 
- **SRS_CONNECTIONSTRINGPARSER_01_010: \[**The key and value shall be added to the result map by using Map_Add.**\]** **SRS_CONNECTIONSTRINGPARSER_01_011: \[**The C strings for the key and value shall be extracted from the previously parsed STRINGs by using STRING_c_str.**\]** **SRS_CONNECTIONSTRINGPARSER_01_019: \[**If the key length is zero then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).**\]** **SRS_CONNECTIONSTRINGPARSER_01_012: \[**If Map_Add fails connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).**\]** **SRS_CONNECTIONSTRINGPARSER_01_013: \[**If STRING_c_str fails then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).**\]** 
**SRS_CONNECTIONSTRINGPARSER_01_014: \[**After the parsing is complete the previously allocated STRINGs and STRING tokenizer shall be freed by calling STRING_TOKENIZER_destroy.**\]** 
