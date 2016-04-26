using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Newtonsoft.Json;

using Microsoft.Azure.Devices;

namespace Query
{
    class Program
    {
        static RegistryManager registryManager;

        static void Main(string[] args)
        {
            string connectionsString = "";

            // Grab the IoT Hub connection string from args
            if (args.Length >= 1)
            {
                connectionsString = args[0];
            }
            else
            {
                Console.WriteLine("Connection not set in code.  You must pass a connection string.");
                Console.WriteLine(AppDomain.CurrentDomain.FriendlyName + " <Your IoT Hub connection string>");
                Console.WriteLine(AppDomain.CurrentDomain.FriendlyName + " will look for a file named devicecreds.txt in the working directory for device connection strings");

                Environment.Exit(1);
            }

            registryManager = RegistryManager.CreateFromConnectionString(connectionsString);

            Console.WriteLine("\n\nQuerying for Tag bacon ---");
            QueryTags().Wait();

            // Query for CustomerID=123456
            Console.WriteLine("\n\nQuerying for CustomerID:123456 ---");
            QueryFilter().Wait();

            // Aggregate Query for Total Weight, Group by CustomerID
            Console.WriteLine("\n\nQuerying for All Devices: Aggregated to Total Weight, Grouped by CustomerId ---");
            QueryAggregate().Wait();

            // Sort by CustomerID
            Console.WriteLine("\n\nQuerying for All Devices: Sorted by QoS ---");
            QuerySort().Wait();

            // Query based on device properties
            Console.WriteLine("\n\nQuerying devices with FirmwareVersion == '1.0' ---");
            DevicePropertyQuery().Wait();

            registryManager.CloseAsync().Wait();
            Console.WriteLine("Done! Hit <Enter> to exit.");
            Console.ReadLine();
        }
        
        public async static Task QueryTags()
        {
            try
            {
                // Query by tag
                var foundDevices = await registryManager.QueryDevicesAsync(new[] { "bacon" }, 100);
                PrintDevices(foundDevices);
            }
            catch (Exception e)
            {
                Console.WriteLine("Cannot query devices by tags:");
                Console.WriteLine(e.ToString());
            }
        }

        public async static Task QueryFilter()
        {
            // Create JSON query string
            var query = JsonConvert.SerializeObject(
                new
                {
                    filter = new
                    {
                        property = new
                        {
                            name = "CustomerId",
                            type = "service"
                        },
                        value = "123456",
                        comparisonOperator = "eq",
                        type = "comparison"
                    }
                }
            );

            // Execute query
            var foundDevices = (await registryManager.QueryDevicesJsonAsync(query)).Result;
            PrintDevices(foundDevices);
        }
        
        public async static Task QueryAggregate()
        {
            // Create JSON query string
            var query = JsonConvert.SerializeObject(
                new
                {
                    filter = new
                    {
                        property = new
                        {
                            name = "CustomerId",
                            type = "service"
                        },
                        value = (string)null,
                        comparisonOperator = "ne",
                        type = "comparison"
                    },
                    aggregate = new
                    {
                        keys = new[] 
                        {
                            new
                            {
                                name = "CustomerId",
                                type = "service"
                            }
                        },
                        properties = new[] 
                        {
                            new
                            {

                                @operator = "sum",
                                property = new
                                {
                                    name = "Weight",
                                    type = "service"
                                },
                                columnName = "TotalWeight"
                            }
                        }
                    },
                    sort = new string[] { }
                }
            );

            // Execute aggregate query
            var results = (await registryManager.QueryDevicesJsonAsync(query)).AggregateResult;

            // Print devices
            foreach (var r in results)
            {
                Console.WriteLine("CustomerId: {0}\t\tTotal Weight {1}", r["CustomerId"], r["TotalWeight"]);
            }
         }

        public async static Task QuerySort()
        {
            // Create JSON query string
            var query = JsonConvert.SerializeObject(
                new
                {
                    sort = new[]
                    {
                        new
                        {
                            property = new
                            {
                                name = "QoS",
                                type = "service"
                            },
                            order = "asc"
                        }
                    }
                }
            );

            // Execute query
            IEnumerable<Device> foundDevices = (await registryManager.QueryDevicesJsonAsync(query)).Result;
            PrintDevices(foundDevices);
        }

        public async static Task DevicePropertyQuery()
        {
            Console.WriteLine();
            Console.WriteLine("Devices with FirmwareVersion == '1.0'");
            Console.WriteLine();

            // Create JSON query string
            var query = JsonConvert.SerializeObject(
                new
                {
                    filter = new
                    {
                        property = new
                        {
                            name = "FirmwareVersion",
                            type = "device"
                        },
                        value = "1.0",
                        comparisonOperator = "eq",
                        type = "comparison"
                    }
                }
            );

            // Execute query
            IEnumerable<Device> foundDevices = (await registryManager.QueryDevicesJsonAsync(query)).Result;
            PrintDevices(foundDevices);

            Console.WriteLine();
            Console.WriteLine("Devices with FirmwareVersion != '1.0'");
            Console.WriteLine();

            // Create JSON query string
            query = JsonConvert.SerializeObject(
                new
                {
                    filter = new
                    {
                        property = new
                        {
                            name = "FirmwareVersion",
                            type = "device"
                        },
                        value = "1.0",
                        comparisonOperator = "ne",
                        type = "comparison"
                    },
                }
            );

            // Execute query
            foundDevices = (await registryManager.QueryDevicesJsonAsync(query)).Result;
            PrintDevices(foundDevices);
        }

        public static void PrintDevices(IEnumerable<Device> foundDevices)
        {
            foreach (Device d in foundDevices)
            {
                Console.Write("{0}\n-- Tags : ", d.Id);
                foreach (var tag in d.ServiceProperties.Tags)
                {
                    Console.Write(" {0} ", tag);
                }
                Console.Write("\n-- Service Properties : ");
                foreach (var prop in d.ServiceProperties.Properties)
                {
                    Console.Write(" {0}:{1}", prop.Key, prop.Value);
                }
                Console.Write("\n-- Device Properties : ");
                foreach (var prop in d.DeviceProperties)
                {
                    Console.Write(" {0}:{1}", prop.Key, prop.Value.Value);
                }
                Console.WriteLine();

            }
        }
    }
}
