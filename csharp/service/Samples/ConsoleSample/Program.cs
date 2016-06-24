using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Azure.Devices;

namespace ConsoleSample
{
    class Program
    {
        const string connectionString = "...";
        static void Main(string[] args)
        {
            //SendMessage();
            AddDevice();
        }

        static void SendMessage()
        {
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString, TransportType.Amqp);
            var str = "Hello, Cloud!";
            var message = new Message(Encoding.ASCII.GetBytes(str));
            serviceClient.SendAsync("GarageDoorSensor", message).Wait();
            Console.WriteLine("done");
        }

        static void AddDevice()
        {
            RegistryManager manager = RegistryManager.CreateFromConnectionString(connectionString);
            manager.AddDeviceAsync(new Device("new_device"));
        }
    }
}
