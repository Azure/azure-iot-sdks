using Microsoft.Azure.Devices;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DeviceExplorer
{
    struct DeviceMethodReturnValue
    {
        public string Status;
        public string Payload;
    }

    struct DeviceTwinData
    {
        public string deviceJson;
        public string tagsJson;
        public string reportedPropertiesJson;
        public string desiredPropertiesJson;
    }

    class DeviceTwinAndMethod
    {
        private string messageDeviceTwinFunctionalityNotFound = "Device Twin functionality not found." + Environment.NewLine + "Make sure you are using the latest Microsoft.Azure.Devices package.";
        private string connString;
        private string deviceName;

        public DeviceTwinAndMethod(string connectionString, string deviceId)
        {
            connString = connectionString;
            deviceName = deviceId;
        }

        public async Task<DeviceTwinData> GetDeviceTwinData()
        {
            DeviceTwinData result = new DeviceTwinData();

            dynamic registryManager = RegistryManager.CreateFromConnectionString(connString);
            try
            {
                var deviceTwin = await registryManager.GetTwinAsync(deviceName);
                if (deviceTwin != null)
                {
                    result.deviceJson = deviceTwin.ToJson();
                    result.tagsJson = deviceTwin.Tags.ToJson();
                    result.reportedPropertiesJson = deviceTwin.Properties.Reported.ToJson();
                    result.desiredPropertiesJson = deviceTwin.Properties.Desired.ToJson();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + Environment.NewLine + "Make sure you are using the latest Microsoft.Azure.Devices package.", "Device Twin Properties", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return result;
        }

        public async Task<bool> UpdateTwinData(string updateJson)
        {
            dynamic registryManager = RegistryManager.CreateFromConnectionString(connString);
            if (registryManager != null)
            {
                try
                {
                    string assemblyClassName = "Twin";
                    Type typeFound = (from assembly in AppDomain.CurrentDomain.GetAssemblies()
                                      from assemblyType in assembly.GetTypes()
                                      where assemblyType.Name == assemblyClassName
                                      select assemblyType).FirstOrDefault();

                    if (typeFound != null)
                    {
                        var deviceTwin = await registryManager.GetTwinAsync(deviceName);
                        dynamic dp = JsonConvert.DeserializeObject(updateJson, typeFound);
                        dp.DeviceId = deviceName;
                        dp.ETag = deviceTwin.ETag;
                        registryManager.UpdateTwinAsync(dp.DeviceId, dp, dp.ETag);
                    }
                    else
                    {
                        MessageBox.Show(messageDeviceTwinFunctionalityNotFound, "Device Twin Properties Update", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
                catch (Exception ex)
                {
                    string errMess = "Update Twin failed. Exception: " + ex.ToString();
                    MessageBox.Show(errMess, "Device Twin Desired Properties Update", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                MessageBox.Show("Registry Manager is no initialized!", "Device Twin Desired Properties Update", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            await Task.Delay(1000);
            return true; ;
        }

        public async Task<DeviceMethodReturnValue> CallDeviceMethod(string deviceMethodName, string deviceMethodPayload, TimeSpan timeoutInSeconds, CancellationToken cancellationToken)
        {
            bool isOK = false;
            string exStr = "";
            DeviceMethodReturnValue deviceMethodReturnValue;
            deviceMethodReturnValue.Status = "";
            deviceMethodReturnValue.Payload = "";

            dynamic serviceClient = ServiceClient.CreateFromConnectionString(connString);
            try
            {
                string assemblyClassName = "CloudToDeviceMethod";
                Type typeFound = (from assembly in AppDomain.CurrentDomain.GetAssemblies()
                                  from assemblyType in assembly.GetTypes()
                                  where assemblyType.Name == assemblyClassName
                                  select assemblyType).FirstOrDefault();
                if (typeFound == null)
                {
                    throw new Exception(messageDeviceTwinFunctionalityNotFound);
                }
                else
                {
                    object cloudToDeviceMethod = Activator.CreateInstance(typeFound, deviceMethodName, timeoutInSeconds);
                    MethodInfo methodInfo_SetPayloadJson = cloudToDeviceMethod.GetType().GetMethod("SetPayloadJson");
                    object[] parametersArray = new object[] { deviceMethodPayload };
                    dynamic cloudToDeviceMethodInstance = methodInfo_SetPayloadJson.Invoke(cloudToDeviceMethod, parametersArray);

                    dynamic result = await serviceClient.InvokeDeviceMethodAsync(deviceName, cloudToDeviceMethodInstance, cancellationToken);

                    deviceMethodReturnValue.Status = result.Status;
                    deviceMethodReturnValue.Payload = result.GetPayloadAsJson();

                    isOK = true;
                }
            }
            catch (Exception ex)
            {
                exStr = ex.Message;
            }

            if (!isOK)
            {
                MessageBox.Show(exStr, "Device Twin Properties", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return deviceMethodReturnValue;
        }
    }
}
