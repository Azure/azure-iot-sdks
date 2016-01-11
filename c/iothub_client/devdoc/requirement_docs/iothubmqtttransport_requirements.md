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

