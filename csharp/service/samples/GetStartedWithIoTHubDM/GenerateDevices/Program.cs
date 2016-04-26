using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

using Microsoft.Azure.Devices;
using Newtonsoft.Json;
using System.IO;

namespace GenerateDevices
{
    class Program
    {
        static RegistryManager registryManager;
        static List<Device> devices;
        public static string connectionString = "";

        static void Main(string[] args)
        {
            
            // Grab the IoT Hub connection string from args
            if (args.Length >= 1)
            {
                connectionString = args[0];
            }
            else
            {
                Console.WriteLine("Connection not set in code.  You must pass a connection string.");
                Console.WriteLine(AppDomain.CurrentDomain.FriendlyName + " <Your IoT Hub connection string>");

                Environment.Exit(1);
            }

            // Create Registry Manager
            registryManager = RegistryManager.CreateFromConnectionString(connectionString);
            
            // Import the devices from sampledevices.json, add them to IoT Hub, and save the unique device connection strings in devicecreds.txt
            ImportDevices().Wait();

            // Close Registry Manager
            registryManager.CloseAsync().Wait();
        }

        public async static Task ImportDevices()
        {
            try
            {
                // Deserialize the JSON file
                Console.WriteLine("Deserializing " + Setup.Setup.deviceStructure);
                devices = JsonConvert.DeserializeObject<List<Device>>(File.ReadAllText(Setup.Setup.deviceStructure));
                
                // String to store all device credentials
                var deviceCredsString = new StringBuilder();

                foreach (Device device in devices)
                {
                    // Remove the device if it exists from a previous run of the sample
                    try
                    {
                        await registryManager.RemoveDeviceAsync(device.Id);
                    }
                    catch(Exception ex)
                    {
                        if (ex is AggregateException)
                        {
                            var aggregateEx = ex as AggregateException;
                            foreach (var innerEx in aggregateEx.InnerExceptions)
                            {
                                if (!(innerEx is Microsoft.Azure.Devices.Common.Exceptions.DeviceNotFoundException))
                                {
                                    throw ex;
                                }
                            }
                        }
                        else if (!(ex is Microsoft.Azure.Devices.Common.Exceptions.DeviceNotFoundException))
                        {
                            throw ex;
                        }
                    }

                    // Add the device
                    Console.WriteLine("Adding device : {0}", device.Id);
                    var d = await registryManager.AddDeviceAsync(device);

                    // Add device connection string to string builder
                    var hostName = connectionString.Split(new string[] { ";" }, StringSplitOptions.None)[0];
                    deviceCredsString.Append(String.Format("{0};DeviceId={1};SharedAccessKey={2}{3}", hostName, d.Id, d.Authentication.SymmetricKey.PrimaryKey, System.Environment.NewLine));
                }

                // Write all device connection strings to devicecreds.txt
                if (File.Exists(Setup.Setup.deviceCreds))
                {
                    File.Delete(Setup.Setup.deviceCreds);
                }

                // Write string to file
                using (var deviceCredsFile = new StreamWriter(Setup.Setup.deviceCreds))
                {
                    deviceCredsFile.WriteLine(deviceCredsString.ToString());
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    } 
}
