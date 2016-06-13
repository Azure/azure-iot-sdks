namespace e2e_ss
{
    using Microsoft.Azure.Devices;
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using SysTimer = System.Timers.Timer;

    class Program
    {
        const int    OneSecond = 1000;
        const int    OneMinute = 60 * OneSecond;
        const string WritePropertyError = "ERROR_WRITING_TO_DEVICE";

        static string             _cs;
        static JobClient          _dj;
        static RegistryManager    _rm;
        static ManualResetEvent   _clientReady = new ManualResetEvent(false);
        static string             _deviceId = "SimpleSample_" + Guid.NewGuid().ToString();
        static bool               _registered = false;
        static SysTimer           _closer;

        static Dictionary<String, TestCase> _ReadTestCases = new Dictionary<string, TestCase>();
        static Dictionary<String, TestCase> _WriteTestCases = new Dictionary<string, TestCase>();
        static Dictionary<String, TestCase> _ExecuteTestCases = new Dictionary<string, TestCase>();
        static Dictionary<String, TestCase> _ObserveTestCases = new Dictionary<string, TestCase>();


        static void ListAllSimpleSampleTestDevices()
        {
            Task<IEnumerable<Device>> results = _rm.GetDevicesAsync(500);
            results.Wait();
            var devices = results.Result;

            foreach (var device in devices)
            {
                Console.WriteLine("*device: {0}", device.Id);
            }
        }


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


        static String Register()
        {
            Task<Device> newDevice = _rm.AddDeviceAsync(new Device(_deviceId));
            newDevice.Wait();
            var device = newDevice.Result;

            Console.WriteLine("Registered device: {0}", device.Id);

            string hostName = _cs.Split(new string[] { ";" }, StringSplitOptions.None)[0];
            StringBuilder deviceCreds = new StringBuilder();
            deviceCreds.Append(String.Format("{0};DeviceId={1};SharedAccessKey={2}{3}", hostName, device.Id,
                device.Authentication.SymmetricKey.PrimaryKey, System.Environment.NewLine));
            return deviceCreds.ToString(0, deviceCreds.Length - 2);
        }


        static String getProperty(String property)
        {
            Task<Device> getDeviceTask = _rm.GetDeviceAsync(_deviceId);
            getDeviceTask.Wait();

            Device device = getDeviceTask.Result;
            string retValue = String.Empty;
            try
            {
                retValue = device.DeviceProperties[property].Value.ToString();
            }

            catch (Exception ex)
            {
                Console.WriteLine("{0}: {1}", property, ex.Message);
            }

            return retValue;
        }


        private static void onDataOut(object caller, DataReceivedEventArgs eArgs)
        {
            if ((string.IsNullOrEmpty(eArgs.Data) == false) && eArgs.Data.StartsWith("Info: "))
            {
                if (eArgs.Data.Contains("REGISTERED"))
                {
                    _registered = true;
                    Console.WriteLine("Device connected to DM channel");
                }

                else if (true == _registered)
                {
                    if (eArgs.Data.Contains("** ")) Console.WriteLine(eArgs.Data);
                    if (eArgs.Data.Contains("returning"))
                    {
                        // this is a 'read' operation
                        string[] data = eArgs.Data.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                        string propertyName = data[data.Length - 1];
                        if (_ReadTestCases.ContainsKey(propertyName))
                        {
                            var aCase = _ReadTestCases[propertyName];
                            if (null == aCase.ExpectedValue)
                            {
                                string rawValue = data[2];
                                if ((rawValue.IndexOf('[') == 0) && (rawValue.IndexOf(']') > 0))
                                {
                                    rawValue = rawValue.Substring(1, rawValue.Length - 2);
                                }

                                aCase.ExpectedValue = rawValue;
                            }
                        }
                    }

                    else if (eArgs.Data.Contains("being set"))
                    {
                        // this is a 'write' operation
                        string[] data = eArgs.Data.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                        string propertyName = data[1];
                        if (_WriteTestCases.ContainsKey(propertyName))
                        {
                            var aCase = _WriteTestCases[propertyName];
                            if (null == aCase.RecordedValue)
                            {
                                string rawValue = data[data.Length - 1];
                                if ((rawValue.IndexOf('[') == 0) && (rawValue.IndexOf(']') > 0))
                                {
                                    rawValue = rawValue.Substring(1, rawValue.Length - 2);
                                }

                                aCase.RecordedValue = rawValue;
                            }
                        }
                    }

                    else if (eArgs.Data.Contains("inside execute"))
                    {
                        // this is an 'execute' operation
                        string[] data = eArgs.Data.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                        string propertyName = data[data.Length - 1];

                        if (_ExecuteTestCases.ContainsKey(propertyName))
                        {
                            _ExecuteTestCases[propertyName].RecordedValue = "true";
                        }
                    }
                }
            }

            /** If all test cases have been run */
            bool done = true;
            foreach (var aCase in _ReadTestCases)
            {
                done &= aCase.Value.HasRun;
                if (done == false) break;
            }

            foreach (var aCase in _ObserveTestCases)
            {
                done &= aCase.Value.HasRun;
                if (done == false) break;
            }

            foreach (var aCase in _WriteTestCases)
            {
                done &= aCase.Value.HasRun;
                if (done == false) break;
            }

            foreach (var aCase in _ExecuteTestCases)
            {
                done &= aCase.Value.HasRun;
                if (done == false) break;
            }

            if (done)
            {
                _clientReady.Set();
            }
        }


        private static void onErrorOut(object caller, DataReceivedEventArgs eArgs)
        {
            if (string.IsNullOrEmpty(eArgs.Data) == false)
            {
                if (eArgs.Data.StartsWith("Error:") && eArgs.Data.Contains("Registration FAILED"))
                {
                    _registered = false;
                    Console.WriteLine("*Device failed to connect to DM channel");

                    _clientReady.Set();
                }

                else if (eArgs.Data.StartsWith("Observe Update"))
                {
                    string[] data = eArgs.Data.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    string uRI = data[1].Substring(7, data[0].Length - 1);
                    if (_ObserveTestCases.ContainsKey(uRI))
                    {
                        TestCase aCase = _ObserveTestCases[uRI];
                        aCase.ExpectedValue = data[2];
                        aCase.RecordedValue = ReadPropertyThroughService(aCase.Name);
                        Console.WriteLine("Property: {0} Observed '{1}'", aCase.Name, aCase.RecordedValue);
                    }
                }
            }
        }


        class DmClient
        {
            private Process process_;

            private DmClient(String exeName, String connectionString)
            {
                var startInfo = new ProcessStartInfo()
                {
                    FileName = exeName,
                    CreateNoWindow = true,
                    UseShellExecute = false,
                    Arguments = "-cs " + connectionString,
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
                try
                {
                    process_.Kill();
                    process_.WaitForExit();
                }

                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        private class TestCase
        {
            public enum TestType { Read, Write, Observe, Execute };

            public String Name { get; }

            private TestType type_;
            public TestType Type
            {
                get { return type_; }
                private set
                {
                    this.type_ = value;
                    if (value == TestType.Execute)
                    {
                        this.ExpectedValue = "true";
                    }
                }
            }

            public String RecordedValue { get; set; }
            public object ExpectedValue { get; set; }

            public TestCase(String name, TestType type)
            {
                this.Name = name;
                this.Type = type;
            }

            public bool IsValid
            {
                get
                {
                    string message = this.Type.ToString() + "_" + this.Name;
                    if (HasRun) return AreEqual(ExpectedValue.ToString(), RecordedValue, message);

                    Console.WriteLine(message + " failed to run");
                    return false;
                }
            }

            public bool HasRun
            {
                get { return ((this.ExpectedValue != null) && (this.RecordedValue != null)); }
            }
        }


        static void RunTestCases()
        {
            DateTime start = DateTime.Now;

            try
            {
                _dj = JobClient.CreateFromConnectionString(_cs);

                /** wait for the service/client handshake to finish */
                Thread.Sleep(100 * OneSecond);

                while (_registered == false)
                {
                    Thread.Sleep(OneSecond);
                }

                Console.WriteLine();
                foreach (var one in _ReadTestCases)
                {
                    Console.WriteLine("TestCase: Read({0})", one.Key);
                    one.Value.ExpectedValue = null;
                    one.Value.RecordedValue = ReadPropertyThroughService(one.Value.Name);
                    Console.WriteLine("\t{0} is reported by the service as '{1}'", one.Value.Name, one.Value.RecordedValue);
                }

                Console.WriteLine();
                foreach (var one in _WriteTestCases)
                {
                    one.Value.RecordedValue = null;
                    if (false == WritePropertyThroughService(one.Value.Name, one.Value.ExpectedValue))
                    {
                        one.Value.RecordedValue = WritePropertyError;
                    }
                }

                foreach (var one in _ExecuteTestCases)
                {
                    ExecuteResourceThroughService(one.Value.Name);
                }
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            TimeSpan ts = DateTime.Now.Subtract(start);
            Console.WriteLine("\nE2E took {0} seconds.", ts.TotalSeconds);
        }


        /**
         *  arg[0] = full path to iotdm_simple_sample binary.
         *  arg[1] = IoT Hub connection string.
         */
        static int Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("Usage: ");
                Console.WriteLine("  - to run the tests: 2e_ss.exe <clientEXE> <IotHub Connection String>");
                Console.WriteLine("  - to delete stale test devices: 2e_ss.exe --cleanupRegistrar <IotHub Connection String>");

                return -1;
            }

            _cs = args[1];
            _rm = RegistryManager.CreateFromConnectionString(_cs);

            if (args[0].Equals("--cleanupRegistrar"))
            {
                UnregisterStaleTestDevices();

                return 0;
            }

            String connectionString = Register();
            var client = DmClient.Start(args[0], connectionString);

            // the 'read' test cases.
            var oneCase = new TestCase(DevicePropertyNames.FirmwareVersion, TestCase.TestType.Read);
            _ReadTestCases.Add("Device_FirmwareVersion", oneCase);
            oneCase = new TestCase(DevicePropertyNames.BatteryStatus, TestCase.TestType.Read);
            _ReadTestCases.Add("Device_BatteryStatus", oneCase);
            //oneCase = new TestCase(DevicePropertyNames.BatteryLevel, TestCase.TestType.Read);
            //_ReadTestCases.Add("Device_BatteryLevel", oneCase);
            oneCase = new TestCase(DevicePropertyNames.HardwareVersion, TestCase.TestType.Read);
            _ReadTestCases.Add("Device_HardwareVersion", oneCase);
            oneCase = new TestCase(DevicePropertyNames.SerialNumber, TestCase.TestType.Read);
            _ReadTestCases.Add("Device_SerialNumber", oneCase);
            oneCase = new TestCase(DevicePropertyNames.MemoryFree, TestCase.TestType.Read);
            _ReadTestCases.Add("Device_MemoryFree", oneCase);
            oneCase = new TestCase(DevicePropertyNames.ModelNumber, TestCase.TestType.Read);
            _ReadTestCases.Add("Device_ModelNumber", oneCase);
            oneCase = new TestCase(DevicePropertyNames.Manufacturer, TestCase.TestType.Read);
            _ReadTestCases.Add("Device_Manufacturer", oneCase);
            oneCase = new TestCase(DevicePropertyNames.DefaultMaxPeriod, TestCase.TestType.Read);
            _ReadTestCases.Add("LWM2MServer_DefaultMaximumPeriod", oneCase);
            oneCase = new TestCase(DevicePropertyNames.DefaultMinPeriod, TestCase.TestType.Read);
            _ReadTestCases.Add("LWM2MServer_DefaultMinimumPeriod", oneCase);
            oneCase = new TestCase(DevicePropertyNames.RegistrationLifetime, TestCase.TestType.Read);
            _ReadTestCases.Add("LWM2MServer_Lifetime", oneCase);

            // the 'write' test cases
            oneCase = new TestCase(DevicePropertyNames.UtcOffset, TestCase.TestType.Write);
            oneCase.ExpectedValue = "-10:00"; /* US/Hawaii timezone */
            _WriteTestCases.Add("Device_UtcOffset", oneCase);
            oneCase = new TestCase(DevicePropertyNames.BatteryLevel, TestCase.TestType.Write);
            oneCase.ExpectedValue = WritePropertyError;
            _WriteTestCases.Add("Device_BatteryLevel", oneCase);
            oneCase = new TestCase(DevicePropertyNames.RegistrationLifetime, TestCase.TestType.Write);
            oneCase.ExpectedValue = 10;
            _WriteTestCases.Add("LWM2MServer_Lifetime", oneCase);

            // the 'execute' test cases
            oneCase = new TestCase("Device_FactoryReset", TestCase.TestType.Execute);
            _ExecuteTestCases.Add("Device_FactoryReset", oneCase);
            oneCase = new TestCase("Device_Reboot", TestCase.TestType.Execute);
            _ExecuteTestCases.Add("Device_Reboot", oneCase);
            oneCase = new TestCase("FirmwareUpdate_Update", TestCase.TestType.Execute);
            _ExecuteTestCases.Add("FirmwareUpdate_Update", oneCase);

            // the 'observe' test cases
            /**
             * Note that we are using a URI - This is a side effect of wakaama messages on stderr...
             *    Let's figure out a more generic way of identifying the URI!
             */
            oneCase = new TestCase(DevicePropertyNames.BatteryLevel, TestCase.TestType.Observe);
            _ObserveTestCases.Add("/3/0/9", oneCase);
            oneCase = new TestCase(DevicePropertyNames.FirmwareVersion, TestCase.TestType.Observe);
            _ObserveTestCases.Add("/3/0/3", oneCase);
            oneCase = new TestCase(DevicePropertyNames.FirmwareUpdateState, TestCase.TestType.Observe);
            _ObserveTestCases.Add("/5/0/3", oneCase);

            Thread thread = new Thread(new ThreadStart(RunTestCases));
            thread.Start();

            /**
             *  Start a timer for the process. The interval should be large enough to validate all tests.
             *  Care must be taken to ensure PMIN and PMAX, for example, are honored.
             */
            _closer = new SysTimer(35 * OneMinute);
            _closer.Elapsed += (sender, e) =>
            {
                _clientReady.Set();
            };

            _closer.Start();
            _clientReady.WaitOne();
            if (_registered)
            {
                client.Stop();
            }

            thread.Abort();
            Unregister();

            /** report -- */
            int nrErrorCases = 0;
            foreach (var aCase in _ReadTestCases)
            {
                if (false == aCase.Value.IsValid)
                {
                    nrErrorCases ++;
                }
            }

            foreach (var aCase in _WriteTestCases)
            {
                if (false == aCase.Value.IsValid)
                {
                    nrErrorCases++;
                }
            }

            foreach (var aCase in _ExecuteTestCases)
            {
                if (false == aCase.Value.IsValid)
                {
                    nrErrorCases++;
                }
            }

            foreach (var aCase in _ObserveTestCases)
            {
                if (false == aCase.Value.IsValid)
                {
                    nrErrorCases++;
                }
            }

            int nrCases = _ReadTestCases.Count + _WriteTestCases.Count + _ExecuteTestCases.Count + _ObserveTestCases.Count;
            Console.WriteLine("\n--------\n\t{0} cases failed out of {1} tests.\n--------\n", nrErrorCases, nrCases);

            return nrErrorCases;
        }


        static bool AreEqual(string expected, string actual, string message)
        {
            if (!String.Equals(expected, actual))
            {
                Console.Write("ERROR: Expected '{0}' to equal '{1}'", expected, actual);
                Console.WriteLine(" {0}", (message == null) ? String.Empty : message);

                return false;
            }

            return true;
        }


        static string ReadPropertyThroughService(string propertyName)
        {
            try
            {
                var jobID = "Read" + propertyName + Guid.NewGuid().ToString();

                DateTime start = DateTime.Now;
                Task<JobResponse> job = _dj.ScheduleDevicePropertyReadAsync(jobID, _deviceId, propertyName);
                job.Wait();
                JobResponse rs = job.Result;
                while (rs.Status < JobStatus.Completed)
                {
                    job = _dj.GetJobAsync(jobID);
                    job.Wait();
                    rs = job.Result;
                }

                TimeSpan ts = DateTime.Now.Subtract(start);
                Console.WriteLine("\tRead Property('{0}') took {1} seconds.", propertyName, ts.TotalSeconds);

                string rv = getProperty(propertyName);

                return rv;
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            return String.Empty;
        }


        static bool WritePropertyThroughService(string propertyName, object propertyValue)
        {
            try
            {
                var jobID = "Write" + propertyName + Guid.NewGuid().ToString();

                Task<JobResponse> job = _dj.ScheduleDevicePropertyWriteAsync(jobID, _deviceId, propertyName, propertyValue);
                job.Wait();
                JobResponse rs = job.Result;
                while (rs.Status < JobStatus.Completed)
                {
                    job = _dj.GetJobAsync(jobID);
                    job.Wait();
                    rs = job.Result;
                }

                return ((rs.Status == JobStatus.Completed) && rs.StatusMessage.Equals("Devices: 1; Succeeded: 1; Failed: 0;"));
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            return false;
        }


        static bool ExecuteResourceThroughService(string resourceName)
        {
            try
            {
                var jobID = "Execute" + Guid.NewGuid().ToString();

                Task<JobResponse> job = null;
                if (resourceName.Equals("Device_FactoryReset"))
                {
                    job = _dj.ScheduleFactoryResetDeviceAsync(jobID, _deviceId);
                }

                else if (resourceName.Equals("Device_Reboot"))
                {
                    job = _dj.ScheduleRebootDeviceAsync(jobID, _deviceId);
                }

                else if (resourceName.Equals("FirmwareUpdate_Update"))
                {
                    job = _dj.ScheduleFirmwareUpdateAsync(jobID, _deviceId, "https://FakeURI", TimeSpan.FromMinutes(25));
                }

                if (job != null)
                {
                    job.Wait();
                    JobResponse rs = job.Result;
                    while (rs.Status < JobStatus.Completed)
                    {
                        job = _dj.GetJobAsync(jobID);
                        job.Wait();
                        rs = job.Result;
                    }

                    return (rs.Status == JobStatus.Completed);
                }
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            return false;
        }
    }
}
