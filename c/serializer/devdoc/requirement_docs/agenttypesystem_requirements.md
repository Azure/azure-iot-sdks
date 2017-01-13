# AgentTypeSystem
 
## References:

![OData JSON Format Version 4.0. (2013, Nov 19).](http://docs.oasis-open.org/odata/odata-json-format/v4.0/odata-json-format-v4.0.html)

![OData Version 4.0 Part 3: Common Schema Definition Language (CSDL). (2013, Nov 19).](http://docs.oasis-open.org/odata/odata/v4.0/cos01/part3-csdl/odata-v4.0-cos01-part3-csdl.html)

![odata-abnf-construction-rules. (2013, Nov 17).](http://docs.oasis-open.org/odata/odata/v4.0/cs02/abnf/odata-abnf-construction-rules.txt)



**SRS_AGENT_TYPE_SYSTEM_99_001: [**  AGENT_TYPE_SYSTEM shall have the following interface **]**

```c
struct AGENT_DATA_TYPE_TAG;
typedef struct AGENT_DATA_TYPE_TAG AGENT_DATA_TYPE;

[…] /*implementation of AGENT_DATA_TYPE */ 
 
typedef enum AGENT_DATA_TYPES_RESULT_TAG
{
    AGENT_DATA_TYPES_OK,
    AGENT_DATA_TYPES_ERROR,
    AGENT_DATA_TYPES_INVALID_ARG,
    AGENT_DATA_TYPES_INVALID_NAME,
    AGENT_DATA_TYPES_INVALID_TYPE,
    AGENT_DATA_TYPES_NOT_IMPLEMENTED
} AGENT_DATA_TYPES_RESULT;

AGENT_DATA_TYPES_RESULT AgentDataTypes_ToString(char* destination,
	size_t destinationSize, const AGENT_DATA_TYPE* value);
 
/*Create/Destroy work in pairs. For some data type not calling Destroy might be ok. For some, it will lead to memory leaks*/
 
/*creates an AGENT_DATA_TYPE containing a EDM_BOOLEAN from a int*/
AGENT_DATA_TYPES_RESULT Create_EDM_BOOLEAN_from_int(AGENT_DATA_TYPE* agentData,
	int v);
 
/*creates an AGENT_DATA_TYPE containing a UINT8*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_UINT8(AGENT_DATA_TYPE* agentData,
	UINT8_T v);
 
/*creates an AGENT_DATA_TYPE containing a EDM_DATE_TIME without a timezone*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_time_t(
	AGENT_DATA_TYPE* agentData, time_t v);
 
/*creates an AGENT_DATA_TYPE containing a EDM_DATE_TIME  */
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(
	AGENT_DATA_TYPE* agentData, EDM_DATE_TIME_OFFSET v);

/*creates an AGENT_DATA_TYPE containing a EDM_GUID*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_EDM_GUID(
	AGENT_DATA_TYPE* agentData, EDM_GUID v);


/*creates an AGENT_DATA_TYPE containing a EDM_BINARY*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_EDM_BINARY(AGENT_DATA_TYPE* agentData, EDM_BINARY v);


/*creates an AGENT_DATA_TYPE containing a EDM_DATE */
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_date(
	AGENT_DATA_TYPE* agentData, SINT16_T year, UINT8_T month, UINT8_T day);

 
/*create an AGENT_DATA_TYPE containing an EDM_DECIMAL from a string*/
AGENT_DATA_TYPES_RESULT Create_EDM_DECIMAL_from_charz(
	AGENT_DATA_TYPE* agentData, const char* v);
 
/*create an AGENT_DATA_TYPE containing an EDM_DOUBLE from a DOUBLE_T*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_DOUBLE(
	AGENT_DATA_TYPE* agentData, DOUBLE_T v);
 
/*create an AGENT_DATA_TYPE from INT16_T*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT16(
	AGENT_DATA_TYPE* agentData, SINT16_T v);
 
/*create an AGENT_DATA_TYPE from INT32_T*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT32(
	AGENT_DATA_TYPE* agentData, SINT32_T v);
 
/*create an AGENT_DATA_TYPE from INT64_T*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT64(
	AGENT_DATA_TYPE* agentData,SINT64_T v);
 
/*create an AGENT_DATA_TYPE from SINT8_T*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT8(AGENT_DATA_TYPE* agentData,
	SINT8_T v);
 
/*create an AGENT_DATA_TYPE from SINGLE*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_FLOAT(AGENT_DATA_TYPE* agentData,
	FLOAT_T v);
 
/*create an AGENT_DATA_TYPE from ANSI zero terminated string*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_charz(AGENT_DATA_TYPE* agentData,
	char* v);

/*create an AGENT_DATA_TYPE from ANSI zero terminated string (no quotes)*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_charz_no_quotes(AGENT_DATA_TYPE* agentData,
	char* v);

/*create an AGENT_DATA_TYPE that holds a struct from its fields*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_Members(AGENT_DATA_TYPE* agentData,
	const char* typeName, size_t nMembers, const char* const * memberNames,
	AGENT_DATA_TYPE *memberValues); 

/*create an AGENT_DATA_TYPE that holds a structs from its fields*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_MemberPointers(AGENT_DATA_TYPE* agentData, const char* typeName, size_t nMembers, const char* const * memberNames, const AGENT_DATA_TYPE** memberPointerValues);

/*creates a copy of the AGENT_DATA_TYPE*/
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(
AGENT_DATA_TYPE* dest, const AGENT_DATA_TYPE* src);


void Destroy_AGENT_DATA_TYPE(AGENT_DATA_TYPE * agentData);

AGENT_DATA_TYPES_RESULT CreateAgentDataType_From_String (const char* source, AGENT_DATA_TYPE_TYPE type, AGENT_DATA_TYPE * agentData);
```


AGENT_DATA_TYPE is a Variant-like type that represents all the data types supported by the agent. A client (data provider) typically uses one of the Create… functions, passes the created data to the agent and finally uses Destroy_AGENT_DATA_TYPE on the created data.

**SRS_AGENT_TYPE_SYSTEM_99_013: [**  All the functions shall check their parameters for validity. When an invalid parameter is detected, the value AGENT_DATA_TYPES_INVALID_ARG shall be returned  **]**
**SRS_AGENT_TYPE_SYSTEM_99_014: [**  All functions shall return AGENT_DATA_TYPES_OK when the processing is successful. **]**


### AgentDataTypes_ToString
**SRS_AGENT_TYPE_SYSTEM_99_032: [**  This function supplies the JSON string representation of an AGENT_DATA_TYPE. **]**


**SRS_AGENT_TYPE_SYSTEM_99_015: [**  If destination parameter is NULL, AgentDataTypes_ToString shall return AGENT_DATA_TYPES_INVALID_ARG. **]**


**SRS_AGENT_TYPE_SYSTEM_99_053: [**  If value contains invalid data, AgentDataTypes_ToString shall return AGENT_DATA_TYPES_INVALID_ARG. **]**


**SRS_AGENT_TYPE_SYSTEM_99_016: [**  When the value cannot be converted to a string AgentDataTypes_ToString shall return AGENT_DATA_TYPES_ERROR. **]**


Depending on the type of EDM data represented, the JSON output takes the following forms:
**SRS_AGENT_TYPE_SYSTEM_99_017: [**  EDM_BOOLEAN: as in (odata-abnf-construction-rules, 2013), booleanValue = "true" / "false" **]**


**SRS_AGENT_TYPE_SYSTEM_99_018: [**  EDM_BYTE: as in (odata-abnf-construction-rules, 2013), byteValue  = 1*3DIGIT ; numbers in the range from 0 to 255 **]**


**SRS_AGENT_TYPE_SYSTEM_99_019: [**  EDM_DATETIMEOFFSET: dateTimeOffsetValue = year "-" month "-" day "T" hour ":" minute [ ":" second [ "." fractionalSeconds ] ( "Z" / sign hour ":" minute )] **]**
**SRS_AGENT_TYPE_SYSTEM_99_020: [**  EDM_DECIMAL: decimalValue = [SIGN 1*DIGIT ["." 1*DIGIT]] **]**
**SRS_AGENT_TYPE_SYSTEM_99_022: [**  EDM_DOUBLE: doubleValue = decimalValue [ "e" [SIGN 1*DIGIT ] / nanInfinity ; IEEE 754 binary64 floating-point number (15-17 decimal digits). The representation shall use DBL_DIG C #define]**]**
**SRS_AGENT_TYPE_SYSTEM_99_023: [**  EDM_INT16: int16Value = [ sign 1*5DIGIT  ; numbers in the range from -32768 to 32767] **]**
**SRS_AGENT_TYPE_SYSTEM_99_024: [**  EDM_INT32: int32Value = [ sign 1*10DIGIT ; numbers in the range from -2147483648 to 2147483647] **]**
**SRS_AGENT_TYPE_SYSTEM_99_025: [**  EDM_INT64: int64Value = [ sign 1*19DIGIT ; numbers in the range from -9223372036854775808 to 9223372036854775807] **]**
**SRS_AGENT_TYPE_SYSTEM_99_026: [**  EDM_SBYTE: sbyteValue = [ sign 1*3DIGIT  ; numbers in the range from -128 to 127] **]**
**SRS_AGENT_TYPE_SYSTEM_99_027: [**  EDM_SINGLE: singleValue = doubleValue ; IEEE 754 binary32 floating-point number (6-9 decimal digits). The representatiuon shall use FLT_DIG. **]**
**SRS_AGENT_TYPE_SYSTEM_99_068: [**  EDM_DATE: dateValue = year "-" month "-" day. **]**
**SRS_AGENT_TYPE_SYSTEM_99_028: [**  EDM_STRING: string           = SQUOTE *( SQUOTE-in-string / pchar-no-SQUOTE ) SQUOTE **]**
**SRS_AGENT_TYPE_SYSTEM_01_003: [** EDM_STRING_no_quotes: the string is copied as given when the AGENT_DATA_TYPE was created. **]**
**SRS_AGENT_TYPE_SYSTEM_99_093: [**  EDM_GUID: 8HEXDIG "-" 4HEXDIG "-" 4HEXDIG "-" 4HEXDIG "-" 12HEXDIG **]**
**SRS_AGENT_TYPE_SYSTEM_99_099: [**  EDM_BINARY: = *(4base64char) [ base64b16  / base64b8 ]
base64char  = ALPHA / DIGIT / "-" / "_"
base64b16   = 2base64char ( 'A' / 'E' / 'I' / 'M' / 'Q' / 'U' / 'Y' / 'c' / 'g' / 'k' / 'o' / 's' / 'w' / '0' / '4' / '8' )   [ "=" ]
base64b8    = base64char ( 'A' / 'Q' / 'g' / 'w' ) [ "==" ]
ALPHA  = %x41-5A / %x61-7A
DIGIT  = %x30-39
**]**
SRS_AGENT_TYPE_SYSTEM_99_062:[ If the AGENT_DATA_TYPE represents a "complex type", then the JSON marshaller shall produce the following JSON value:
{
	memberName1: memberValue1,
	memberName2: memberValue2,
	…
	memberNamen: memberValuen
}, where "n" is the same "n" as in "nMembers" parameter passed to Create_AGENT_DATA_TYPE_from_Members].
**SRS_AGENT_TYPE_SYSTEM_99_101: [**  EDM_NULL_TYPE shall return the unquoted string null. **]**

### Create_EDM_BOOLEAN_from_int
**SRS_AGENT_TYPE_SYSTEM_99_031: [**  Creates a AGENT_DATA_TYPE representing an EDM_BOOLEAN. **]**
**SRS_AGENT_TYPE_SYSTEM_99_029: [**  If v is  0 then the AGENT_DATA_TYPE shall have the value "false" Boolean. **]**
**SRS_AGENT_TYPE_SYSTEM_99_030: [**  If v is different than 0 then the AGENT_DATA_TYPE shall have the value "true". **]**

### Create_AGENT_DATA_TYPE_from_UINT8
**SRS_AGENT_TYPE_SYSTEM_99_033: [**  Creates an AGENT_DATA_TYPE from a UINT8_T. **]**

### Create_AGENT_DATA_TYPE_from_time_t
**SRS_AGENT_TYPE_SYSTEM_99_034: [**  Creates an AGENT_DATA_TYPE containing an EDM_DATETIMEOFFSET from a time_t. **]**
**SRS_AGENT_TYPE_SYSTEM_99_035: [**  If time_t has leap seconds or invalid data, the return shall be AGENT_DATA_TYPES_INVALID_ARG. **]**

### Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET
**SRS_AGENT_TYPE_SYSTEM_99_091: [**  Creates an AGENT_DATA_TYPE containing an Edm.DateTimeOffset from an EDM_DATE_TIME_OFFSET. **]**
**SRS_AGENT_TYPE_SYSTEM_99_092: [**  The structure shall be validated to be conforming to OData specifications (odata-abnf-construction-rules, 2013), and if found invalid, AGENT_DATA_TYPES_INVALID_ARG shall be returned. **]**

### Create_AGENT_DATA_TYPE_from_date
**SRS_AGENT_TYPE_SYSTEM_99_069: [**  Creates an AGENT_DATA_TYPE containing an EDM_DATE from a year, a month and a day of the month. **]**
**SRS_AGENT_TYPE_SYSTEM_99_070: [**  If year-month-date does not indicate a valid day (for example 31 Feb 2013), then AGENT_DATA_TYPES_INVALID_ARG shall be returned. **]**

### Create_EDM_DECIMAL_from_charz
```c
AGENT_DATA_TYPES_RESULT Create_EDM_DECIMAL_from_charz(AGENT_DATA_TYPE* agentData, const char* v);
```

**SRS_AGENT_TYPE_SYSTEM_99_039: [**  Creates an AGENT_DATA_TYPE containing an EDM_DECIMAL from a null-terminated string. **]**
**SRS_AGENT_TYPE_SYSTEM_99_067: [**  If the string indicated by the parameter v does not match exactly an ODATA string representation, AGENT_DATA_TYPES_INVALID_ARG shall be returned. **]**

### Create_AGENT_DATA_TYPE_from_DOUBLE
**SRS_AGENT_TYPE_SYSTEM_99_041: [**  Creates an AGENT_DATA_TYPE containing an EDM_DOUBLE from DOUBLE_T **]**
**SRS_AGENT_TYPE_SYSTEM_99_042: [**  Values of NaN, -INF, +INF are accepted **]**
### Create_AGENT_DATA_TYPE_from_SINT16

**SRS_AGENT_TYPE_SYSTEM_99_043: [**  Creates an AGENT_DATA_TYPE containing an EDM_INT16 from SINT16_T **]**

### Create_AGENT_DATA_TYPE_from_SINT32
**SRS_AGENT_TYPE_SYSTEM_99_044: [**  Creates an AGENT_DATA_TYPE containing an EDM_INT32 from SINT32_T **]**

### Create_AGENT_DATA_TYPE_from_SINT64
**SRS_AGENT_TYPE_SYSTEM_99_045: [**  Creates an AGENT_DATA_TYPE containing an EDM_INT64 from SINT64_T **]**

### Create_AGENT_DATA_TYPE_from_SINT8
**SRS_AGENT_TYPE_SYSTEM_99_046: [**  Creates an AGENT_DATA_TYPE containing an EDM_SBYTE from SINT8_T **]**

### Create_AGENT_DATA_TYPE_from_FLOAT
**SRS_AGENT_TYPE_SYSTEM_99_047: [**  Creates an AGENT_DATA_TYPE containing an EDM_SINGLE from FLOAT_T **]**
**SRS_AGENT_TYPE_SYSTEM_99_048: [**  Value of NaN, +Inf, -Inf are accepted. **]**

### Create_AGENT_DATA_TYPE_from_charz
**SRS_AGENT_TYPE_SYSTEM_99_049: [**  Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string. **]**

### Create_AGENT_DATA_TYPE_from_charz_no_quotes
**SRS_AGENT_TYPE_SYSTEM_01_001: [** Creates an AGENT_DATA_TYPE containing an EDM_STRING from an ASCII zero terminated string. **]**
**SRS_AGENT_TYPE_SYSTEM_01_002: [** When serialized, this type is not enclosed with quotes. **]**

### Create_AGENT_DATA_TYPE_from_EDM_GUID
**SRS_AGENT_TYPE_SYSTEM_99_094: [**  Creates an AGENT_DATA_TYPE containing a EDM_GUID from an EDM_GUID **]**

### Create_AGENT_DATA_TYPE_from_EDM_BINARY
**SRS_AGENT_TYPE_SYSTEM_99_098: [**  Creates an AGENT_DATA_TYPE containing a EDM_BINARY from a EDM_BINARY. **]**

### Create_NULL_AGENT_DATA_TYPE
**SRS_AGENT_TYPE_SYSTEM_99_103: [**  Creates an AGENT_DATA_TYPE of type EDM_NULL. **]**

### Create_AGENT_DATA_TYPE_from_Members
**SRS_AGENT_TYPE_SYSTEM_99_054: [**  Using this API shall create an AGENT_DATA_TYPE containing a struct representation. **]**
**SRS_AGENT_TYPE_SYSTEM_99_055: [**  If typeName is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG . **]**
**SRS_AGENT_TYPE_SYSTEM_99_056: [**  If nMembers is 0, the function shall return AGENT_DATA_TYPES_INVALID_ARG . **]**
**SRS_AGENT_TYPE_SYSTEM_99_057: [**  If memberNames is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG . **]**
**SRS_AGENT_TYPE_SYSTEM_99_058: [**  If any of the memberNames[i] is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG .] **]**
**SRS_AGENT_TYPE_SYSTEM_99_059: [**  If memberValues is NULL, the function shall return AGENT_DATA_TYPES_INVALID_ARG . **]**
**SRS_AGENT_TYPE_SYSTEM_99_063: [**  If there are two memberNames with the same name, then the function shall return  AGENT_DATA_TYPES_INVALID_ARG. **]**

### Create_AGENT_DATA_TYPE_from_MemberPointers
**SRS_AGENT_TYPE_SYSTEM_99_108: [**  This API shall create a complex AGENT_DATA_TYPE from pointers to AGENT_DATA_TYPE fields. **]**
**SRS_AGENT_TYPE_SYSTEM_99_109: [**  AGENT_DATA_TYPES_INVALID_ARG shall be returned if memberPointerValues parameter is NULL. **]**
**SRS_AGENT_TYPE_SYSTEM_99_110: [**  AGENT_DATA_TYPES_ERROR shall be returned for all other errors.  **]**

### Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE
**SRS_AGENT_TYPE_SYSTEM_99_065: [** Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall clone the value of an existing agent data. **]**
**SRS_AGENT_TYPE_SYSTEM_99_066: [** On success Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPE_OK. **]**
**SRS_AGENT_TYPE_SYSTEM_99_064: [** If any argument is NULL Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE shall return AGENT_DATA_TYPES_INVALID_ARG. **]**

### Destroy_AGENT_DATA_TYPE
**SRS_AGENT_TYPE_SYSTEM_99_050: [**  Destroy_AGENT_DATA_TYPE shall deallocate all allocated resources used to represent the type. **]**
**SRS_AGENT_TYPE_SYSTEM_99_051: [**  After it is called and successfully finishes, the agentData shall contain EDM_NO_TYPE. **]**

### CreateAgentDataType_From_String
```c 
AGENT_DATA_TYPES_RESULT CreateAgentDataType_From_String (const char* source, AGENT_DATA_TYPE_TYPE type, AGENT_DATA_TYPE * agentData);
```

**SRS_AGENT_TYPE_SYSTEM_99_071: [**  CreateAgentDataType_From_String shall create an AGENT_DATA_TYPE from a char* representation of the type indicated by type parameter. **]**
**SRS_AGENT_TYPE_SYSTEM_99_072: [**  The implementation for the transformation of the char* source into AGENT_DATA_TYPE shall be type specific. **]**
**SRS_AGENT_TYPE_SYSTEM_99_073: [**  CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is NULL. **]**
**SRS_AGENT_TYPE_SYSTEM_99_074: [**  CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if agentData is NULL. **]**
**SRS_AGENT_TYPE_SYSTEM_99_075: [**  CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_NOT_IMPLEMENTED if type is not a supported type. **]**

The supported values for the type parameter are:
**SRS_AGENT_TYPE_SYSTEM_99_076: [**  EDM_BOOLEAN_TYPE **]**
**SRS_AGENT_TYPE_SYSTEM_99_077: [**  EDM_BYTE **]**
**SRS_AGENT_TYPE_SYSTEM_99_078: [**  EDM_DATETIMEOFFSET **]**
**SRS_AGENT_TYPE_SYSTEM_99_079: [**  EDM_DECIMAL **]**
**SRS_AGENT_TYPE_SYSTEM_99_080: [**  EDM_DOUBLE **]**
**SRS_AGENT_TYPE_SYSTEM_99_089: [** EDM_SINGLE **]**
**SRS_AGENT_TYPE_SYSTEM_99_081: [**  EDM_INT16 **]**
**SRS_AGENT_TYPE_SYSTEM_99_082: [**  EDM_INT32 **]**
**SRS_AGENT_TYPE_SYSTEM_99_083: [**  EDM_INT64 **]**
**SRS_AGENT_TYPE_SYSTEM_99_084: [**  EDM_SBYTE **]**
**SRS_AGENT_TYPE_SYSTEM_99_085: [**  EDM_DATE **]**
**SRS_AGENT_TYPE_SYSTEM_99_086: [**  EDM_STRING **]**
**SRS_AGENT_TYPE_SYSTEM_01_004: [** EDM_STRING_NO_QUOTES **]**
**SRS_AGENT_TYPE_SYSTEM_99_097: [**  EDM_GUID **]**
**SRS_AGENT_TYPE_SYSTEM_99_100: [**  EDM_BINARY **]**
**SRS_AGENT_TYPE_SYSTEM_99_102: [**  EDM_NULL_TYPE **]**
**SRS_AGENT_TYPE_SYSTEM_99_087: [**  CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_INVALID_ARG if source is not a valid string for a value of type type. **]**
**SRS_AGENT_TYPE_SYSTEM_99_088: [**  CreateAgentDataType_From_String shall return AGENT_DATA_TYPES_ERROR if any other error occurs. **]**