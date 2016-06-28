// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System.Collections.Generic;             // Used for List class.
using System.Linq;                            // Used for Cast<OpenXmlElement>
using System.Text.RegularExpressions;         // Used for Regex class.
using DocumentFormat.OpenXml.Wordprocessing;  // Used for Body class.
using DocumentFormat.OpenXml.Packaging;       // Used for WordprocessingDocument class.
using DocumentFormat.OpenXml;                 // Used for OpenXmlElement class
using System.Windows.Forms;                   // Used for MessageBox class
using System;                                 // Used for Exception and Console classes

namespace TraceabilityTool
{
    // This class is responsible for extracting requirement IDs from requirement documents in Word OpenXML format (.docm files only),
    // extracting requirement IDs from source code files (.c, .cpp, .h) and generating data structures making it easy to create
    // requirement traceability reports and reports showing errors and mismatches between requirements and implementation/tests.
    static class ReportGenerator
    {
        // Lists of files found in the root directory
        public static List<string> requirementDocuments = new List<string>();
        public static List<string> sourceCodeFiles = new List<string>();

        // A list of requirement IDs and corresponding document file paths
        public static Dictionary<string, string> reqDocLookup = new Dictionary<string, string>();
        // A count of requirements per document.
        public static Dictionary<string, int> reqDocCount = new Dictionary<string, int>();
        // A list of requirement IDs and corresponding source code file paths
        public static ReqPathMatrix reqCodeLookup = new ReqPathMatrix();
        // A list of requirement IDs found in requirements documents and corresponding source code file paths (only contains traceable requirements).
        public static ReqPathMatrix reqCodeMatrix = new ReqPathMatrix();
        // A list of requirement IDs found in requirements documents and corresponding test code file paths (only contains traceable requirements).
        public static ReqPathMatrix reqTestMatrix = new ReqPathMatrix();

        // Requirements that were found in the source code files, but were not found in the requirement documents.
        public static InvalidReqDictionary invalidRequirements = new InvalidReqDictionary();
        // Requirements that were found in documents, but couldn't be found in the code.
        public static Dictionary<string, string> missingCodeCoverage = new Dictionary<string, string>();
        public static int missingCodeCoverageKeyWidth = 0;
        // Requirements that were found in documents, but couldn't be found in the tests.
        public static Dictionary<string, string> missingTestCoverage = new Dictionary<string, string>();
        public static int missingTestCoverageKeyWidth = 0;
        // Duplicate requirement IDs found in requirements documents.
        public static Dictionary<string, List<string>> repeatingRequirements = new Dictionary<string, List<string>>();
        public static int repeatingRequirementsKeyWidth = 0;

        public static bool useGUI = true;


        public static int GenerateReport(string rootFolderPath, string outputFolderPath, string[] exclusionDirs, MainForm mainForm)
        {
            // Clear the lists/dictionaries of files and requirement IDs in case we need to generate the reports again.
            requirementDocuments.Clear();
            reqCodeLookup.Clear();
            reqDocCount.Clear();
            sourceCodeFiles.Clear();
            repeatingRequirements.Clear();
            invalidRequirements.Clear();
            reqDocLookup.Clear();
            reqCodeMatrix.Clear();
            reqTestMatrix.Clear();
            missingCodeCoverage.Clear();
            missingTestCoverage.Clear();

            int result = 0;

            // disable GUI elements if the program was called from command line interface.
            if (mainForm == null)
                useGUI = false;

            // output dir is an empty string if not valid.
            bool useOutputDir = (outputFolderPath.Length != 0);
        
            // Read requirement identifiers and file paths from word documents and source code files.
            GetRequirementsFromDocuments(rootFolderPath, exclusionDirs);

            // Update status on the progress bar
            if (mainForm != null)
            {
                mainForm.UpdateStatus(25);
            }

            GetRequirementsFromSource(rootFolderPath, exclusionDirs);

            // Update status on the progress bar
            if (mainForm != null)
            {
                mainForm.UpdateStatus(50);
            }

            GenerateTraceabilityMatrix();

            // Update status on the progress bar
            if (mainForm != null)
            {
                mainForm.UpdateStatus(75);
            }

            FindUncoveredRequirements();

            // Write all reports to plain text files
            if (MainForm.outputText && useOutputDir)
            {
                ReportWriter.WriteTraceabilityReport(outputFolderPath);
                ReportWriter.WriteInvalidReqReport(outputFolderPath);
                ReportWriter.WriteMissingCodeCoverageReport(outputFolderPath);
                ReportWriter.WriteMissingTestCoverageReport(outputFolderPath);
                ReportWriter.WriteMissingCodeAndTestCoverageReport(outputFolderPath);
                ReportWriter.WriteRepeatingReqReport(outputFolderPath);
            }

            // Write all reports to CSV files
            if (MainForm.outputCSV && useOutputDir)
            {
                CSVReportWriter.WriteTraceabilityReport(outputFolderPath);
                CSVReportWriter.WriteMissingReqReport(outputFolderPath);
                CSVReportWriter.WriteRepeatingReqReport(outputFolderPath);
            }

            if (MainForm.buildCheck)
            {
                result = ConsoleReportWriter.WriteMissingReqReport();
            }

            // Update status on the progress bar
            if (mainForm != null)
            {
                mainForm.UpdateStatus(100);
            }

            return result;
        }


