// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System.Collections.Generic;  // Used for List class

namespace TraceabilityTool
{
    // This class defines a custom data type used for storage of file path and line number
    // of a requirement ID found in a source code file.
    class FilePathLineNum
    {
        public string filePath;
        public int    lineNum;

        public FilePathLineNum(FilePathLineNum location)
        {
            this.filePath = location.filePath;
            this.lineNum  = location.lineNum;
        }

        public FilePathLineNum(string filePath, int lineNum)
        {
            this.filePath = filePath;
            this.lineNum  = lineNum;
        }
    }


    // This class defines a custom dictionary data type used for storage of a requirement ID and
    // corresponding file path and line number of the source code file the requirement ID was found in.
    class ReqPathMatrix : Dictionary<string, List<FilePathLineNum>>
    {
        new public void Add(string key, List<FilePathLineNum> locationList)
        {
            List<FilePathLineNum> locations;

            if (this.ContainsKey(key))
            {
                // Update the existing list of file paths.
                locations = this[key];
            }
            else
            {
                // Create a new list of file paths.
                locations = new List<FilePathLineNum>();
            }

            // Add new paths to the current list of file paths.
            locations.AddRange(locationList);

            // Place the updated file paths list into the dictionary.
            this[key] = locations;
        }

        public void Add(string key, FilePathLineNum location)
        {
            List<FilePathLineNum> locations;

            if (this.ContainsKey(key))
            {
                // Update the existing list of file paths.
                locations = this[key];
            }
            else
            {
                // Create a new list of file paths.
                locations = new List<FilePathLineNum>();
            }

            // Add new path to the current list of file paths
            locations.Add(new FilePathLineNum(location));

            // Place the updated file paths list into the dictionary.
            this[key] = locations;
        }

    }
}
