# iothubtransport_amqp_connection Requirements


## Overview

This library contains functions to assist creating and managing the basic uamqp components for establishing an AMQP connection with Azure IoT Hub.


## Dependencies

This module will depend on the following modules:

iothubtransport_amqp_common
azure-c-shared-utilities
azure-c-uamqp


## Exposed API

```c
	typedef enum AMQP_CONNECTION_STATE_TAG
	{
		AMQP_CONNECTION_STATE_OPENED,
		AMQP_CONNECTION_STATE_CLOSED,
		AMQP_CONNECTION_STATE_ERROR
	} AMQP_CONNECTION_STATE;

	typedef void(*ON_AMQP_CONNECTION_STATE_CHANGED)(const void* context, AMQP_CONNECTION_STATE old_state, AMQP_CONNECTION_STATE new_state);

	typedef struct AMQP_CONNECTION_CONFIG_TAG
	{
		const char* iothub_host_fqdn;
		XIO_HANDLE underlying_io_transport;
		bool create_sasl_io;
		bool create_cbs_connection;
		bool is_trace_on;
		
		ON_AMQP_CONNECTION_STATE_CHANGED on_state_changed_callback;
		const void* on_state_changed_context;
	} AMQP_CONNECTION_CONFIG;

	typedef struct AMQP_CONNECTION_STATE* AMQP_CONNECTION_HANDLE;

	AMQP_CONNECTION_HANDLE amqp_connection_create(AMQP_CONNECTION_CONFIG* config);
	void amqp_connection_destroy(AMQP_CONNECTION_HANDLE conn_handle);
	void amqp_connection_do_work(AMQP_CONNECTION_HANDLE conn_handle);
	int amqp_connection_get_session_handle(AMQP_CONNECTION_HANDLE conn_handle, SESSION_HANDLE* session_handle);
	int amqp_connection_get_cbs_handle(AMQP_CONNECTION_HANDLE conn_handle, CBS_HANDLE* cbs_handle);
	int amqp_connection_set_logging(AMQP_CONNECTION_HANDLE conn_handle, bool is_trace_on);
```


## amqp_connection_create

```c
AMQP_CONNECTION_HANDLE amqp_connection_create(AMQP_CONNECTION_CONFIG* config);
```