        private static void GetRequirementsFromDocuments(string rootFolderPath, string[] exclusionDirs)
        {
            // Generate a list of Word document files under the root folder.
            List<string> reqDocFileFilter = new List<string>();
            reqDocFileFilter.Add("*.docm");
            reqDocFileFilter.Add("*.md");
            FileFinder.SetFileFilters(reqDocFileFilter);
            FileFinder.GetFileList(rootFolderPath, exclusionDirs, ref requirementDocuments);

            // Read requirement identifiers from the Word documents.
            foreach (string requirementDoc in requirementDocuments)
            {
                try
                {
                    if (requirementDoc.EndsWith(".docm" ))
                    {
                        ReadWordDocRequirements(requirementDoc, ref reqDocLookup);
                    }
                    else if (requirementDoc.EndsWith(".md"))
                    {
                        ReadMarkdownRequirements(requirementDoc, ref reqDocLookup);
                    }
                }
                catch (Exception exception)
                {
                    string message = "An error occurred while attempting to access the file " + requirementDoc + System.Environment.NewLine +
                                     "The error is:  " + exception.Message + System.Environment.NewLine;
                    if (ReportGenerator.useGUI)
                        MessageBox.Show(message, "Error");
                    else
                        Console.WriteLine(message);
                    Program.exitCode = 1;
                }
            }
        }


        private static void GetRequirementsFromSource(string rootFolderPath, string[] exclusionDirs)
        {
            // Generate a list of source code files under the root folder.
            List<string> sourceCodeFilterList = new List<string>();
            sourceCodeFilterList.Add("*.c");
            sourceCodeFilterList.Add("*.cpp");
            sourceCodeFilterList.Add("*.h");
            sourceCodeFilterList.Add("*.cs");
            sourceCodeFilterList.Add("*.java");

            FileFinder.SetFileFilters(sourceCodeFilterList);
            FileFinder.GetFileList(rootFolderPath, exclusionDirs, ref sourceCodeFiles);

            // Read requirement identifiers from the source code files.
            foreach (string sourceFile in sourceCodeFiles)
            {
                try
                {
                    ReadSourceCodeRequirements(sourceFile, ref reqCodeLookup);
                }
                catch (Exception exception)
                {
                    string message = "An error occurred while attempting to access the file " + sourceFile + System.Environment.NewLine +
                                     "The error is:  " + exception.Message + System.Environment.NewLine;
                    if (ReportGenerator.useGUI)
                        MessageBox.Show(message, "Error");
                    else
                        Console.WriteLine(message);
                    Program.exitCode = 1;
                }
            }
        }


        private static void GenerateTraceabilityMatrix()
        {
            // Generate traceability matrix by checking if every requirement found in the source code
            // can be found in requirements documents.
            int srsStartPos;
            string reqID;
            string reqPrefix;
            // Loop through each requirement reference (Codes_SRS_* or Tests_SRS_*) found in the code
            foreach (string codeReq in reqCodeLookup.Keys)
            {
                // Find the prefix of the requirement reference in the code
                srsStartPos = codeReq.IndexOf("SRS");
                reqPrefix = codeReq.Substring(0, srsStartPos);
                // Find the requirement identifier (starting with "SRS_") in the key.
                reqID = codeReq.Substring(srsStartPos);
                // Check if the requirement reference prefix is correct and the requirement is found in requirement documents.
                if (reqPrefix.Equals("Codes_") &&
                    reqDocLookup.ContainsKey(reqID))
                {
                    // Update requirement-to-code matrix (req. ID and source code file paths).
                    reqCodeMatrix.Add(reqID, reqCodeLookup[codeReq]);
                }
                else if (reqPrefix.Equals("Tests_") &&
                         reqDocLookup.ContainsKey(reqID))
                {
                    // Update requirement-to-test matrix (req. ID and test code file paths).
                    reqTestMatrix.Add(reqID, reqCodeLookup[codeReq]);
                }
                else if (reqPrefix.Equals("Codes_") ||
                         reqPrefix.Equals("Tests_"))
                {
                    // Add requirement to the list of invalid requirements - requirement has no defition in .docm files.
                    invalidRequirements.Add(reqID, reqCodeLookup[codeReq], "Requirement definition not found");
                }
                else
                {
                    // Add requirement to the list of invalid requirements - requirement reference is missing "Codes_" or "Tests_" prefix.
                    invalidRequirements.Add(reqID, reqCodeLookup[codeReq], "Requirement prefix not valid");
                }
            }
        }


