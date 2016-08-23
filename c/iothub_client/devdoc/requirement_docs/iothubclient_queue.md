# IoTHubClient_queue Requirements

## Overview

IoTHubClient_queue enables you to queue messages to the iothub client.

Exposed API

```c
typedef struct IOTHUB_QUEUE_DATA_TAG* IOTHUB_QUEUE_HANDLE;

#define IOTHUB_QUEUE_RESULT_VALUES     \
    IOTHUB_QUEUE_OK,                   \
    IOTHUB_QUEUE_INVALID_ARG,          \
    IOTHUB_QUEUE_QUEUE_EMPTY,          \
    IOTHUB_QUEUE_ERROR                 \

DEFINE_ENUM(IOTHUB_QUEUE_RESULT, IOTHUB_QUEUE_RESULT_VALUES);

typedef void(*IOTHUB_QUEUE_DESTROY_CALLBACK)(void* queue_item);

MOCKABLE_FUNCTION(, IOTHUB_QUEUE_HANDLE, IoTHubQueue_Create_Queue);
MOCKABLE_FUNCTION(, void, IoTHubQueue_Destroy_Queue, IOTHUB_QUEUE_HANDLE, handle, IOTHUB_QUEUE_DESTROY_CALLBACK, destroy_callback);
MOCKABLE_FUNCTION(, IOTHUB_QUEUE_RESULT, IoTHubQueue_Add_Item, IOTHUB_QUEUE_HANDLE, handle, void*, client_queue_item);
MOCKABLE_FUNCTION(, const void*, IoTHubQueue_Get_Queue_Item, IOTHUB_QUEUE_HANDLE, handle);
MOCKABLE_FUNCTION(, IOTHUB_QUEUE_RESULT, IoTHubQueue_Remove_Next_Item, IOTHUB_QUEUE_HANDLE, handle, IOTHUB_QUEUE_DESTROY_CALLBACK, destroy_callback);
MOCKABLE_FUNCTION(, IOTHUB_QUEUE_ENUM_HANDLE, IoTHubQueue_Enum_Queue, IOTHUB_QUEUE_HANDLE, handle);
MOCKABLE_FUNCTION(, const void*, IoTHubQueue_Enum_Next_Item, IOTHUB_QUEUE_ENUM_HANDLE, enum_handle);
MOCKABLE_FUNCTION(, void, IoTHubQueue_Enum_Close, IOTHUB_QUEUE_ENUM_HANDLE, enum_handle);
```

### IoTHubQueue_Create_Queue

```c
extern IOTHUB_QUEUE_HANDLE IoTHubQueue_Create_Queue();
```

**SRS_IOTHUB_QUEUE_07_001: [**If an error is encountered IoTHubQueue_Create_Queue shall return NULL.**]**  
**SRS_IOTHUB_QUEUE_07_002: [**On Success IoTHubQueue_Create_Queue shall return a non-NULL handle that refers to a Queue.**]**  

### IoTHubQueue_Destroy_Queue

```c
extern void IoTHubQueue_Destroy_Queue(IOTHUB_QUEUE_HANDLE handle, IOTHUB_QUEUE_DESTROY_CALLBACK, destroy_callback);
```

**SRS_IOTHUB_QUEUE_07_003: [**If `handle` is NULL then IoTHubQueue_Destroy_Queue shall do nothing.**]**  
**SRS_IOTHUB_QUEUE_07_004: [**IoTHubQueue_Destroy_Queue shall only free memory allocated within this compilation unit.**]**  
**SRS_IOTHUB_QUEUE_07_005: [**If destroy_callback is not NULL, IoTHubQueue_Destroy_Queue shall call destroy_callback with the item to be deleted.**]**  

### IoTHubQueue_Add_Item

```c
extern IOTHUB_QUEUE_RESULT IoTHubQueue_Add_Item(IOTHUB_QUEUE_HANDLE handle, void* queue_item);
```

**SRS_IOTHUB_QUEUE_07_006: [**If `handle` or `queue_item` are NULL IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_INVALID_ARG.**]**  
**SRS_IOTHUB_QUEUE_07_007: [**IoTHubQueue_Add_Item shall allocate an queue item and store the item into the queue.**]**  
**SRS_IOTHUB_QUEUE_07_008: [**On success IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_OK.**]**  
**SRS_IOTHUB_QUEUE_07_009: [**If any error is encountered IoTHubQueue_Add_Item shall return IOTHUB_QUEUE_ERROR.**]**  

