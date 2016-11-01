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
        // It is NOT a good practice to put device credentials in the code as shown below.
        // This is done in this sample for simplicity purposes.
        // Good practices such as the use of TPM or other secure storage on the device are recommended to store device credentials.
        // String containing Hostname, Device Id & Device Key in one of the following formats:
        //  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
        //  "HostName=<iothub_host_name>;CredentialType=SharedAccessSignature;DeviceId=<device_id>;SharedAccessSignature=SharedAccessSignature sr=<iot_host>/devices/<device_id>&sig=<token>&se=<expiry_time>";
        private const string DeviceConnectionString = "<replace>";
        private const string FilePath = "<replace>";

        static void Main(string[] args)
        {
            try
            {
                SendToBlobSample().Wait();
            }
            catch (Exception ex)
            {
                Console.WriteLine("{0}\n", ex.Message);
                if (ex.InnerException != null)
                {
                    Console.WriteLine(ex.InnerException.Message + "\n");
                }
            }
        }

        static async Task SendToBlobSample()
        {
            var deviceClient = DeviceClient.CreateFromConnectionString(DeviceConnectionString, TransportType.Http1);
            var fileStreamSource = new FileStream(FilePath, FileMode.Open);
            var fileName = Path.GetFileName(fileStreamSource.Name);

            Console.WriteLine("Uploading File: {0}", fileName);

            var watch = System.Diagnostics.Stopwatch.StartNew();
            await deviceClient.UploadToBlobAsync(fileName, fileStreamSource);
            watch.Stop();

            Console.WriteLine("Time to upload file: {0}ms\n", watch.ElapsedMilliseconds);
        }
    }
}
