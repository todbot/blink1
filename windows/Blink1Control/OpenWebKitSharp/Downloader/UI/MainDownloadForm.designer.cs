using MyDownloader.Spider.UI;
namespace MyDownloader.App.UI
{
    partial class MainDownloadForm
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainDownloadForm));
            this.tmrRefresh = new System.Windows.Forms.Timer(this.components);
            this.toolStripMain = new System.Windows.Forms.ToolStrip();
            this.toolNewDownload = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStart = new System.Windows.Forms.ToolStripButton();
            this.toolPause = new System.Windows.Forms.ToolStripButton();
            this.toolPauseAll = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolRemove = new System.Windows.Forms.ToolStripButton();
            this.toolRemoveCompleted = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.toolMoveSelectionsUp = new System.Windows.Forms.ToolStripButton();
            this.toolMoveSelectionsDown = new System.Windows.Forms.ToolStripButton();
            this.downloadList1 = new MyDownloader.App.UI.DownloadList();
            this.panel1 = new System.Windows.Forms.Panel();
            this.toolStripMain.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tmrRefresh
            // 
            this.tmrRefresh.Enabled = true;
            this.tmrRefresh.Interval = 1000;
            this.tmrRefresh.Tick += new System.EventHandler(this.tmrRefresh_Tick);
            // 
            // toolStripMain
            // 
            this.toolStripMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.toolStripMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolNewDownload,
            this.toolStripSeparator1,
            this.toolStart,
            this.toolPause,
            this.toolPauseAll,
            this.toolStripSeparator2,
            this.toolRemove,
            this.toolRemoveCompleted,
            this.toolStripSeparator3,
            this.toolMoveSelectionsUp,
            this.toolMoveSelectionsDown});
            this.toolStripMain.Location = new System.Drawing.Point(0, 0);
            this.toolStripMain.Name = "toolStripMain";
            this.toolStripMain.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.toolStripMain.Size = new System.Drawing.Size(541, 25);
            this.toolStripMain.TabIndex = 1;
            // 
            // toolNewDownload
            // 
            this.toolNewDownload.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolNewDownload.Image = ((System.Drawing.Image)(resources.GetObject("toolNewDownload.Image")));
            this.toolNewDownload.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolNewDownload.Name = "toolNewDownload";
            this.toolNewDownload.Size = new System.Drawing.Size(23, 22);
            this.toolNewDownload.Text = "New Download";
            this.toolNewDownload.Click += new System.EventHandler(this.toolNewDownload_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStart
            // 
            this.toolStart.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStart.Image = ((System.Drawing.Image)(resources.GetObject("toolStart.Image")));
            this.toolStart.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStart.Name = "toolStart";
            this.toolStart.Size = new System.Drawing.Size(23, 22);
            this.toolStart.Text = "Start";
            this.toolStart.Click += new System.EventHandler(this.toolStart_Click);
            // 
            // toolPause
            // 
            this.toolPause.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolPause.Image = ((System.Drawing.Image)(resources.GetObject("toolPause.Image")));
            this.toolPause.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolPause.Name = "toolPause";
            this.toolPause.Size = new System.Drawing.Size(23, 22);
            this.toolPause.Text = "Pause";
            this.toolPause.Click += new System.EventHandler(this.toolPause_Click);
            // 
            // toolPauseAll
            // 
            this.toolPauseAll.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolPauseAll.Image = ((System.Drawing.Image)(resources.GetObject("toolPauseAll.Image")));
            this.toolPauseAll.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolPauseAll.Name = "toolPauseAll";
            this.toolPauseAll.Size = new System.Drawing.Size(23, 22);
            this.toolPauseAll.Text = "Pause All";
            this.toolPauseAll.Click += new System.EventHandler(this.toolPauseAll_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // toolRemove
            // 
            this.toolRemove.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolRemove.Image = ((System.Drawing.Image)(resources.GetObject("toolRemove.Image")));
            this.toolRemove.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolRemove.Name = "toolRemove";
            this.toolRemove.Size = new System.Drawing.Size(23, 22);
            this.toolRemove.Text = "Remove";
            this.toolRemove.Click += new System.EventHandler(this.toolRemove_Click);
            // 
            // toolRemoveCompleted
            // 
            this.toolRemoveCompleted.Image = ((System.Drawing.Image)(resources.GetObject("toolRemoveCompleted.Image")));
            this.toolRemoveCompleted.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolRemoveCompleted.Name = "toolRemoveCompleted";
            this.toolRemoveCompleted.Size = new System.Drawing.Size(132, 22);
            this.toolRemoveCompleted.Text = "Remove Completed";
            this.toolRemoveCompleted.Click += new System.EventHandler(this.toolRemoveCompleted_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
            // 
            // toolMoveSelectionsUp
            // 
            this.toolMoveSelectionsUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolMoveSelectionsUp.Image = ((System.Drawing.Image)(resources.GetObject("toolMoveSelectionsUp.Image")));
            this.toolMoveSelectionsUp.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolMoveSelectionsUp.Name = "toolMoveSelectionsUp";
            this.toolMoveSelectionsUp.Size = new System.Drawing.Size(23, 22);
            this.toolMoveSelectionsUp.Text = "Move Selections Up";
            this.toolMoveSelectionsUp.Click += new System.EventHandler(this.toolStripButton2_Click);
            // 
            // toolMoveSelectionsDown
            // 
            this.toolMoveSelectionsDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolMoveSelectionsDown.Image = ((System.Drawing.Image)(resources.GetObject("toolMoveSelectionsDown.Image")));
            this.toolMoveSelectionsDown.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolMoveSelectionsDown.Name = "toolMoveSelectionsDown";
            this.toolMoveSelectionsDown.Size = new System.Drawing.Size(23, 22);
            this.toolMoveSelectionsDown.Text = "Move Selections Down";
            this.toolMoveSelectionsDown.Click += new System.EventHandler(this.toolStripButton3_Click);
            // 
            // downloadList1
            // 
            this.downloadList1.ClipboardMonitorEnabled = false;
            this.downloadList1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.downloadList1.Location = new System.Drawing.Point(0, 0);
            this.downloadList1.Name = "downloadList1";
            this.downloadList1.Size = new System.Drawing.Size(541, 375);
            this.downloadList1.TabIndex = 0;
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.Color.White;
            this.panel1.Controls.Add(this.toolStripMain);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 375);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(541, 25);
            this.panel1.TabIndex = 2;
            // 
            // MainDownloadForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(541, 400);
            this.Controls.Add(this.downloadList1);
            this.Controls.Add(this.panel1);
            this.Name = "MainDownloadForm";
            this.Text = "Downloader";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.toolStripMain.ResumeLayout(false);
            this.toolStripMain.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Timer tmrRefresh;
        private System.Windows.Forms.ToolStrip toolStripMain;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton toolStart;
        private System.Windows.Forms.ToolStripButton toolPause;
        private System.Windows.Forms.ToolStripButton toolPauseAll;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton toolRemove;
        private System.Windows.Forms.ToolStripButton toolRemoveCompleted;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton toolNewDownload;
        private System.Windows.Forms.ToolStripButton toolMoveSelectionsUp;
        private System.Windows.Forms.ToolStripButton toolMoveSelectionsDown;
        protected internal DownloadList downloadList1;
        private System.Windows.Forms.Panel panel1;


    }
}