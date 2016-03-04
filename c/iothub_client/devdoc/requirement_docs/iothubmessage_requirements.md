#IoTHubMessage Requirements

##Overview
The IoTHub_Message component is encapsulating one message that can be transferred by an IoT hub client.
References
[iothubclient_c_library](../iothubclient_c_library.docx)

##Exposed API

```c
#define IOTHUBMESSAGE_CONTENT_TYPE_VALUES \
IOTHUBMESSAGE_BYTEARRAY, \
IOTHUBMESSAGE_STRING, \
IOTHUBMESSAGE_UNKNOWN \
 
DEFINE_ENUM(IOTHUBMESSAGE_CONTENT_TYPE, IOTHUBMESSAGE_CONTENT_TYPE_VALUES);
 
typedef void* IOTHUB_MESSAGE_HANDLE;
 
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromByteArray(const unsigned char* byteArray, size_t size);
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromString(const char* source);
 
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_Clone(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
 
extern IOTHUB_MESSAGE_RESULT
IoTHubMessage_GetByteArray(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const unsigned char** buffer, size_t* size);
extern const char* IoTHubMessage_GetString(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
extern IOTHUBMESSAGE_CONTENT_TYPE IoTHubMessage_GetContentType(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
extern MAP_HANDLE IoTHubMessage_Properties(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
extern IOTHUB_MESSAGE_RESULT
IoTHubMessage_SetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const char* messageId);
extern const char* IoTHubMessage_GetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

extern IOTHUB_MESSAGE_RESULT
IoTHubMessage_SetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const char* correlationId);
extern const char* IoTHubMessage_GetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
 
extern void IoTHubMessage_Destroy(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
```

##IoTHubMessage_CreateFromByteArray 
```c
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromByteArray(const unsigned char* byteArray, size_t size);
```
IoTHubMessage_CreateFromByteArray creates a new IoTHubMessage from a byte array.
**SRS_IOTHUBMESSAGE_06_001: [**If size is zero then byteArray may be NULL.**]**   
**SRS_IOTHUBMESSAGE_06_002: [**If size is NOT zero then byteArray MUST NOT be NULL.**]** 
**SRS_IOTHUBMESSAGE_02_022: [**IoTHubMessage_CreateFromByteArray shall call BUFFER_create passing byteArray and size as parameters.**]** 
**SRS_IOTHUBMESSAGE_02_023: [**IoTHubMessage_CreateFromByteArray shall call Map_Create to create the message properties.**]** 
**SRS_IOTHUBMESSAGE_02_024: [**If there are any errors then IoTHubMessage_CreateFromByteArray shall return NULL.**]** 
**SRS_IOTHUBMESSAGE_02_025: [**Otherwise, IoTHubMessage_CreateFromByteArray shall return a non-NULL handle.**]** 
**SRS_IOTHUBMESSAGE_02_026: [**The type of the new message shall be IOTHUBMESSAGE_BYTEARRAY.**]** 

##IoTHubMessage_CreateFromString
```c
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromString(const char* source);
```
IoTHubMessage_CreateFromString creates a new IoTHubMessage from a null terminated string.
**SRS_IOTHUBMESSAGE_02_027: [**IoTHubMessage_CreateFromString shall call STRING_construct passing source as parameter.**]** 
**SRS_IOTHUBMESSAGE_02_028: [**IoTHubMessage_CreateFromString shall call Map_Create to create the message properties.**]** 
**SRS_IOTHUBMESSAGE_02_029: [**If there are any encountered in the execution of IoTHubMessage_CreateFromString then IoTHubMessage_CreateFromString shall return NULL.**]** 
**SRS_IOTHUBMESSAGE_02_031: [**Otherwise, IoTHubMessage_CreateFromString shall return a non-NULL handle.**]** 
**SRS_IOTHUBMESSAGE_02_032: [**The type of the new message shall be IOTHUBMESSAGE_STRING.**]** 

##IoTHubMessage_Destroy
```c
extern void IoTHubMessage_Destroy(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
```
**SRS_IOTHUBMESSAGE_01_003: [**IoTHubMessage_Destroy shall free all resources associated with iotHubMessageHandle.**]**  
**SRS_IOTHUBMESSAGE_01_004: [**If iotHubMessageHandle is NULL, IoTHubMessage_Destroy shall do nothing.**]** 

