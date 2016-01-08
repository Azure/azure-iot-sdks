# DeviceSerializer Requirements

## Overview

The DeviceSerializer class overrides the gson serialization with a custom one for the Device class,
so that the resulting JSON matches the format required by the service.

## References

## Exposed API

```java
public class DeviceSerializer implements JsonDeserializer<Device>
{
    @Override
    public JsonElement serialize(Device device, Type type, JsonSerializationContext jsonSerializationContext)
}
```

### serialize

```java
    @Override
    public Device deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext jsonDeserializationContext) throws JsonParseException
```

**SRS_SERVICE_SDK_JAVA_DEVICE_SERIALIZER_15_001: [** The function shall serialize a Device object into a JSON string recognized by the service **]**