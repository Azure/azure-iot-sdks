param (
[string] $path = $null
)

$amqpTransportPkg = Get-ChildItem $path\Microsoft.Azure.IoTHub.AmqpTransport.*.nupkg
$httpTransportPkg = Get-ChildItem $path\Microsoft.Azure.IoTHub.HttpTransport.*.nupkg
$mqttTransportPkg = Get-ChildItem $path\Microsoft.Azure.IoTHub.MqttTransport.*.nupkg
$serializerPkg = Get-ChildItem $path\Microsoft.Azure.IoTHub.Serializer.*.nupkg
$uamqpPkg = Get-ChildItem $path\Microsoft.Azure.uamqp.*.nupkg

Install-Package $($path + '\' + ($amqpTransportPkg.Name)) -ProjectName iothubclient_amqp_e2e_nuget_exe
Install-Package $($path + '\' + ($mqttTransportPkg.Name)) -ProjectName iothubclient_mqtt_e2e_nuget_exe
Install-Package $($path + '\' + ($httpTransportPkg.Name)) -ProjectName iothubclient_http_e2e_nuget_exe
Install-Package $($path + '\' + ($uamqpPkg.Name)) -ProjectName iothubclient_http_e2e_nuget_exe
Install-Package $($path + '\' + ($serializerPkg.Name)) -ProjectName serializer_e2e_nuget_exe
Install-Package $($path + '\' + ($amqpTransportPkg.Name)) -ProjectName serializer_e2e_nuget_exe
Install-Package $($path + '\' + ($httpTransportPkg.Name)) -ProjectName serializer_e2e_nuget_exe
Install-Package $($path + '\' + ($uamqpPkg.Name)) -ProjectName iothubclient_mqtt_e2e_nuget_exe