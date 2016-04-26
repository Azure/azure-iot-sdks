using System;
using System.Collections.Generic;

using System.IO;


namespace Setup
{
    public class Setup
    {
        public static string deviceCreds = "devicecreds.txt";
        public static string deviceStructure = "sampledevices.json";

        public static List<string> ReadDeviceID()
        {
            List<string> idList = new List<string>();
            try
            {
                // Read all device IDs from file
                Console.WriteLine("Reading Device IDs from file...");
                string[] lines = File.ReadAllLines(deviceCreds);

                 foreach(string line in lines)
                {
                    if (!string.IsNullOrEmpty(line))
                    {
                        var deviceKeyParts = line.Split(';');
                        var deviceIDParts = deviceKeyParts[1].Split('=');
                        idList.Add(deviceIDParts[1]);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception in ReadDeviceID:\n + e.Message");
                Console.WriteLine(e.ToString());
            }

            return idList;
        }

    }
}
