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
        static void Main(string[] args)
        {
            var connectionString = "HostName=arturl-home-hub.azure-devices.net;SharedAccessKeyName=iothubowner;SharedAccessKey=oeB6/pY72ulF8VCBuAStTKo9r4z4K1UcBvHK5tkTq4Q=";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString, TransportType.Amqp);
            var str = "Hello, Cloud!";
            var message = new Message(Encoding.ASCII.GetBytes(str));
            serviceClient.SendAsync("GarageDoorSensor", message).Wait();
            Console.WriteLine("done");
        }
    }
}
