using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace compilembed
{
    // Reference:
    // https://developer.mbed.org/handbook/Compile-API

    class Program
    {
        private const string userNameOption = "-un";
        private const string passwordOption = "-pwd";
        private const string repoOption = "-r";
        private const string platformOption = "-plat";
        private const string downloadOutputLocationOption = "-o";

        static void Main(string[] args)
        {
            MBEDOnlineCompile onlineCompile;
            string userName = string.Empty;
            string password = string.Empty;
            string repo = string.Empty;
            string platform = string.Empty;
            string downloadOutputPath = string.Empty;

            for (int i = 0; i < args.Length; i++)
            {
                if (args[i] == userNameOption)
                {
                    i++;
                    if (i < args.Length)
                    {
                        userName = args[i];
                    }
                }

                if (args[i] == passwordOption)
                {
                    i++;
                    if (i < args.Length)
                    {
                        password = args[i];
                    }
                }

                if (args[i] == repoOption)
                {
                    i++;
                    if (i < args.Length)
                    {
                        repo = args[i];
                    }
                }

                if (args[i] == platformOption)
                {
                    i++;
                    if (i < args.Length)
                    {
                        platform = args[i];
                    }
                }

                if (args[i] == downloadOutputLocationOption)
                {
                    i++;
                    if (i < args.Length)
                    {
                        downloadOutputPath = args[i];
                    }
                }
            }

            if (string.IsNullOrEmpty(userName) ||
                string.IsNullOrEmpty(password) ||
                string.IsNullOrEmpty(repo) ||
                string.IsNullOrEmpty(platform))
            {
                System.Console.WriteLine("Usage:");
                System.Console.WriteLine(string.Format("-{0:10}    UserName to be used to access the MBED online compiler", userNameOption));
                System.Console.WriteLine(string.Format("-{0:10}    Password to be used to access the MBED online compiler", passwordOption));
                System.Console.WriteLine(string.Format("-{0:10}    Repo to be compiled", repoOption));
                System.Console.WriteLine(string.Format("-{0:10}    Platform to be compiled", platformOption));
                System.Console.WriteLine(string.Format("-{0:10}    Path where the compiled binary should be saved on (If not provided, file is not downloaded).", downloadOutputPath));

                Environment.ExitCode = 2;
            }
            else
            {
                onlineCompile = new MBEDOnlineCompile(userName, password);

                System.Console.WriteLine("Starting compile ...");
                onlineCompile.StartCompile(platform, null, repo);

                ICollection<string> messages = new List<string>();
                bool failed = false;

                while (!onlineCompile.PollStatus(messages, out failed))
                {
                    foreach (string msg in messages)
                    {
                        System.Console.WriteLine(msg);
                    }

                    messages.Clear();
                }

                if (failed)
                {
                    System.Console.WriteLine("Compile FAILED!");
                    Environment.ExitCode = 1;
                }
                else
                {
                    if (!String.IsNullOrWhiteSpace(downloadOutputPath))
                    {
                        Console.WriteLine("Downloading compiled binary into '{0}' ...", downloadOutputPath);

                        try
                        {
                            onlineCompile.DownloadBin(downloadOutputPath);
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine("Failed downloading binary file: {0}", ex.ToString());
                        }
                    }

                    System.Console.WriteLine("Done.");
                }
            }
        }
    }
}