**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_001: [**If `config` is NULL, amqp_connection_create() shall fail and return NULL**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_002: [**If `config->iothub_host_fqdn` is NULL, amqp_connection_create() shall fail and return NULL**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_003: [**If `config->underlying_io_transport` is NULL, amqp_connection_create() shall fail and return NULL**]**

**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_057: [**amqp_connection_create() shall allocate memory for an instance of the connection instance**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_058: [**If malloc() fails, amqp_connection_create() shall fail and return NULL**]**

**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_005: [**A copy of `config->iothub_host_fqdn` shall be saved on `instance->iothub_host_fqdn`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_066: [**If STRING_construct() fails, amqp_connection_create() shall fail and return NULL**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_006: [**`config->underlying_io_transport` shall be saved on `instance->underlying_io_transport`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_008: [**`config->is_trace_on` shall be saved on `instance->is_trace_on`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_060: [**`config->on_state_changed_callback` shall be saved on `instance->on_state_changed_callback`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_061: [**`config->on_state_changed_context` shall be saved on `instance->on_state_changed_context`**]**


### Creating SASL instances 
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_011: [**If `config->create_sasl_io` is true or `config->create_cbs_connection` is true, amqp_connection_create() shall create SASL I/O**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_012: [**`instance->sasl_mechanism` shall be created using saslmechanism_create()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_013: [**If saslmechanism_create() fails, amqp_connection_create() shall fail and return NULL**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_014: [**A SASLCLIENTIO_CONFIG shall be set with `instance->underlying_io_transport` and `instance->sasl_mechanism`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_015: [**`instance->sasl_io` shall be created using xio_create() passing saslclientio_get_interface_description() and the SASLCLIENTIO_CONFIG instance**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_016: [**If xio_create() fails, amqp_connection_create() shall fail and return NULL**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_017: [**The sasl_io "logtrace" option shall be set using xio_setoption(), passing `instance->is_trace_on`**]**

### Creating the connection and session
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_007: [**If `instance->sasl_io` is defined it shall be used as parameter `xio` in connection_create2()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_018: [**If `instance->sasl_io` is not defined, `instance->underlying_io_transport` shall be used as parameter `xio` in connection_create2()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_019: [**amqp_connection_create() shall use the `instance->sasl_io` as the `connection_underlying_io`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_020: [**connection_create2() shall also receive `on_connection_state_changed` and `on_connection_error` callback functions**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_021: [**If connection_create2() fails, amqp_connection_create() shall fail and return NULL**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_023: [**The connection tracing shall be set using connection_set_trace(), passing `instance->is_trace_on`**]**

**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_024: [**`instance->session_handle` shall be created using session_create(), passing `instance->connection_handle`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_025: [**If session_create() fails, amqp_connection_create() shall fail and return NULL**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_026: [**The `instance->session_handle` incoming window size shall be set as UINT_MAX using session_set_incoming_window()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_027: [**The `instance->session_handle` outgoing window size shall be set as 100 using session_set_outgoing_window()**]**

### Creating the CBS instance
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_028: [**Only if `config->create_cbs_connection` is true, amqp_connection_create() shall create and open the CBS_HANDLE**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_029: [**`instance->cbs_handle` shall be created using cbs_create(), passing `instance->session_handle` and `on_cbs_state_changed` callback**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_030: [**If cbs_create() fails, amqp_connection_create() shall fail and return NULL**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_031: [**`instance->cbs_handle` shall be opened using cbs_open()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_032: [**If cbs_open() fails, amqp_connection_create() shall fail and return NULL**]**

### General
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_034: [**If no failures occur, amqp_connection_create() shall return the handle to the connection state**]**


#### on_connection_state_changed
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_063: [**If `on_connection_state_changed` is called back, `instance->on_state_changed_callback` shall be invoked, if defined, only if the new state is different than the previous**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_064: [**If `on_connection_state_changed` new state is CONNECTION_STATE_OPENED, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_OPENED**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_065: [**If `on_connection_state_changed` new state is CONNECTION_STATE_END, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_CLOSED**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_071: [**If `on_connection_state_changed` new state is CONNECTION_STATE_ERROR, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_ERROR**]**

#### on_cbs_state_changed
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_068: [**If `on_cbs_state_changed` is called back, `instance->on_state_changed_callback` shall be invoked, if defined, only if the new state is different than the previous**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_069: [**If `on_cbs_state_changed` new state is AMQP_MANAGEMENT_STATE_OPEN, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_OPENED**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_070: [**If `on_cbs_state_changed` new state is AMQP_MANAGEMENT_STATE_IDLE, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_CLOSED**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_070: [**If `on_cbs_state_changed` new state is AMQP_MANAGEMENT_STATE_ERROR, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_ERROR**]**

#### on_connection_error
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_022: [**If the connection calls back with an I/O error, `instance->on_state_changed_callback` shall be invoked if set passing code AMQP_CONNECTION_STATE_ERROR and `instance->on_state_changed_context`**]**



## amqp_connection_destroy

```c
void amqp_connection_destroy(AMQP_CONNECTION_HANDLE conn_handle);
```

**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_035: [**If `conn_handle` is NULL, amqp_connection_destroy() shall fail and return**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_036: [**amqp_connection_destroy() shall destroy `instance->cbs_handle` if set using cbs_destroy()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_037: [**amqp_connection_destroy() shall destroy `instance->session_handle` if set using session_destroy()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_067: [**amqp_connection_destroy() shall destroy `instance->connection_handle` if set using connection_destroy()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_038: [**amqp_connection_destroy() shall destroy `instance->sasl_io` if set using xio_destroy()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_039: [**amqp_connection_destroy() shall destroy `instance->sasl_mechanism` if set using saslmechanism_destroy()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_059: [**amqp_connection_destroy() shall destroy `instance->iothub_host_fqdn` if set using STRING_delete()**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_040: [**amqp_connection_destroy() shall free the memory allocated for the connection instance**]**


## amqp_connection_do_work

```c
void amqp_connection_do_work(AMQP_CONNECTION_HANDLE conn_handle);
```

**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_041: [**If `conn_handle` is NULL, amqp_connection_do_work() shall fail and return**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_042: [**connection_dowork() shall be invoked passing `instance->connection_handle`**]**


## amqp_connection_get_session_handle

```c
int amqp_connection_get_session_handle(AMQP_CONNECTION_HANDLE conn_handle, SESSION_HANDLE* session_handle);
```

**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_043: [**If `conn_handle` is NULL, amqp_connection_get_session_handle() shall fail and return __LINE__**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_044: [**If `session_handle` is NULL, amqp_connection_get_session_handle() shall fail and return __LINE__**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_045: [**`session_handle` shall be set to point to `instance->session_handle`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_046: [**amqp_connection_get_session_handle() shall return success code 0**]**


## amqp_connection_get_cbs_handle

```c
int amqp_connection_get_cbs_handle(AMQP_CONNECTION_HANDLE conn_handle, CBS_HANDLE* cbs_handle);
```

**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_047: [**If `conn_handle` is NULL, amqp_connection_get_cbs_handle() shall fail and return __LINE__**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_048: [**If `cbs_handle` is NULL, amqp_connection_get_cbs_handle() shall fail and return __LINE__**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_049: [**If `instance->cbs_handle` is NULL, amqp_connection_get_cbs_handle() shall fail and return __LINE__**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_050: [**`cbs_handle` shall be set to point to `instance->cbs_handle`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_051: [**amqp_connection_get_cbs_handle() shall return success code 0**]**


## amqp_connection_set_logging

```c
int amqp_connection_set_logging(AMQP_CONNECTION_HANDLE conn_handle, bool is_trace_on);
```

**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_052: [**If `conn_handle` is NULL, amqp_connection_set_logging() shall fail and return __LINE__**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_053: [**`instance->is_trace_on` shall be set to `is_trace_on`**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_054: [**Tracing on `instance->sasl_io` shall be set to `instance->is_trace_on` if the value has changed**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_072: [**If xio_setoption() fails, amqp_connection_set_logging() shall fail and return __LINE__**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_055: [**Tracing on `instance->connection_handle` shall be set to `instance->is_trace_on` if the value has changed**]**
**SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_056: [**amqp_connection_set_logging() shall return success code 0**]**

