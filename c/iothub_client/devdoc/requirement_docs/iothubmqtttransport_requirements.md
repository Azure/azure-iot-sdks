# IoTHubMQTTTransport Requirements

##Overview

IoTHubMQTTTransport is the library that enables communications with MQTT message

##Exposed API

```C
extern TRANSPORT_HANDLE IoTHubTransportMqtt_Create(const IOTHUBTRANSPORT_CONFIG* config);
extern void IoTHubTransportMqtt_Destroy(TRANSPORT_HANDLE handle);

extern int IoTHubTransportMqtt_Subscribe(TRANSPORT_HANDLE handle);
extern void IoTHubTransportMqtt_Unsubscribe(TRANSPORT_HANDLE handle);

extern void IoTHubTransportMqtt_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle);

extern IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_GetSendStatus(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
extern IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_SetOption(TRANSPORT_HANDLE handle, const char* optionName, const void* value);
extern const void* MQTT_Protocol(void);
```

##IoTHubTransportMqtt_Create
```
TRANSPORT_HANDLE IoTHubTransportMqtt_Create(const IOTHUBTRANSPORT_CONFIG* config)
```
IoTHubTransportMqtt_Create shall create a TRANSPORT_HANDLE that can be further used in the calls to this module’s APIS.  

**SRS_IOTHUB_MQTT_TRANSPORT_07_001: [**If parameter config is NULL then IoTHubTransportMqtt_Create shall return NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_002: [**If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransportMqtt_Create shall return NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_003: [**If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_004: [**If the config's waitingToSend variable is NULL then IoTHubTransportMqtt_Create shall return NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_005: [**If the upperConfig's variables deviceKey, iotHubName, or iotHubSuffix are empty strings then IoTHubTransportMqtt_Create shall return NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_006: [**If the upperConfig's variables deviceId is an empty strings or length is greater then 128 then IoTHubTransportMqtt_Create shall return NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_007: [**If the upperConfig's variables protocolGatewayHostName is non-Null and the length is an empty string then IoTHubTransportMqtt_Create shall return NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_008: [**If the upperConfig contains a valid protocolGatewayHostName value the this shall be used for the hostname, otherwise the hostname shall be constructed using the iothubname and iothubSuffix.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_009: [**If any error is encountered then IoTHubTransportMqtt_Create shall return NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_010: [**IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_011: [**On Success IoTHubTransportMqtt_Create shall return a non-NULL value.**]**  

##IoTHubTransportMqtt_Destroy
```
void IoTHubTransportMqtt_Destroy(TRANSPORT_HANDLE handle)
```
**SRS_IOTHUB_MQTT_TRANSPORT_07_012: [**IoTHubTransportMqtt_Destroy shall do nothing if parameter handle is NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_013: [**If the parameter subscribe is true then IoTHubTransportMqtt_Destroy shall call IoTHubTransportMqtt_Unsubscribe.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_014: [**IoTHubTransportMqtt_Destroy shall free all the resources currently in use.**]**  

##IoTHubTransportMqtt_Subscribe
```
int IoTHubTransportMqtt_Subscribe(TRANSPORT_HANDLE handle)
```
**SRS_IOTHUB_MQTT_TRANSPORT_07_015: [**If parameter handle is NULL than IoTHubTransportMqtt_Subscribe shall return a non-zero value.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_016: [**IoTHubTransportMqtt_Subscribe shall call mqtt_client_subscribe to subscribe to the Message Topic.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_017: [**Upon failure IoTHubTransportMqtt_Subscribe shall return a non-zero value.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_018: [**On success IoTHubTransportMqtt_Subscribe shall return 0.**]**  

##IoTHubTransportMqtt_Unsubscribe
```
void IoTHubTransportMqtt_Unsubscribe(TRANSPORT_HANDLE handle)
```
**SRS_IOTHUB_MQTT_TRANSPORT_07_019: [**If parameter handle is NULL then IoTHubTransportMqtt_Unsubscribe shall do nothing.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_020: [**IoTHubTransportMqtt_Unsubscribe shall call mqtt_client_unsubscribe to unsubscribe the mqtt message topic.**]**  

##IoTHubTransportMqtt_DoWork
```
void IoTHubTransportMqtt_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
```
**SRS_IOTHUB_MQTT_TRANSPORT_07_026: [**IoTHubTransportMqtt_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_027: [**IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_028: [**IoTHubTransportMqtt_DoWork shall retrieve the payload message from the messageHandle parameter.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_029: [**IoTHubTransportMqtt_DoWork shall create a MQTT_MESSAGE_HANDLE and pass this to a call to  mqtt_client_publish.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_030: [**IoTHubTransportMqtt_DoWork shall call mqtt_client_dowork everytime it is called if it is connected.**]**  

##IoTHubTransportMqtt_GetSendStatus
```
IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_GetSendStatus(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
```
**SRS_IOTHUB_MQTT_TRANSPORT_07_023: [**IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.**]**  
**SRS_IOTHUB_MQTT_TRANSPORT_07_024: [**IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.**]**   
**SRS_IOTHUB_MQTT_TRANSPORT_07_025: [**IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.**]**  

##IoTHubTransportMqtt_SetOption
```
IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_SetOption(TRANSPORT_HANDLE handle, const char* optionName, const void* value)
```
**SRS_IOTHUB_MQTT_TRANSPORT_07_021: [**If any parameter is NULL then IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]**
**SRS_IOTHUB_MQTT_TRANSPORT_07_031: [**If the option parameter is set to "logtrace" then the value shall be a bool_ptr and the value will determine if the mqtt client log is on or off.**]**   
**SRS_IOTHUB_MQTT_TRANSPORT_07_032: [**IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG if the option parameter is not a known option string.**]**   

##MQTT_Protocol
```
const void* MQTT_Protocol(void)
```
**SRS_IOTHUB_MQTT_TRANSPORT_07_022: [**This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it’s fields: 
IoTHubTransport_Create = IoTHubTransportMqtt_Create  
IoTHubTransport_Destroy = IoTHubTransportMqtt_Destroy  
IoTHubTransport_Subscribe = IoTHubTransportMqtt_Subscribe  
IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_Unsubscribe  
IoTHubTransport_DoWork = IoTHubTransportMqtt_DoWork  
IoTHubTransport_SetOption = IoTHubTransportMqtt_SetOption**]**
