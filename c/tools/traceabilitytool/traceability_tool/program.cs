// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System;                // Used for StringComparison class
using System.Windows.Forms;  // Used for Application class
using System.IO;             // Used for StreamWriter class
using System.Runtime.InteropServices;  // Used for DllImport
using System.Diagnostics;    // Used for Process class


namespace TraceabilityTool
{
    // This class is responsible for starting the program as a GUI application or as a command-line-based application.
    // It allows to process command line parameters.
    static class Program
    {
        // Directory to look for Word documents with requirements and source code with implementaiton/tests in.
        public static string inputDir = "";
        // Directory to place reports into.
        public static string outputDir = "";
        // directories to exclude
        public static string[] exclusionDirs = null;
        // Exit program with code 0 if correct parameters were supplied.
        public static int exitCode = 0;

        [DllImport("user32.dll")]
        static extern IntPtr GetForegroundWindow();

        [DllImport("user32.dll", SetLastError = true)]
        static extern uint GetWindowThreadProcessId(IntPtr hWnd, out int lpdwProcessId);

        [DllImport("kernel32", SetLastError = true)]
        static extern bool AttachConsole(int dwProcessId);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool FreeConsole();


        private static bool enableConsole()
        {
            IntPtr ptr = GetForegroundWindow();
            int cmdProcessID;
            bool consoleEnabled = false;

            GetWindowThreadProcessId(ptr, out cmdProcessID);
            Process process = Process.GetProcessById(cmdProcessID);
            if (process.ProcessName.ToLower() == "cmd")
            {
                // The uppermost window is a cmd process.  The console is already running.
                AttachConsole(process.Id);
                consoleEnabled = true;
            }

            return consoleEnabled;
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        /// 
        [STAThread]
        public static void Main(string[] args)
        {
            bool inputDirValid = false;
            bool outputDirValid = false;
            bool exclusionDirsValid = false;
            bool buildCheck = false;
            bool optionGUI = false;
            // Command line interface can be enabled as a command line option
            bool consoleEnabled = false;

            if (args.Length > 0)
            {
                for (int i = 0; i < args.Length; i++)
                {
                    // Find the input directory paramter, which should follow the "-i" parameter
                    if (args[i].Equals("-i", StringComparison.OrdinalIgnoreCase) && (i < args.Length - 1))
                    {
                        inputDirValid = Directory.Exists(args[i + 1]);
                        if (inputDirValid)
                            inputDir = args[i + 1];
                    }

                    // Find the output directory parameter, which should follow the "-o" parameter
                    if (args[i].Equals("-o", StringComparison.OrdinalIgnoreCase) && (i < args.Length - 1))
                    {
                        outputDirValid = Directory.Exists(args[i + 1]);
                        if (outputDirValid)
                        {
                            outputDir = args[i + 1];
                        }
                    }

                    // The "GUI" option enables GUI mode of operation as opposed to command-line mode.
                    if (args[i].Equals("-GUI", StringComparison.OrdinalIgnoreCase))
                        optionGUI = true;

                    // The "CSV" option forces reports to be generated in the comma-separated values (CSV) format in addition to plain text reports.
                    if (args[i].Equals("-CSV", StringComparison.OrdinalIgnoreCase))
                        MainForm.outputCSV = true;

                    // The "BUILDCHECK" option forces reports to be generated in the comma-separated values (CSV) format in addition to plain text reports.
                    if (args[i].Equals("-BUILDCHECK", StringComparison.OrdinalIgnoreCase))
                    {
                        MainForm.buildCheck = true;
                        buildCheck = true;
                    }

                    if (args[i].Equals("-e", StringComparison.OrdinalIgnoreCase) && (i < args.Length - 1))
                    {
                        string[] dirs = args[i + 1].Split(';');
                        exclusionDirsValid = true;
                        foreach (string d in dirs)
                        {
                            exclusionDirsValid &= Directory.Exists(d);
                        }
                        if (exclusionDirsValid)
                        {
                            exclusionDirs = dirs;
                        }
                    }
                }

                // parse out incompatible options
                if ((buildCheck && (optionGUI || MainForm.outputCSV || outputDirValid)) ||
                     (!buildCheck && (!outputDirValid && !optionGUI)))
                {
                    Usage();
                    exitCode = 1;
                }

                if (!optionGUI && !inputDirValid)
                {
                    Usage();
                    exitCode = 1;
                }
            }
            else
            {
                //Default to enable console if Usage is wrong.
                consoleEnabled = enableConsole();
                Usage();
                exitCode = 1;
            }
            



            if (exitCode == 0)
            {


                if (!optionGUI)
                {
                    consoleEnabled = enableConsole();
                }

                if (optionGUI)
                {
                    // GUI enabled
                    Application.EnableVisualStyles();
                    Application.SetCompatibleTextRenderingDefault(false);
                    Application.Run(new MainForm());
                }
                else
                {
                    // The minimum required parameters were correctly specified.  Run the reports in CLI mode
                    exitCode = ReportGenerator.GenerateReport(inputDir, outputDir, exclusionDirs, null);
                    Console.WriteLine();
                    if (outputDirValid)
                    {
                        Console.WriteLine("Reports have been generated in " + outputDir);
                    }
                }

            }


            if (consoleEnabled)
                FreeConsole();

            System.Environment.Exit(exitCode);
        }
        public static void Usage()
        {
            Console.WriteLine("");
            Console.WriteLine("Command line parameters are invalid.");
            Console.WriteLine("Valid parameters:");
            Console.WriteLine("-i <path> to specify the input/root directory path to search for requirements documents and source code in.");
            Console.WriteLine("-o <path> to specify the output directory path to write reports into.");
            Console.WriteLine("-e <path[;path...] to specify directories to exclude from reports.");
            Console.WriteLine("-gui to use the GUI interface.");
            Console.WriteLine("-csv to generate reports in CSV format in addition to default plain text reports.");
            Console.WriteLine("-buildcheck to generate reports to the console for build checking.");
            Console.WriteLine("-buildcheck is incompatible with -o, -gui and -csv");
            Console.WriteLine("When not running in buildcheck mode, input and output directories are required.");
        }
    }


}
