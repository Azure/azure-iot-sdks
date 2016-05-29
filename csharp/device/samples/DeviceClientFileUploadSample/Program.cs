// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.Azure.Devices.Client.Samples
{
    class Program
    {
        // String containing Hostname, Device Id & Device Key in one of the following formats:
        //  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
        //  "HostName=<iothub_host_name>;CredentialType=SharedAccessSignature;DeviceId=<device_id>;SharedAccessSignature=SharedAccessSignature sr=<iot_host>/devices/<device_id>&sig=<token>&se=<expiry_time>";
        private const string DeviceConnectionString = "<replace>";

        static void Main(string[] args)
        {
            try
            {
                SendToBlobSample().Wait();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error in sample: {0}", ex.Message);
            }
        }

        static async Task SendToBlobSample()
        {
            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(DeviceConnectionString, TransportType.Http1);
            FileStream fileStreamSource = new FileStream(@"c:\temp\ForFileUpload.txt", FileMode.Open);
            string fileName = Path.GetFileName(fileStreamSource.Name);

            Console.WriteLine("Uploading File: {0}\n", fileName);
            await deviceClient.UploadToBlobAsync(fileName, fileStreamSource);
            Console.WriteLine("Done!\n");
        }
    }
}