### IoTHubQueue_Next_In_Queue

```c
extern const void* IoTHubQueue_Get_Queue_Item(IOTHUB_QUEUE_HANDLE handle);
```

**SRS_IOTHUB_QUEUE_07_010: [**If `handle` is NULL IoTHubQueue_Get_Queue_Item shall return NULL.**]**  
**SRS_IOTHUB_QUEUE_07_011: [**If the queue is empty, IoTHubQueue_Get_Queue_Item shall return NULL.**]**  
**SRS_IOTHUB_QUEUE_07_012: [**If the void is retrieved from the queue IoTHubQueue_Get_Queue_Item shall return a const void.**]**  


### IoTHubQueue_Remove_Next_Item

```c
extern IOTHUB_QUEUE_RESULT IoTHubQueue_Remove_Next_Item(IOTHUB_QUEUE_HANDLE handle, IOTHUB_QUEUE_DESTROY_CALLBACK destroy_callback);
```

**SRS_IOTHUB_QUEUE_07_013: [**If `handle` is NULL IoTHubQueue_Remove_Next_Item shall return IOTHUB_QUEUE_INVALID_ARG.**]**  
**SRS_IOTHUB_QUEUE_07_014: [**If the queue is empty, IoTHubQueue_Remove_Next_Item shall return IOTHUB_QUEUE_QUEUE_EMPTY.**]**  
**SRS_IOTHUB_QUEUE_07_015: [**If the Item is successfully Removed from the queue and `destroy_callback` is not NULL, IoTHubQueue_Remove_Next_Item will call `destroy_callback` and deallocate the item.**]**  
**SRS_IOTHUB_QUEUE_07_016: [**On success IoTHubQueue_Remove_Next_Item shall return IOTHUB_QUEUE_OK.**]**  

### IoTHubQueue_Enum_Queue

```c
IOTHUB_QUEUE_ENUM_HANDLE IoTHubQueue_Enum_Queue(IOTHUB_QUEUE_HANDLE handle)
```

**SRS_IOTHUB_QUEUE_07_017: [**If `handle` is NULL IoTHubQueue_Enum_Queue shall return NULL.**]**  
**SRS_IOTHUB_QUEUE_07_018: [**IoTHubQueue_Enum_Queue shall allocate and initialize the data neccessary for enumeration of the queue.**]**  
**SRS_IOTHUB_QUEUE_07_019: [**On Success IoTHubQueue_Enum_Queue shall return a IOTHUB_QUEUE_ENUM_HANDLE.**]**  
**SRS_IOTHUB_QUEUE_07_025: [**If the queue referenced by handle is empty IoTHubQueue_Enum_Queue shall return NULL.**]**  
**SRS_IOTHUB_QUEUE_07_026: [**If any error is encountered IoTHubQueue_Enum_Queue shall return NULL.**]**  

### IoTHubQueue_Enum_Next_Item

```c
const CLIENT_QUEUE_ITEM* IoTHubQueue_Enum_Next_Item(IOTHUB_QUEUE_ENUM_HANDLE enum_handle)
```

**SRS_IOTHUB_QUEUE_07_020: [**If `enum_handle` is NULL IoTHubQueue_Enum_Next_Item shall return NULL.**]**  
**SRS_IOTHUB_QUEUE_07_021: [**If the end of the queue is reached IoTHubQueue_Enum_Next_Item shall return NULL.**]**  
**SRS_IOTHUB_QUEUE_07_022: [**On success IoTHubQueue_Enum_Next_Item shall return the next queue item from the previous call to IoTHubQueue_Enum_Next_Item.**]**   

### IoTHubQueue_Enum_Close

```c
void IoTHubQueue_Enum_Close(IOTHUB_QUEUE_ENUM_HANDLE enum_handle)
```

**SRS_IOTHUB_QUEUE_07_023: [**If enum_handle is NULL IoTHubQueue_Enum_Close shall do nothing.**]**  
**SRS_IOTHUB_QUEUE_07_024: [**IoTHubQueue_Enum_Close shall deallocate any information that has been allocated in IoTHubQueue_Enum_Queue.**]**  