        public static void ReadWordDocRequirements(string filePath, ref Dictionary<string, string> reqLookup)
        {
            WordprocessingDocument wordprocessingDocument = WordprocessingDocument.Open(filePath, false);
            Body body = wordprocessingDocument.MainDocumentPart.Document.Body;

            // Accept deletions in tracked changes (revisions)
            List<OpenXmlElement> deletions = body.Descendants<Deleted>().Cast<OpenXmlElement>().ToList();
            deletions.AddRange(body.Descendants<DeletedRun>().Cast<OpenXmlElement>().ToList());
            deletions.AddRange(body.Descendants<DeletedMathControl>().Cast<OpenXmlElement>().ToList());
            foreach (OpenXmlElement deletion in deletions)
            {
                deletion.Remove();
            }

            string text = body.InnerText;

            wordprocessingDocument.Close();
            string pattern = @"SRS_[A-Z_]+_\d{2}_\d{3}";

            ExtractRequirements(filePath, pattern, text, ref reqLookup);
        }

        public static void ReadMarkdownRequirements(string filePath, ref Dictionary<string, string> reqLookup)
        {

            System.IO.StreamReader fileAsStream = new System.IO.StreamReader(filePath);
            string fileAsString = fileAsStream.ReadToEnd();
            fileAsStream.Close();
            string pattern = @"SRS_[A-Z_]+_\d{2}_\d{3}";

            ExtractRequirements(filePath, pattern, fileAsString, ref reqLookup);
        }

        public static void ExtractRequirements(string filePath, string pattern, string text, ref Dictionary<string, string> reqLookup)
        {
            int count = 0;
            foreach (Match m in Regex.Matches(text, pattern))
            {
                count++;
                // Add each requirement from Word documents to the lookup dictionary
                // unless this requirement already exists.
                if (reqLookup.ContainsKey(m.Value))
                {
                    // Requirement already exists.  Put it in the dictionary of repeating requirements.
                    List<string> filePaths;

                    if (repeatingRequirements.ContainsKey(m.Value))
                    {
                        // Update the existing list of file paths.
                        filePaths = repeatingRequirements[m.Value];
                    }
                    else
                    {
                        // Create a new list of file paths.
                        filePaths = new List<string>();

                        // Add the file path from the dictionary first (original document path), then add the current path.
                        filePaths.Add(reqLookup[m.Value]);
                    }
                    // Add the new path for the repeating requirement.
                    filePaths.Add(filePath);
                    // Add all paths to the dictionary.
                    repeatingRequirements.Add(m.Value, filePaths);

                    if (m.Value.Length > repeatingRequirementsKeyWidth)
                        repeatingRequirementsKeyWidth = m.Value.Length;
                }
                else
                {
                    // Create a new entry with requirement ID as the key and the path as the value to the dictionary.
                    reqLookup.Add(m.Value, filePath);
                }
            }
            if (count > 0)
            {
                reqDocCount.Add(filePath, count);
            }

        }

        public static void ReadSourceCodeRequirements(string filePath, ref ReqPathMatrix codeReqLookup)
        {
            // Read the file as one string.
            System.IO.StreamReader fileAsStream = new System.IO.StreamReader(filePath);
            string fileAsString = fileAsStream.ReadToEnd();
            fileAsStream.Close();

            // Look for requirement references in the format something_SRS_something_123 or SRS_something_123
            // or _something_SRS_something_123 and _SRS_something_123
            string pattern = @"\b[A-Za-z_]*_?SRS_\w+_\d{2}_\d{3}";
            int lineNum = 1;  // Start counting lines from 1.
            int pos = 0;      // First character index for a regular expression match is 0.
            foreach (Match m in Regex.Matches(fileAsString, pattern))
            {
                while (pos < fileAsString.Length && pos < m.Index)
                {
                    if (fileAsString[pos] == '\n')
                        lineNum++;
                    pos++;
                }
                codeReqLookup.Add(m.Value, new FilePathLineNum(filePath, lineNum));
            }
        }


        private static void FindUncoveredRequirements()
        {
            // Find all requirements from Word documents that don't exist in either code or test traceability matrix.
            // Loop through each requirement from Word documents and check it against the requirements found in the source code.
            foreach (string req in reqDocLookup.Keys)
            {
                if (!reqCodeMatrix.ContainsKey(req))
                {
                    missingCodeCoverage.Add(req, reqDocLookup[req]);

                    if (req.Length > missingCodeCoverageKeyWidth)
                        missingCodeCoverageKeyWidth = req.Length;
                }

                if (!reqTestMatrix.ContainsKey(req))
                {
                    missingTestCoverage.Add(req, reqDocLookup[req]);

                    if (req.Length > missingTestCoverageKeyWidth)
                        missingTestCoverageKeyWidth = req.Length;
                }
            }
        }
        
    }
}