##IoTHubMessage_GetByteArray
```c
extern IOTHUB_MESSAGE_RESULT
```c
IoTHubMessage_GetByteArray(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const unsigned char** buffer, size_t* size);
```
IoTHubMessage_GetByteArray provides a pointer and size for the data associated with the IoT hub message handle. 
**SRS_IOTHUBMESSAGE_01_011: [**The pointer shall be obtained by using BUFFER_u_char and it shall be copied in the buffer argument.**]** 
**SRS_IOTHUBMESSAGE_01_012: [**The size of the associated data shall be obtained by using BUFFER_length and it shall be copied to the size argument.**]** 
**SRS_IOTHUBMESSAGE_01_014: [**If any of the arguments passed to IoTHubMessage_GetByteArray  is NULL IoTHubMessage_GetByteArray shall return IOTHUBMESSAGE_INVALID_ARG.**]** 
**SRS_IOTHUBMESSAGE_02_021: [**If iotHubMessageHandle is not a iothubmessage containing BYTEARRAY data, then IoTHubMessage_GetByteArray  shall return IOTHUBMESSAGE_INVALID_ARG.**]**
**SRS_IOTHUBMESSAGE_02_033: [**IoTHubMessage_GetByteArray shall return IOTHUBMESSAGE_OK when all oeprations complete succesfully.**]** 

##IoTHubMessage_Clone
```c
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_Clone(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
```
**SRS_IOTHUBMESSAGE_03_001: [**IoTHubMessage_Clone shall create a new IoT hub message with data content identical to that of the iotHubMessageHandle parameter.**]**
**SRS_IOTHUBMESSAGE_03_005: [**IoTHubMessage_Clone shall return NULL if iotHubMessageHandle is NULL.**]**
**SRS_IOTHUBMESSAGE_02_006: [**IoTHubMessage_Clone shall clone the content by a call to BUFFER_clone or STRING_clone**]** 
**SRS_IOTHUBMESSAGE_02_005: [**IoTHubMessage_Clone shall clone the properties map by using Map_Clone.**]** 
**SRS_IOTHUBMESSAGE_03_002: [**IoTHubMessage_Clone shall return upon success a non-NULL handle to the newly created IoT hub message.**]**
**SRS_IOTHUBMESSAGE_03_004: [**IoTHubMessage_Clone shall return NULL if it fails for any reason.**]**

##IoTHubMessage_Properties
```c
extern MAP_HANDLE IoTHubMessage_Properties(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
```

IoTHubMessage_Properties exposes the storage of the message properties.
**SRS_IOTHUBMESSAGE_02_001: [**If iotHubMessageHandle is NULL then IoTHubMessage_Properties shall return NULL.**]** 
**SRS_IOTHUBMESSAGE_02_002: [**Otherwise, for any non-NULL iotHubMessageHandle it shall return a non-NULL MAP_HANDLE.**]** 
**SRS_IOTHUBMESSAGE_07_008: [**ValidateAsciiCharactersFilter shall loop through the mapKey and mapValue strings to ensure that they only contain valid US-Ascii characters Ascii value 32 - 126.**]** 

##IoTHubMessage_GetContentType
```c
extern IOTHUBMESSAGE_CONTENT_TYPE IoTHubMessage_GetContentType(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
```
IoTHubMessage_GetContentType returns the type of the message given by parameter iotHubMessageHandle.
**SRS_IOTHUBMESSAGE_02_008: [**If any parameter is NULL then IoTHubMessage_GetContentType shall return IOTHUBMESSAGE_UNKNOWN.**]** 
**SRS_IOTHUBMESSAGE_02_009: [**Otherwise IoTHubMessage_GetContentType shall return the type of the message.**]** 

##IoTHubMessage_GetString
```c
extern const char* IoTHubMessage_GetString(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
```
IoTHubMessage_GetString returns the stored null terminated string.
**SRS_IOTHUBMESSAGE_02_016: [**If any parameter is NULL then IoTHubMessage_GetString  shall return NULL.**]** 
**SRS_IOTHUBMESSAGE_02_017: [**IoTHubMessage_GetString shall return NULL if the iotHubMessageHandle does not refer to a IOTHUBMESSAGE of type STRING.**]** 
**SRS_IOTHUBMESSAGE_02_018: [**IoTHubMessage_GetStringData shall return the currently stored null terminated string.**]** 

##IoTHubMessage_GetMessageId
```c 
extern const char * IoTHubMessage_GetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
```
**SRS_IOTHUBMESSAGE_07_010: [**if the iotHubMessageHandle parameter is NULL then IoTHubMessage_GetMessageId shall return a NULL value.**]** 
**SRS_IOTHUBMESSAGE_07_011: [**IoTHubMessage_GetMessageId shall return the messageId as a const char*.**]** 

##IoTHubMessage_SetMessageId
```c
extern IOTHUB_MESSAGE_RESULT IoTHubMessage_SetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const char* messageId);
```
**SRS_IOTHUBMESSAGE_07_012: [**if any of the parameters are NULL then IoTHubMessage_SetMessageId shall return a IOTHUB_MESSAGE_INVALID_ARG value.**]** 
**SRS_IOTHUBMESSAGE_07_013: [**If the IOTHUB_MESSAGE_HANDLE messageId is not NULL, then the IOTHUB_MESSAGE_HANDLE messageId will be deallocated.**]** 
**SRS_IOTHUBMESSAGE_07_014: [**If the allocation or the copying of the messageId fails, then IoTHubMessage_SetMessageId shall return IOTHUB_MESSAGE_ERROR.**]** 
**SRS_IOTHUBMESSAGE_07_015: [**IoTHubMessage_SetMessageId finishes successfully it shall return IOTHUB_MESSAGE_OK.**]**

##IoTHubMessage_GetCorrelationId
```c
extern const char * IoTHubMessage_GetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
```
**SRS_IOTHUBMESSAGE_07_016: [**if the iotHubMessageHandle parameter is NULL then IoTHubMessage_GetCorrelationId shall return a NULL value.**]** 
**SRS_IOTHUBMESSAGE_07_017: [**IoTHubMessage_GetCorrelationId shall return the correlationId as a const char*.**]** 

##IoTHubMessage_SetCorrelationId
```c
extern IOTHUB_MESSAGE_RESULT IoTHubMessage_SetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const char* correlationId);
```
**SRS_IOTHUBMESSAGE_07_018: [**if any of the parameters are NULL then IoTHubMessage_SetCorrelationId shall return a IOTHUB_MESSAGE_INVALID_ARG value.**]** 
**SRS_IOTHUBMESSAGE_07_019: [**If the IOTHUB_MESSAGE_HANDLE correlationId is not NULL, then the IOTHUB_MESSAGE_HANDLE correlationId will be deallocated.**]** 
**SRS_IOTHUBMESSAGE_07_020: [**If the allocation or the copying of the correlationId fails, then IoTHubMessage_SetCorrelationId shall return IOTHUB_MESSAGE_ERROR.**]** 
**SRS_IOTHUBMESSAGE_07_021: [**IoTHubMessage_SetCorrelationId finishes successfully it shall return IOTHUB_MESSAGE_OK.**]** 

