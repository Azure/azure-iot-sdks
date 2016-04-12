namespace e2e_ss
{
    using Microsoft.Azure.Devices;
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;

    class Program
    {
        static string           _cs;
        static string           _sscs;
        static RegistryManager  _rm;
        static bool             _registered = false;
        static ManualResetEvent _clientReady = new ManualResetEvent(false);
        static string           _deviceId = "SimpleSample_" + Guid.NewGuid().ToString();

        static string           _clientBatteryLevel;
        static string           _clientFirmwareVersion;

        static void UnregisterStaleTestDevices()
        {
            Task<IEnumerable<Device>> results = _rm.GetDevicesAsync(500);
            results.Wait();
            var devices = results.Result;

            foreach (var device in devices)
            {
                if (device.Id.StartsWith("SimpleSample_"))
                {
                    Unregister(device);
                }
            }
        }

        static void Unregister(Device device)
        {
            try
            {
                _rm.RemoveDeviceAsync(device).Wait();
                Console.WriteLine("*Unregistered device: {0}", device.Id);
            }

            catch (Exception x)
            {
                Console.WriteLine("Exception: {0}", x.ToString());
            }
        }

        static void Unregister()
        {
            Task<Device> getDeviceTask = _rm.GetDeviceAsync(_deviceId);
            getDeviceTask.Wait();

            Device device = getDeviceTask.Result;
            Unregister(device);
        }

        static void Register()
        {
            Task<Device> newDevice = _rm.AddDeviceAsync(new Device(_deviceId));
            newDevice.Wait();
            var device = newDevice.Result;

            Console.WriteLine("*Registered device: {0}", device.Id);

            string hostName = _cs.Split(new string[] { ";" }, StringSplitOptions.None)[0];
            StringBuilder deviceCreds = new StringBuilder();
            deviceCreds.Append(String.Format("{0};DeviceId={1};SharedAccessKey={2}{3}", hostName, device.Id,
                device.Authentication.SymmetricKey.PrimaryKey, System.Environment.NewLine));
            _sscs = deviceCreds.ToString(0, deviceCreds.Length - 2);
        }

        static String getProperty(String property)
        {
            Task<Device> getDeviceTask = _rm.GetDeviceAsync(_deviceId);
            getDeviceTask.Wait();

            Device device = getDeviceTask.Result;
            return device.SystemProperties[property].Value.ToString();
        }

        private static void onDataOut(object caller, DataReceivedEventArgs eArgs)
        {
            if (string.IsNullOrEmpty(eArgs.Data) == false)
            {
                /* looking for: 'Info: returning %d/%s for <PropertyName>' */
                if (eArgs.Data.StartsWith("Info: returning"))
                {
                    if (String.IsNullOrEmpty(_clientFirmwareVersion) && eArgs.Data.Contains("Device_FirmwareVersion"))
                    {
                        string[] data = eArgs.Data.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                        _clientFirmwareVersion = data[2].Substring(1, data[2].Length - 2);
                    }
                    else if (String.IsNullOrEmpty(_clientBatteryLevel) && eArgs.Data.Contains("Device_BatteryLevel"))
                    {
                        string[] data = eArgs.Data.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                        _clientBatteryLevel = data[2];
                    }
                }

                else if (eArgs.Data.Contains("REGISTERED"))
                {
                    _registered = true;
                    Console.WriteLine("*Device connected to DM channel");
                }

                if (_registered &&
                    !String.IsNullOrEmpty(_clientFirmwareVersion) &&
                    !String.IsNullOrEmpty(_clientBatteryLevel))
                {
                    _clientReady.Set();
                }
            }
        }

        private static void onErrorOut(object caller, DataReceivedEventArgs eArgs)
        {
            if ((string.IsNullOrEmpty(eArgs.Data) == false))
            {
            }
        }

        class DmClient
        {
            private Process process_;

            private DmClient(String exeName, String connectionString)
            {
                var startInfo = new ProcessStartInfo() {
                    FileName = exeName,
                    CreateNoWindow = true,
                    UseShellExecute = false,
                    Arguments = connectionString,
                    RedirectStandardError = true,
                    RedirectStandardOutput = true
                };

                // can throw file not found exception
                process_ = Process.Start(startInfo);
                if (process_ != null)
                {
                    process_.EnableRaisingEvents = true;
                    process_.OutputDataReceived += new DataReceivedEventHandler(onDataOut);
                    process_.ErrorDataReceived += new DataReceivedEventHandler(onErrorOut);

                    process_.BeginOutputReadLine();
                    process_.BeginErrorReadLine();
                }
            }

            public static DmClient Start(String exeName, String connectionString)
            {
                var dmClient = new DmClient(exeName, connectionString);
                return (dmClient.process_ == null) ? null : dmClient;
            }

            public void Stop()
            {
                process_.Kill();
                process_.WaitForExit();
            }
        }

        /**
         *  arg[0] = full path to iotdm_simple_sample binary.
         *  arg[1] = IoT Hub connection string.
         */
        static int Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("Usage: e2e_ss.exe <clientEXE> <IotHub Connection String>");
            }

            _cs = args[1];
            _rm = RegistryManager.CreateFromConnectionString(_cs);

            UnregisterStaleTestDevices(); // TODO: We don't want to do this here, mulitple instances of this app will duel for devices
            Register();

            var client = DmClient.Start(args[0], _sscs);

            _clientReady.WaitOne();

            // client is ready, but server probably is not
            Thread.Sleep(6000);

            bool passed =
                VerifyAutoSyncProperties() &&
                VerifyWriteReadCycle();

            client.Stop();
            Unregister();

            Console.WriteLine("Tests " + (passed ? "PASSED" : "FAILED") + ".");
            return passed ? 0 : 1;
        }

        static bool AreEqual(string expected, string actual, string message)
        {
            if (!String.Equals(expected, actual))
            {
                Console.Write("ERROR: Expected '{0}' to equal '{1}'", expected, actual);

                if (message == null)
                {
                    Console.WriteLine(String.Empty);
                }
                else
                {
                    Console.WriteLine(" {0}", message);
                }

                return false;
            }

            return true;
        }

        static bool ServiceValueEqualsClientValue(string name, string clientValue)
        {
            String serviceValue = getProperty(name);
            return AreEqual(clientValue, serviceValue, name);
        }

        static bool VerifyAutoSyncProperties()
        {
            Console.WriteLine("+Verify auto-sync properties");
            return
                ServiceValueEqualsClientValue(SystemPropertyNames.BatteryLevel, _clientBatteryLevel) &&
                ServiceValueEqualsClientValue(SystemPropertyNames.FirmwareVersion, _clientFirmwareVersion);
        }

        static bool VerifyWriteReadCycle()
        {
            Console.WriteLine("+Write, then read property");

            var valueToWrite = "-10:00"; // US/Hawaii timezone
            var valueRead = String.Empty;
            if (WritePropertyThroughService(SystemPropertyNames.Timezone, valueToWrite))
            {
                valueRead = ReadPropertyThroughService(SystemPropertyNames.Timezone);
            }

            return AreEqual(valueToWrite, valueRead, SystemPropertyNames.Timezone);
        }

        static string ReadPropertyThroughService(string propertyName)
        {
            try
            {
                var deviceJobClient = JobClient.CreateFromConnectionString(_cs);
                var jobID = Guid.NewGuid().ToString();
                Task<JobResponse> job = deviceJobClient.ScheduleSystemPropertyReadAsync(jobID, _deviceId, SystemPropertyNames.FirmwareVersion);
                job.Wait();
                JobResponse rs = job.Result;
                while (rs.Status < JobStatus.Completed)
                {
                    Thread.Sleep(2000);
                    job = deviceJobClient.GetJobAsync(jobID);
                    job.Wait();
                    rs = job.Result;
                }

                if (rs.Status == JobStatus.Completed)
                {
                    return getProperty(propertyName);
                }
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            return String.Empty;
        }

        static bool WritePropertyThroughService(string propertyName, string propertyValue)
        {
            try
            {
                var deviceJobClient = JobClient.CreateFromConnectionString(_cs);
                var jobID = Guid.NewGuid().ToString();

                Task<JobResponse> job = deviceJobClient.ScheduleSystemPropertyWriteAsync(jobID, _deviceId, propertyName, propertyValue);
                job.Wait();
                JobResponse rs = job.Result;
                while (rs.Status < JobStatus.Completed )
                {
                    Thread.Sleep(2000);
                    job = deviceJobClient.GetJobAsync(jobID);
                    job.Wait();
                    rs = job.Result;
                }

                return (rs.Status == JobStatus.Completed);
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            return false;
        }
    }
}
