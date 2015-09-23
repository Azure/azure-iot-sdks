// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace TraceabilityTool
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnRootPath = new System.Windows.Forms.Button();
            this.btnOutputPath = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtRootPath = new System.Windows.Forms.TextBox();
            this.txtOutputPath = new System.Windows.Forms.TextBox();
            this.lblRootPath = new System.Windows.Forms.Label();
            this.lblOutputPath = new System.Windows.Forms.Label();
            this.btnGenerateReport = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.ReportProgress = new System.Windows.Forms.ProgressBar();
            this.btnShowOutput = new System.Windows.Forms.Button();
            this.backgroundWorker = new System.ComponentModel.BackgroundWorker();
            this.chkTextOutput = new System.Windows.Forms.CheckBox();
            this.chkCSVOutput = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // btnRootPath
            // 
            this.btnRootPath.Location = new System.Drawing.Point(287, 25);
            this.btnRootPath.Name = "btnRootPath";
            this.btnRootPath.Size = new System.Drawing.Size(75, 23);
            this.btnRootPath.TabIndex = 0;
            this.btnRootPath.Text = "Browse...";
            this.btnRootPath.UseVisualStyleBackColor = true;
            this.btnRootPath.Click += new System.EventHandler(this.btnRootPath_Click);
            // 
            // btnOutputPath
            // 
            this.btnOutputPath.Location = new System.Drawing.Point(287, 72);
            this.btnOutputPath.Name = "btnOutputPath";
            this.btnOutputPath.Size = new System.Drawing.Size(75, 23);
            this.btnOutputPath.TabIndex = 0;
            this.btnOutputPath.Text = "Browse...";
            this.btnOutputPath.UseVisualStyleBackColor = true;
            this.btnOutputPath.Click += new System.EventHandler(this.btnOutputPath_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(87, 30);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(57, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Root path:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(78, 81);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(66, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Output path:";
            // 
            // txtRootPath
            // 
            this.txtRootPath.Location = new System.Drawing.Point(148, 27);
            this.txtRootPath.Name = "txtRootPath";
            this.txtRootPath.Size = new System.Drawing.Size(133, 20);
            this.txtRootPath.TabIndex = 3;
            this.txtRootPath.TextChanged += new System.EventHandler(this.txtRootPath_TextChanged);
            // 
            // txtOutputPath
            // 
            this.txtOutputPath.Location = new System.Drawing.Point(148, 74);
            this.txtOutputPath.Name = "txtOutputPath";
            this.txtOutputPath.Size = new System.Drawing.Size(133, 20);
            this.txtOutputPath.TabIndex = 3;
            this.txtOutputPath.TextChanged += new System.EventHandler(this.txtOutputPath_TextChanged);
            // 
            // lblRootPath
            // 
            this.lblRootPath.AutoSize = true;
            this.lblRootPath.ForeColor = System.Drawing.Color.Red;
            this.lblRootPath.Location = new System.Drawing.Point(151, 50);
            this.lblRootPath.Name = "lblRootPath";
            this.lblRootPath.Size = new System.Drawing.Size(62, 13);
            this.lblRootPath.TabIndex = 4;
            this.lblRootPath.Text = "Invalid path";
            // 
            // lblOutputPath
            // 
            this.lblOutputPath.AutoSize = true;
            this.lblOutputPath.ForeColor = System.Drawing.Color.Red;
            this.lblOutputPath.Location = new System.Drawing.Point(151, 97);
            this.lblOutputPath.Name = "lblOutputPath";
            this.lblOutputPath.Size = new System.Drawing.Size(62, 13);
            this.lblOutputPath.TabIndex = 4;
            this.lblOutputPath.Text = "Invalid path";
            // 
            // btnGenerateReport
            // 
            this.btnGenerateReport.Location = new System.Drawing.Point(165, 196);
            this.btnGenerateReport.Name = "btnGenerateReport";
            this.btnGenerateReport.Size = new System.Drawing.Size(133, 43);
            this.btnGenerateReport.TabIndex = 5;
            this.btnGenerateReport.Text = "Generate reports";
            this.btnGenerateReport.UseVisualStyleBackColor = true;
            this.btnGenerateReport.Click += new System.EventHandler(this.btnGenerateReport_Click);
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(0, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(100, 23);
            this.label3.TabIndex = 0;
            // 
            // ReportProgress
            // 
            this.ReportProgress.Location = new System.Drawing.Point(31, 258);
            this.ReportProgress.Name = "ReportProgress";
            this.ReportProgress.Size = new System.Drawing.Size(410, 23);
            this.ReportProgress.TabIndex = 7;
            // 
            // btnShowOutput
            // 
            this.btnShowOutput.Location = new System.Drawing.Point(165, 302);
            this.btnShowOutput.Name = "btnShowOutput";
            this.btnShowOutput.Size = new System.Drawing.Size(133, 43);
            this.btnShowOutput.TabIndex = 5;
            this.btnShowOutput.Text = "Show output";
            this.btnShowOutput.UseVisualStyleBackColor = true;
            this.btnShowOutput.Click += new System.EventHandler(this.btnShowOutput_Click);
            // 
            // backgroundWorker
            // 
            this.backgroundWorker.WorkerReportsProgress = true;
            this.backgroundWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker_DoWork);
            this.backgroundWorker.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.backgroundWorker_ProgressChanged);
            this.backgroundWorker.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundWorker_RunWorkerCompleted);
            // 
            // chkTextOutput
            // 
            this.chkTextOutput.AutoSize = true;
            this.chkTextOutput.Checked = true;
            this.chkTextOutput.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkTextOutput.Location = new System.Drawing.Point(191, 132);
            this.chkTextOutput.Name = "chkTextOutput";
            this.chkTextOutput.Size = new System.Drawing.Size(80, 17);
            this.chkTextOutput.TabIndex = 8;
            this.chkTextOutput.Text = "Text output";
            this.chkTextOutput.UseVisualStyleBackColor = true;
            this.chkTextOutput.CheckedChanged += new System.EventHandler(this.chkTextOutput_CheckedChanged);
            // 
            // chkCSVOutput
            // 
            this.chkCSVOutput.AutoSize = true;
            this.chkCSVOutput.Cursor = System.Windows.Forms.Cursors.Default;
            this.chkCSVOutput.Location = new System.Drawing.Point(191, 155);
            this.chkCSVOutput.Name = "chkCSVOutput";
            this.chkCSVOutput.Size = new System.Drawing.Size(80, 17);
            this.chkCSVOutput.TabIndex = 8;
            this.chkCSVOutput.Text = "CSV output";
            this.chkCSVOutput.UseVisualStyleBackColor = true;
            this.chkCSVOutput.CheckedChanged += new System.EventHandler(this.chkCSVOutput_CheckedChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(463, 367);
            this.Controls.Add(this.chkCSVOutput);
            this.Controls.Add(this.chkTextOutput);
            this.Controls.Add(this.ReportProgress);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.btnShowOutput);
            this.Controls.Add(this.btnGenerateReport);
            this.Controls.Add(this.lblOutputPath);
            this.Controls.Add(this.lblRootPath);
            this.Controls.Add(this.txtOutputPath);
            this.Controls.Add(this.txtRootPath);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnOutputPath);
            this.Controls.Add(this.btnRootPath);
            this.Name = "MainForm";
            this.Text = "Traceability Tool";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnRootPath;
        private System.Windows.Forms.Button btnOutputPath;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtRootPath;
        private System.Windows.Forms.TextBox txtOutputPath;
        private System.Windows.Forms.Label lblRootPath;
        private System.Windows.Forms.Label lblOutputPath;
        private System.Windows.Forms.Button btnGenerateReport;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ProgressBar ReportProgress;
        private System.Windows.Forms.Button btnShowOutput;
        private System.ComponentModel.BackgroundWorker backgroundWorker;
        private System.Windows.Forms.CheckBox chkTextOutput;
        private System.Windows.Forms.CheckBox chkCSVOutput;
    }
}

