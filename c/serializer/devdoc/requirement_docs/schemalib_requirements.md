# IoTHub_Schema_Client Requirements

 
## Overview
 
The IoTHub_Schema_Client module coordinates all modules.

## Exposed API

**SRS_SCHEMALIB_99_001: [**  IoTHub_Schema_Client shall expose the following API:  **]**
```c
#define IOTHUB_SCHEMA_CLIENT_RESULT_VALUES                   \
    IOTHUB_SCHEMA_CLIENT_OK,                                 \
    IOTHUB_SCHEMA_CLIENT_INVALID_ARG,                        \
    IOTHUB_SCHEMA_CLIENT_CODEFIRST_INIT_FAILED,              \
    IOTHUB_SCHEMA_CLIENT_SCHEMA_FAILED,                      \
    IOTHUB_SCHEMA_CLIENT_HTTP_API_INIT_FAILED,               \
    IOTHUB_SCHEMA_CLIENT_ALREADY_INIT,                       \
    IOTHUB_SCHEMA_CLIENT_ERROR,                              \
    IOTHUB_SCHEMA_CLIENT_NOT_INITIALIZED,                    \
    IOTHUB_SCHEMA_CLIENT_ALREADY_STARTED,                    \
    IOTHUB_SCHEMA_CLIENT_DEVICE_CREATE_FAILED,               \
    IOTHUB_SCHEMA_CLIENT_GET_MODEL_HANDLE_FAILED,            \

DEFINE_ENUM(IOTHUB_SCHEMA_CLIENT_RESULT, IOTHUB_SCHEMA_CLIENT_RESULT_VALUES);

#define IOTHUB_SCHEMA_CLIENT_CONFIG_VALUES  \
    SerializeDelayedBufferMaxSize

DEFINE_ENUM(IOTHUB_SCHEMA_CLIENT_CONFIG, IOTHUB_SCHEMA_CLIENT_CONFIG_VALUES);

extern IOTHUB_SCHEMA_CLIENT_RESULT iothub_schema_client_init(const char* overrideSchemaNamespace);
extern void iothub_schema_client_deinit(void);
extern IOTHUB_SCHEMA_CLIENT_RESULT iothub_schema_client_setconfig(IOTHUB_SCHEMA_CLIENT_CONFIG which, void* value);
```

### iothub_schema_client_init
```c
IOTHUB_SCHEMA_CLIENT_RESULT iothub_schema_client_init(const char* overrideSchemaNamespace );
```

When iothub_schema_client_init is called it shall:

**SRS_SCHEMALIB_99_006: [**  Initialize CodeFirst by a call to CodeFirst_Init. **]**

**SRS_SCHEMALIB_99_007: [**  On error IOTHUB_SCHEMA_CLIENT_CODEFIRST_INIT_FAILED shall be returned. **]**

**SRS_SCHEMALIB_99_076: [** iothub_schema_client_init shall pass the value of overrideSchemaNamespace argument to CodeFirst_Init. **]**

**SRS_SCHEMALIB_99_073: [** On success iothub_schema_client_init shall return IOTHUB_SCHEMA_CLIENT_OK. **]**
**SRS_SCHEMALIB_99_074: [** iothub_schema_client_init when already initialized shall return IOTHUB_SCHEMA_CLIENT_ALREADY_INIT. **]**

### iothub_schema_client_deinit
```c
void iothub_schema_client_deinit(void);
```

**SRS_SCHEMALIB_99_025: [**  iothub_schema_client_deinit shall de-initialize all modules initialized by iothub_schema_client_init. **]**

**SRS_SCHEMALIB_99_026: [**  A subsequent call to iothub_schema_client_init shall succeed. **]**

**SRS_SCHEMALIB_99_044: [**  If iothub_schema_client is not initialized, iothub_schema_client_deinit shall not attempt to de-initialize any module. **]**

**SRS_SCHEMALIB_99_075: [** When an iothub_schema_client_init call fails for any reason the previous initialization state shall be preserved. The initialized state shall only be changed on a succesfull Init. **]**

### iothub_schema_client_setconfig
```c
IOTHUB_SCHEMA_CLIENT_RESULT iothub_schema_client_setConfig(IOTHUB_SCHEMA_CLIENT_CONFIG which, void* value);
```

**SRS_SCHEMALIB_99_137: [**  If the value argument is NULL, iothub_schema_client_setconfig shall return IOTHUB_SCHEMA_CLIENT_INVALID_ARG. **]**

**SRS_SCHEMALIB_99_138: [**  If the which argument is not one of the declared members of the IOTHUB_SCHEMA_CLIENT_CONFIG enum, iothub_schema_client_setconfig shall return IOTHUB_SCHEMA_CLIENT_INVALID_ARG. **]**

**SRS_SCHEMALIB_99_142: [**  When the which argument is SerializeDelayedBufferMaxSize, iothub_schema_client_setconfig shall invoke DataPublisher_SetMaxBufferSize with the dereferenced value argument, and shall return IOTHUB_SCHEMA_CLIENT_OK. **]**

