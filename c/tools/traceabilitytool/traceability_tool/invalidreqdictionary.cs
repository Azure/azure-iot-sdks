// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System.Collections.Generic;  // Used for the Lists class

namespace TraceabilityTool
{
    // This class defines a custom data type used for storage of file paths, line numbers and the reasons
    // for requirement IDs being considered invalid for requirement IDs found in source code files.
    class InvalidReqDictEntry
    {
        public string reason;
        public string filePath;
        public int    lineNum;

        public InvalidReqDictEntry(string filePath, int lineNum, string reason)
        {
            this.filePath = filePath;
            this.reason   = reason;
            this.lineNum  = lineNum;
        }
    }


    // This class defines a custom dictionary data type used for storage of a requirement ID and
    // corresponding file path, line number and reason for the entry being added to the dictionary
    // for requirement IDs found in source code files.
    class InvalidReqDictionary : Dictionary<string, List<InvalidReqDictEntry>>
    {
        public int maxKeyLength = 0;
        public int maxReasonLength = 0;

        public void Add(string key, List<FilePathLineNum> locations, string reason)
        {
            foreach (FilePathLineNum loc in locations)
            {
                if (this.ContainsKey(key))
                {
                    List<InvalidReqDictEntry> entry = this[key];
                    InvalidReqDictEntry item = new InvalidReqDictEntry(loc.filePath, loc.lineNum, reason);
                    entry.Add(item);
                    this[key] = entry;
                }
                else
                {
                    List<InvalidReqDictEntry> entry = new List<InvalidReqDictEntry>();
                    InvalidReqDictEntry item = new InvalidReqDictEntry(loc.filePath, loc.lineNum, reason);
                    entry.Add(item);
                    this.Add(key, entry);
                }
            }

            if (key.Length > maxKeyLength)
                maxKeyLength = key.Length;

            if (reason.Length > maxReasonLength)
                maxReasonLength = reason.Length;
        }

    }
}
