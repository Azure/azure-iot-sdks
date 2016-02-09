// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System;                      // Used for Exception and Console classes
using System.Collections.Generic;  // Used for List class
using System.IO;                   // Used for DirectoryInfo class
using System.Windows.Forms;        // Used for MessageBox class
using System.Linq;

namespace TraceabilityTool
{
    // This class provides a way to generate a list of all files found under
    // a given directory path and matching a given file name pattern.
    class FileFinder
    {
        public static List<string> filterPatternList;

        public static void GetFileList(string rootDirPath, string[] exceptions, ref List<string>FileList)
        {
            System.IO.FileInfo[] files = null;
            System.IO.DirectoryInfo[] subDirs = null;
            System.IO.DirectoryInfo rootDir = new DirectoryInfo(rootDirPath);

            // Return if no file filter pattern has been defined.
            if (filterPatternList.Count == 0)
                return;

            // First, process all the files directly under this folder for each file type
            foreach (string fileFilterPattern in filterPatternList)
            {
                try
                {
                    files = rootDir.GetFiles(fileFilterPattern);
                }
                catch (Exception exception)
                {
                    string message = "An error occurred while attempting to access a files with pattern " + fileFilterPattern + System.Environment.NewLine +
                                     "The error is:  " + exception.Message + System.Environment.NewLine;
                    if(ReportGenerator.useGUI)
                        MessageBox.Show(message, "Error");
                    else
                        Console.WriteLine(message);
                    Program.exitCode = 1;
                }

                if (files != null)
                {
                    // Process all the files in the current directory
                    foreach (System.IO.FileInfo fi in files)
                    {
                        FileList.Add(fi.FullName);
                    }
                }
            }

            // Now find all the subdirectories under this directory.
            try
            {
                subDirs = rootDir.GetDirectories();

                foreach (System.IO.DirectoryInfo dirInfo in subDirs)
                {
                    if ((exceptions== null) || !(exceptions.Any(s => s.Equals(dirInfo.FullName))))
                    {
                        // Resursive call for each subdirectory.
                        GetFileList(dirInfo.FullName, exceptions, ref FileList);
                    }
                }
            }
            catch (Exception exception)
            {
                string message = "An error occurred while attempting to access the folder " + rootDir + System.Environment.NewLine +
                                 "The error is:  " + exception.Message + System.Environment.NewLine;
                if (ReportGenerator.useGUI)
                    MessageBox.Show(message, "Error");
                else
                    Console.WriteLine(message);
                Program.exitCode = 1;
            }
           
        }

        public static void SetFileFilters(List<string>newFilerList)
        {
            filterPatternList = newFilerList;
        }
    }
}
