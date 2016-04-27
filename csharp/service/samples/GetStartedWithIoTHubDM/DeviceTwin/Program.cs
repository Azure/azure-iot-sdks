using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.Azure.Devices;
using System.Threading;
using Newtonsoft.Json;

namespace DeviceTwin
{
    class Program
    {
        static RegistryManager registryManager;
        public static string deviceId;
        public static List<string> deviceIds;
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
                Console.WriteLine("Please pass a connection string as argument.");
                Console.WriteLine(AppDomain.CurrentDomain.FriendlyName + "<Your IoT Hub connection string>");

                Environment.Exit(1);
            }

            registryManager = RegistryManager.CreateFromConnectionString(connectionString);
            deviceIds = Setup.Setup.ReadDeviceID();
            deviceId = deviceIds[0];

            OutputDeviceProperties().Wait();

            DeepReadFromPhysicalDevice().Wait();

            DeepWriteToPhysicalDevice().Wait();

            registryManager.CloseAsync().Wait();
            Console.WriteLine("Done! Hit <Enter> to exit.");
            Console.ReadLine();
        }
                
        public async static Task OutputDeviceProperties()
        {
            Device device = await registryManager.GetDeviceAsync(deviceId);

            Console.WriteLine("*********************************************************");
            Console.WriteLine("***** Device properties");
            Console.WriteLine("*********************************************************");
            Console.WriteLine();

            // Display all device properties
            foreach (var property in device.DeviceProperties)
            {
                Console.WriteLine("{0} : {1}", property.Key, property.Value.Value?.ToString());
            }

            Console.WriteLine();

            return;
        }

        public async static Task<DevicePropertyValue> GetCurrentBatteryLevel(string deviceId)
        {
            var device = await registryManager.GetDeviceAsync(deviceId);
            DevicePropertyValue batteryLevel;
            device.DeviceProperties.TryGetValue(DevicePropertyNames.BatteryLevel, out batteryLevel);
            return batteryLevel;
        }

        public static DevicePropertyValue TryGetDevicePropertyValue(Device device, string propertyName)
        {
            DevicePropertyValue propValue = null;
            device.DeviceProperties.TryGetValue(propertyName, out propValue);
            return propValue;
        }

        public async static Task DeepReadFromPhysicalDevice()
        {
            var deviceJobClient = JobClient.CreateFromConnectionString(connectionString);

            for (int i = 0; i < 3; i++)
            {
                DevicePropertyValue battery = await GetCurrentBatteryLevel(deviceId);

                string propertyToRead = DevicePropertyNames.BatteryLevel;

                Console.WriteLine("Reading {0} --- Current Value: {1} at {2}", 
                    propertyToRead,
                    battery?.Value.ToString(),
                    battery?.LastUpdatedTime.ToString("u"));
                Console.WriteLine();            

                await DeepRead(deviceJobClient, propertyToRead);

                DevicePropertyValue newBattery = await GetCurrentBatteryLevel(deviceId);

                Console.WriteLine();
                Console.WriteLine("After Read -- {0} : {1} Updated: {2}", propertyToRead, newBattery?.Value.ToString(), newBattery?.LastUpdatedTime.ToString("u"));
                Console.WriteLine();
            }
            Console.WriteLine();

            return;
        }

        private static async Task DeepRead(JobClient deviceJobClient, string propertyToRead)
        {
            Console.WriteLine();
            Console.WriteLine("*********************************************************");
            Console.WriteLine("***** Starting deep read");
            Console.WriteLine("*********************************************************");
            Console.WriteLine();
            Console.WriteLine("Reading device property from physical device: {0}", propertyToRead);
            Console.WriteLine();

            var jobResponse = await deviceJobClient.ScheduleDevicePropertyReadAsync(Guid.NewGuid().ToString(), deviceId, propertyToRead);

            // Poll for job completion
            while (jobResponse.Status == JobStatus.Enqueued ||
                jobResponse.Status == JobStatus.Running ||
                jobResponse.Status == JobStatus.Unknown)
            {
                jobResponse = await deviceJobClient.GetJobAsync(jobResponse.JobId);
                Console.WriteLine("DevicePropertyRead job status is {0} {1} {2}", jobResponse.Status.ToString(), jobResponse.JobId.ToString(), jobResponse.FailureReason);

                await Task.Delay(TimeSpan.FromSeconds(3));
            }

            Console.WriteLine();

            await OutputJobs(deviceJobClient);
        }

        public async static Task DeepWriteToPhysicalDevice()
        {
            Device device = await registryManager.GetDeviceAsync(deviceId);

            var deviceJobClient = JobClient.CreateFromConnectionString(connectionString);
            string jobId = "";
            
            string propertyToSet = DevicePropertyNames.Timezone;
            string setValue = "PST *** NEW - "+DateTime.Now.TimeOfDay.ToString();

            Console.WriteLine();
            Console.WriteLine("*********************************************************");
            Console.WriteLine("***** Starting Deep Write");
            Console.WriteLine("*********************************************************");
            Console.WriteLine();
            Console.WriteLine("Writing device property to physical device: '{0}' ", propertyToSet);
            Console.WriteLine("Setting {0} to '{1}'", propertyToSet, setValue);

            DevicePropertyValue currentValue = TryGetDevicePropertyValue(device, propertyToSet);

            Console.WriteLine("Current Value of {0} : '{1}' at {2}",
                propertyToSet,
                currentValue?.Value.ToString(),
                currentValue?.LastUpdatedTime.ToString("u"));

            var jobResponse = await deviceJobClient.ScheduleDevicePropertyWriteAsync(Guid.NewGuid().ToString(), deviceId, propertyToSet, setValue);

            // Poll for job completion
            while (jobResponse.Status == JobStatus.Enqueued ||
                jobResponse.Status == JobStatus.Running ||
                jobResponse.Status == JobStatus.Unknown)
            {
                jobResponse = await deviceJobClient.GetJobAsync(jobResponse.JobId);
                Console.WriteLine("DevicePropertyWrite job status is {0} {1} {2}", jobResponse.Status.ToString(), jobResponse.JobId.ToString(), jobResponse.FailureReason);

                jobId = jobResponse.JobId;
                Thread.Sleep(3000);
            }

            Console.WriteLine();

            await OutputJobs(deviceJobClient);

            Console.WriteLine();
            Console.WriteLine("DevicePropertyWrite job complete.\r\n");
            Console.WriteLine();

            await DeepRead(deviceJobClient, propertyToSet);

            // Refresh the Device object
            device = await registryManager.GetDeviceAsync(deviceId);

            currentValue = TryGetDevicePropertyValue(device, DevicePropertyNames.Timezone);

            Console.WriteLine();
            Console.WriteLine("After deep write new value for '{0}' : {1} Updated: {2}", DevicePropertyNames.Timezone, currentValue?.Value.ToString(), currentValue?.LastUpdatedTime.ToString("u"));

            Console.WriteLine();

            return;
        }

        private static async Task OutputJobs(JobClient deviceJobClient)
        {
            string json = JsonConvert.SerializeObject(
                new
                {
                    sort = new[]
                        {
                            new
                            {
                                property = new
                                {
                                    name = "StartTimeUtc",
                                    type = "default"
                                },
                                order = "desc"
                            }
                    }

                }
            );

            try
            {
                var result = await deviceJobClient.QueryJobHistoryJsonAsync(json);

                var jobs = result.Result.ToArray();

                Console.WriteLine("*********************************************************");
                Console.WriteLine("***** Results of device jobs");
                Console.WriteLine("*********************************************************");
                Console.WriteLine();

                // List query result
                for (int i = 0; i < 2; i++)
                {
                    var job = jobs[i];

                    Console.WriteLine("StartTime: {0}\t\tJobId: {1}\tParentJobId: '{2}'\tStatus: {3}\tReason: {4}",
                        job.StartTimeUtc.ToString(),
                        job.JobId,
                        job.ParentJobId,
                        job.Status.ToString(),
                        job.FailureReason
                        );
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine("Caught " + ex.ToString());
            }
        }

        private static async Task OutputRunningJobs()
        {
            var deviceJobClient = JobClient.CreateFromConnectionString(connectionString);

            string json = JsonConvert.SerializeObject(
                new
                {
                    filter = new
                    {
                        property = new
                        {
                            name = "Status",
                            type = "default"
                        },
                        value = "Running",
                        comparisonOperator = "eq",
                        type = "comparison"
                    }
                }
            );

            var result = await deviceJobClient.QueryJobHistoryJsonAsync(json);

            Console.WriteLine("Active Devices running jobs");
            // List query result
            foreach (var job in result.Result)
            {
                //JobResponse job = jobs[i];

                Console.WriteLine("DeviceID: {0}\t\tJobId: {1}",
                    job.DeviceId,
                    job.JobId
                    );
            }
        }



    }
}
