// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System;                 // Used for EventArgs class
using System.ComponentModel;  // Used for various event-related classes
using System.Windows.Forms;   // Used for Form class
using System.IO;              // Used for Directory class
using System.Diagnostics;     // Used for Process class


namespace TraceabilityTool
{
    // This class is responsible for displaying and processing input from the GUI elements of the main form.
    public partial class MainForm : Form
    {

        public static bool outputCSV = false;
        public static bool outputText = true;
        public static bool buildCheck = false;

        public MainForm()
        {
            InitializeComponent();
            chkCSVOutput.Checked = outputCSV;
            txtRootPath.Text = Program.inputDir;
            txtOutputPath.Text = Program.outputDir;
            btnShowOutput.Enabled = Directory.Exists(txtOutputPath.Text);
            btnGenerateReport.Enabled = Directory.Exists(txtRootPath.Text) && Directory.Exists(txtOutputPath.Text);
        }

        private void btnGenerateReport_Click(object sender, EventArgs e)
        {
            if (!Directory.Exists(txtRootPath.Text))
            {
                lblRootPath.Text = "Invalid path";
            }
            else if (!Directory.Exists(txtOutputPath.Text))
            {
                lblOutputPath.Text = "Invalid path";
            }
            else
            {
                lblRootPath.Text = "";
                lblOutputPath.Text = "";
                btnGenerateReport.Enabled = false;
                backgroundWorker.RunWorkerAsync();
            }
        }


        private void btnRootPath_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderDlg = new FolderBrowserDialog();
            if (folderDlg.ShowDialog() == DialogResult.OK)
            {
                txtRootPath.Text = folderDlg.SelectedPath;
                lblRootPath.Text = "";
                btnShowOutput.Enabled = true;
            }
            else
            {
                return;
            }
        }

        private void btnOutputPath_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderDlg = new FolderBrowserDialog();
            if (folderDlg.ShowDialog() == DialogResult.OK)
            {
                txtOutputPath.Text = folderDlg.SelectedPath;
                lblOutputPath.Text = "";
            }
            else
            {
                return;
            }
        }

        private void txtRootPath_TextChanged(object sender, EventArgs e)
        {
            if (!Directory.Exists(txtRootPath.Text))
            {
                lblRootPath.Text = "Invalid path";
            }
            else
            {
                lblRootPath.Text = "";
            }

            btnGenerateReport.Enabled = (chkTextOutput.Checked || chkCSVOutput.Checked) &&
                                        Directory.Exists(txtRootPath.Text) &&
                                        Directory.Exists(txtOutputPath.Text);
        }

        private void txtOutputPath_TextChanged(object sender, EventArgs e)
        {
            if (!Directory.Exists(txtOutputPath.Text))
            {
                lblOutputPath.Text = "Invalid path";
                btnShowOutput.Enabled = false;
            }
            else
            {
                lblOutputPath.Text = "";
                btnShowOutput.Enabled = true;
            }

            btnGenerateReport.Enabled = (chkTextOutput.Checked || chkCSVOutput.Checked) &&
                                        Directory.Exists(txtRootPath.Text) &&
                                        Directory.Exists(txtOutputPath.Text);
        }

        private void btnShowOutput_Click(object sender, EventArgs e)
        {
            Process.Start("explorer", txtOutputPath.Text);
        }

        private void backgroundWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            ReportProgress.Value = e.ProgressPercentage;
        }

        private void backgroundWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            ReportGenerator.GenerateReport(txtRootPath.Text, txtOutputPath.Text, null, this);
        }

        private void backgroundWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            btnGenerateReport.Enabled = true;
        }

        public void UpdateStatus(int progressPercentage)
        {
            backgroundWorker.ReportProgress(progressPercentage);
        }

        private void chkTextOutput_CheckedChanged(object sender, EventArgs e)
        {
            outputText = chkTextOutput.Checked;

            btnGenerateReport.Enabled = (chkTextOutput.Checked || chkCSVOutput.Checked) &&
                                        Directory.Exists(txtRootPath.Text) &&
                                        Directory.Exists(txtOutputPath.Text);
        }

        private void chkCSVOutput_CheckedChanged(object sender, EventArgs e)
        {
            outputCSV = chkCSVOutput.Checked;

            btnGenerateReport.Enabled = (chkTextOutput.Checked || chkCSVOutput.Checked) &&
                                        Directory.Exists(txtRootPath.Text) &&
                                        Directory.Exists(txtOutputPath.Text);
        }

    }
}




