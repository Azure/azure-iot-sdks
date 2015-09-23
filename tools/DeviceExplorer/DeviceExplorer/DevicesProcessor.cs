using Microsoft.Azure.Devices;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace DeviceExplorer
{
    class DevicesProcessor
    {
        private List<DeviceEntity> listOfDevices;
        private RegistryManager registryManager;
        private String iotHubConnectionString;
        private int maxCountOfDevices;
        private String protocolGatewayHostName;

        public DevicesProcessor(string iotHubConnenctionString, int devicesCount, string protocolGatewayName)
        {
            this.listOfDevices = new List<DeviceEntity>();
            this.iotHubConnectionString = iotHubConnenctionString;
            this.maxCountOfDevices = devicesCount;
            this.protocolGatewayHostName = protocolGatewayName;
            this.registryManager = RegistryManager.CreateFromConnectionString(iotHubConnectionString);
        }

        public async Task<List<DeviceEntity>> GetDevices()
        {
            try
            {
                DeviceEntity deviceEntity;
                var devices = await registryManager.GetDevicesAsync(maxCountOfDevices);

                if (devices != null)
                {
                    foreach (var device in devices)
                    {
                        deviceEntity = new DeviceEntity()
                        {
                            Id = device.Id,
                            ConnectionState = device.ConnectionState.ToString(),
                            ConnectionString = CreateDeviceConnectionString(device),
                            LastActivityTime = device.LastActivityTime,
                            LastConnectionStateUpdatedTime = device.ConnectionStateUpdatedTime,
                            LastStateUpdatedTime = device.StatusUpdatedTime,
                            MessageCount = device.CloudToDeviceMessageCount,
                            State = device.Status.ToString(),
                            SuspensionReason = device.StatusReason
                        };

                        if (device.Authentication != null &&
                            device.Authentication.SymmetricKey != null)
                        {
                            deviceEntity.PrimaryKey = device.Authentication.SymmetricKey.PrimaryKey;
                            deviceEntity.SecondaryKey = device.Authentication.SymmetricKey.SecondaryKey;
                        }

                        listOfDevices.Add(deviceEntity);
                    }
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
            return listOfDevices;
        }

        private String CreateDeviceConnectionString(Device device)
        {
            StringBuilder deviceConnectionString = new StringBuilder();

            var hostName = String.Empty;
            var tokenArray = iotHubConnectionString.Split(';');
            for (int i = 0; i < tokenArray.Length; i++)
            {
                var keyValueArray = tokenArray[i].Split('=');
                if (keyValueArray[0] == "HostName")
                {
                    hostName =  tokenArray[i] + ';';
                    break;
                }
            }

            if (!String.IsNullOrWhiteSpace(hostName))
            {
                deviceConnectionString.Append(hostName);
                deviceConnectionString.AppendFormat("DeviceId={0}", device.Id);

                if (device.Authentication != null &&
                    device.Authentication.SymmetricKey != null)
                {
                    deviceConnectionString.AppendFormat(";SharedAccessKey={0}", device.Authentication.SymmetricKey.PrimaryKey);
                }

                if (this.protocolGatewayHostName.Length > 0)
                {
                    deviceConnectionString.AppendFormat(";GatewayHostName=ssl://{0}:8883", this.protocolGatewayHostName);
                }
            }
            
            return deviceConnectionString.ToString();
        }
        // For testing without connecting to a live service
        static public List<DeviceEntity> GetDevicesForTest()
        {
            List<DeviceEntity> deviceList;
            deviceList = new List<DeviceEntity>();
            deviceList.Add(new DeviceEntity() { Id = "TestDevice01", PrimaryKey = "TestPrimKey01", SecondaryKey = "TestSecKey01" });
            deviceList.Add(new DeviceEntity() { Id = "TestDevice02", PrimaryKey = "TestPrimKey02", SecondaryKey = "TestSecKey02" });
            deviceList.Add(new DeviceEntity() { Id = "TestDevice03", PrimaryKey = "TestPrimKey03", SecondaryKey = "TestSecKey03" });
            deviceList.Add(new DeviceEntity() { Id = "TestDevice04", PrimaryKey = "TestPrimKey04", SecondaryKey = "TestSecKey04" });
            deviceList.Add(new DeviceEntity() { Id = "TestDevice05", PrimaryKey = "TestPrimKey05", SecondaryKey = "TestSecKey05" });
            return deviceList;
        }
    }
}
