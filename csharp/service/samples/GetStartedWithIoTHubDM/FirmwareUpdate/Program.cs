using System;
using System.Threading.Tasks;
using System.Threading;
using System.Collections.Generic;

using Microsoft.Azure.Devices;
using Newtonsoft.Json;
using Microsoft.Azure.Devices.QueryExpressions;
using System.Linq;

namespace ExecuteFirmwareUpdate
{
    class Program
    {
        static string packageURI = "http://the.firmware.package.location.com/";
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

            Console.WriteLine("ConnectionString: {0}", connectionString);

            // Update all the devices
            Update().Wait();

            Console.WriteLine("Done! Hit <Enter> to exit.");
            Console.ReadLine();
        }

        public static async Task Update()
        {
            try
            {
                // Create the job client
                Console.WriteLine("Creating job client...");
                var deviceJobClient = JobClient.CreateFromConnectionString(connectionString);

                // Read device IDs from devicecreds.txt
                Console.WriteLine("Starting update job...");
                var deviceIds = Setup.Setup.ReadDeviceID();


                // Start the update job on all devices
                var jobResponse = await deviceJobClient.ScheduleFirmwareUpdateAsync(Guid.NewGuid().ToString(), deviceIds, packageURI, TimeSpan.FromMinutes(25));

                // Poll for job completion
                while (jobResponse.Status == JobStatus.Enqueued ||
                    jobResponse.Status == JobStatus.Running ||
                    jobResponse.Status == JobStatus.Unknown)
                {
                    jobResponse = await deviceJobClient.GetJobAsync(jobResponse.JobId);
                    Console.WriteLine("Job status is {0}", jobResponse.Status.ToString());
                    Console.WriteLine();
                    OutputRunningJobs().Wait();
                    Console.WriteLine();

                    Thread.Sleep(1000);
                }

                await OutputJobs(deviceJobClient);

            }
            catch (Exception e)
            {
                Console.WriteLine("Error with job.");
                Console.WriteLine(e.ToString());
            }
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

                // List query result
                for (int i = 0; i < jobs.Count(); i++)
                {
                    JobResponse job = jobs[i];

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
            JobClient deviceJobClient = JobClient.CreateFromConnectionString(connectionString);

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

            JobQueryResult result = await deviceJobClient.QueryJobHistoryJsonAsync(json);

            Console.WriteLine("Active Devices running jobs");
            // List query result
            foreach (var job in result.Result)
            {
                Console.WriteLine("DeviceID: {0}\t\tJobId: {1}",
                    job.DeviceId,
                    job.JobId
                    );
            }
        }
    }
}
