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


        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        /// 
        [STAThread]
        public static void Main(string[] args)
        {
            bool inputDirValid = false;
            bool outputDirValid = false;
            bool optionGUI = false;

            for (int i = 0; i < args.Length; i++)
            {
                // Find the input directory paramter, which should follow the "-i" parameter
                if(args[i].Equals("-i", StringComparison.OrdinalIgnoreCase) && (i < args.Length - 1))
                {
                    inputDirValid = Directory.Exists(args[i+1]);
                    if (inputDirValid)
                        inputDir = args[i + 1];
                }

                // Find the output directory paramter, which should follow the "-o" parameter
                if (args[i].Equals("-o", StringComparison.OrdinalIgnoreCase) && (i < args.Length - 1))
                {
                    outputDirValid = Directory.Exists(args[i + 1]);
                    if (outputDirValid)
                        outputDir = args[i + 1];
                }

                // The "GUI" option enables GUI mode of operation as opposed to command-line mode.
                if (args[i].Equals("-GUI", StringComparison.OrdinalIgnoreCase))
                    optionGUI = true;

                // The "CSV" option forces reports to be generated in the comma-separated values (CSV) format in addition to plain text reports.
                if (args[i].Equals("-CSV", StringComparison.OrdinalIgnoreCase))
                    MainForm.outputCSV = true;
            }


            // Command line interface can be enabled as a command line option
            bool consoleEnabled = false;

            if(!optionGUI)
            {
                // Locate the console (cmd process) from which the program was started if it was invoked from command line.
                IntPtr ptr = GetForegroundWindow();
                int cmdProcessID;
                GetWindowThreadProcessId(ptr, out cmdProcessID);
                Process process = Process.GetProcessById(cmdProcessID);
                if (process.ProcessName == "cmd")
                {
                    // The uppermost window is a cmd process.  The console is already running.
                    AttachConsole(process.Id);
                    consoleEnabled = true;
                }
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
                // Run reports if CLI mode is selected and both directories specified are valid.
                if (inputDirValid && outputDirValid)
                {
                    // The minimum required parameters were correctly specified.  Run the reports.
                    ReportGenerator.GenerateReport(inputDir, outputDir, null);
                    Console.WriteLine();
                    Console.WriteLine("Reports have been generated in " + outputDir);
                }
                else
                {
                    Console.WriteLine("Command line parameters are invalid.");
                    Console.WriteLine("Valid parameters:");
                    Console.WriteLine("-i <path> to specify the input/root directory path to search for requirements documents and source code in.");
                    Console.WriteLine("-o <path> to specify the output directory path to write reports into.");
                    Console.WriteLine("-gui to use the GUI interface.");
                    Console.WriteLine("-csv to generate reports in CSV format in addition to default plain text reports.");
                    Console.WriteLine("Note that both input and output directores must be specified and must be valid in order to run the program in CLI mode.");
                    exitCode = 1;
                }
            }

            if (consoleEnabled)
                FreeConsole();

            System.Environment.Exit(exitCode);
        }
    }
}
