namespace MyDownloader.App.UI
{
    partial class DownloadList
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.lvwDownloads = new System.Windows.Forms.ListView();
            this.columnFile = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnSize = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnCompleted = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnProgress = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnLeft = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnRate = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnAdded = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnState = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnResume = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnURL = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.popUpContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.newDownloadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.startToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pauseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.removeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator7 = new System.Windows.Forms.ToolStripSeparator();
            this.openFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.copyURLToClipboardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.showInExplorerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.logContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.clearLogToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.popUpContextMenu.SuspendLayout();
            this.logContextMenu.SuspendLayout();
            this.SuspendLayout();
            // 
            // lvwDownloads
            // 
            this.lvwDownloads.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.lvwDownloads.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnFile,
            this.columnSize,
            this.columnCompleted,
            this.columnProgress,
            this.columnLeft,
            this.columnRate,
            this.columnAdded,
            this.columnState,
            this.columnResume,
            this.columnURL});
            this.lvwDownloads.ContextMenuStrip = this.popUpContextMenu;
            this.lvwDownloads.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lvwDownloads.FullRowSelect = true;
            this.lvwDownloads.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.lvwDownloads.HideSelection = false;
            this.lvwDownloads.Location = new System.Drawing.Point(0, 0);
            this.lvwDownloads.Name = "lvwDownloads";
            this.lvwDownloads.ShowGroups = false;
            this.lvwDownloads.ShowItemToolTips = true;
            this.lvwDownloads.Size = new System.Drawing.Size(722, 184);
            this.lvwDownloads.TabIndex = 0;
            this.lvwDownloads.UseCompatibleStateImageBehavior = false;
            this.lvwDownloads.View = System.Windows.Forms.View.Details;
            this.lvwDownloads.ItemSelectionChanged += new System.Windows.Forms.ListViewItemSelectionChangedEventHandler(this.lvwDownloads_ItemSelectionChanged);
            this.lvwDownloads.DoubleClick += new System.EventHandler(this.lvwDownloads_DoubleClick);
            // 
            // columnFile
            // 
            this.columnFile.Text = "File";
            this.columnFile.Width = 98;
            // 
            // columnSize
            // 
            this.columnSize.Text = "Size";
            this.columnSize.Width = 80;
            // 
            // columnCompleted
            // 
            this.columnCompleted.Text = "Completed";
            this.columnCompleted.Width = 81;
            // 
            // columnProgress
            // 
            this.columnProgress.Text = "Progress";
            this.columnProgress.Width = 63;
            // 
            // columnLeft
            // 
            this.columnLeft.Text = "Left";
            this.columnLeft.Width = 70;
            // 
            // columnRate
            // 
            this.columnRate.Text = "Rate";
            this.columnRate.Width = 72;
            // 
            // columnAdded
            // 
            this.columnAdded.Text = "Added";
            this.columnAdded.Width = 77;
            // 
            // columnState
            // 
            this.columnState.Text = "State";
            this.columnState.Width = 97;
            // 
            // columnResume
            // 
            this.columnResume.Text = "Resume";
            // 
            // columnURL
            // 
            this.columnURL.Text = "URL";
            this.columnURL.Width = 171;
            // 
            // popUpContextMenu
            // 
            this.popUpContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newDownloadToolStripMenuItem,
            this.toolStripSeparator5,
            this.startToolStripMenuItem,
            this.pauseToolStripMenuItem,
            this.toolStripSeparator6,
            this.removeToolStripMenuItem,
            this.toolStripSeparator7,
            this.openFileToolStripMenuItem,
            this.copyURLToClipboardToolStripMenuItem,
            this.showInExplorerToolStripMenuItem});
            this.popUpContextMenu.Name = "contextMenuStrip1";
            this.popUpContextMenu.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.popUpContextMenu.Size = new System.Drawing.Size(196, 176);
            this.popUpContextMenu.Opening += new System.ComponentModel.CancelEventHandler(this.popUpContextMenu_Opening);
            // 
            // newDownloadToolStripMenuItem
            // 
            this.newDownloadToolStripMenuItem.Name = "newDownloadToolStripMenuItem";
            this.newDownloadToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.newDownloadToolStripMenuItem.Text = "New Download...";
            this.newDownloadToolStripMenuItem.Click += new System.EventHandler(this.newDownloadToolStripMenuItem_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(192, 6);
            // 
            // startToolStripMenuItem
            // 
            this.startToolStripMenuItem.Name = "startToolStripMenuItem";
            this.startToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.startToolStripMenuItem.Text = "Start";
            this.startToolStripMenuItem.Click += new System.EventHandler(this.startToolStripMenuItem_Click);
            // 
            // pauseToolStripMenuItem
            // 
            this.pauseToolStripMenuItem.Name = "pauseToolStripMenuItem";
            this.pauseToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.pauseToolStripMenuItem.Text = "Pause";
            this.pauseToolStripMenuItem.Click += new System.EventHandler(this.pauseToolStripMenuItem_Click);
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(192, 6);
            // 
            // removeToolStripMenuItem
            // 
            this.removeToolStripMenuItem.Name = "removeToolStripMenuItem";
            this.removeToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.removeToolStripMenuItem.Text = "Remove";
            this.removeToolStripMenuItem.Click += new System.EventHandler(this.removeToolStripMenuItem_Click);
            // 
            // toolStripSeparator7
            // 
            this.toolStripSeparator7.Name = "toolStripSeparator7";
            this.toolStripSeparator7.Size = new System.Drawing.Size(192, 6);
            // 
            // openFileToolStripMenuItem
            // 
            this.openFileToolStripMenuItem.Name = "openFileToolStripMenuItem";
            this.openFileToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.openFileToolStripMenuItem.Text = "Open File";
            this.openFileToolStripMenuItem.Click += new System.EventHandler(this.openFileToolStripMenuItem_Click);
            // 
            // copyURLToClipboardToolStripMenuItem
            // 
            this.copyURLToClipboardToolStripMenuItem.Name = "copyURLToClipboardToolStripMenuItem";
            this.copyURLToClipboardToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.copyURLToClipboardToolStripMenuItem.Text = "Copy URL to Clipboard";
            this.copyURLToClipboardToolStripMenuItem.Click += new System.EventHandler(this.copyURLToClipboardToolStripMenuItem_Click);
            // 
            // showInExplorerToolStripMenuItem
            // 
            this.showInExplorerToolStripMenuItem.Name = "showInExplorerToolStripMenuItem";
            this.showInExplorerToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.showInExplorerToolStripMenuItem.Text = "Show in Explorer";
            this.showInExplorerToolStripMenuItem.Click += new System.EventHandler(this.showInExplorerToolStripMenuItem_Click);
            // 
            // logContextMenu
            // 
            this.logContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.clearLogToolStripMenuItem});
            this.logContextMenu.Name = "logContextMenu";
            this.logContextMenu.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.logContextMenu.Size = new System.Drawing.Size(125, 26);
            // 
            // clearLogToolStripMenuItem
            // 
            this.clearLogToolStripMenuItem.Name = "clearLogToolStripMenuItem";
            this.clearLogToolStripMenuItem.Size = new System.Drawing.Size(124, 22);
            this.clearLogToolStripMenuItem.Text = "Clear Log";
            this.clearLogToolStripMenuItem.Click += new System.EventHandler(this.clearLogToolStripMenuItem_Click);
            // 
            // DownloadList
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.lvwDownloads);
            this.Name = "DownloadList";
            this.Size = new System.Drawing.Size(722, 184);
            this.Load += new System.EventHandler(this.DownloadList_Load);
            this.popUpContextMenu.ResumeLayout(false);
            this.logContextMenu.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView lvwDownloads;
        private System.Windows.Forms.ColumnHeader columnFile;
        private System.Windows.Forms.ColumnHeader columnSize;
        private System.Windows.Forms.ColumnHeader columnCompleted;
        private System.Windows.Forms.ColumnHeader columnProgress;
        private System.Windows.Forms.ColumnHeader columnLeft;
        private System.Windows.Forms.ColumnHeader columnRate;
        private System.Windows.Forms.ColumnHeader columnAdded;
        private System.Windows.Forms.ColumnHeader columnState;
        private System.Windows.Forms.ColumnHeader columnURL;
        private System.Windows.Forms.ContextMenuStrip logContextMenu;
        private System.Windows.Forms.ToolStripMenuItem clearLogToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip popUpContextMenu;
        private System.Windows.Forms.ToolStripMenuItem newDownloadToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripMenuItem startToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem pauseToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
        private System.Windows.Forms.ToolStripMenuItem removeToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator7;
        private System.Windows.Forms.ToolStripMenuItem openFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem copyURLToClipboardToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem showInExplorerToolStripMenuItem;
        private System.Windows.Forms.ColumnHeader columnResume;
    }
}
