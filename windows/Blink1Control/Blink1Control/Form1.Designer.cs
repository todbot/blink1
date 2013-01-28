namespace Blink1Control
{
    partial class Form1
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
            if (disposing && (components != null)) {
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.stripMenuBlink1Status = new System.Windows.Forms.ToolStripMenuItem();
            this.stripMenuBlink1Id = new System.Windows.Forms.ToolStripMenuItem();
            this.stripMenuKey = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.stripMenuResetAlerts = new System.Windows.Forms.ToolStripMenuItem();
            this.stripMenuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.stripMenuStartMinimized = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.ContextMenuStrip = this.contextMenuStrip1;
            this.notifyIcon1.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon1.Icon")));
            this.notifyIcon1.Text = "Blink1Control";
            this.notifyIcon1.Visible = true;
            this.notifyIcon1.Click += new System.EventHandler(this.notifyIcon1_Click);
            this.notifyIcon1.DoubleClick += new System.EventHandler(this.notifyIcon1_DoubleClick);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.stripMenuBlink1Status,
            this.stripMenuBlink1Id,
            this.stripMenuKey,
            this.toolStripSeparator2,
            this.stripMenuStartMinimized,
            this.toolStripSeparator1,
            this.stripMenuResetAlerts,
            this.stripMenuExit});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(158, 170);
            // 
            // stripMenuBlink1Status
            // 
            this.stripMenuBlink1Status.Name = "stripMenuBlink1Status";
            this.stripMenuBlink1Status.Size = new System.Drawing.Size(157, 22);
            this.stripMenuBlink1Status.Text = "blink1Status";
            // 
            // stripMenuBlink1Id
            // 
            this.stripMenuBlink1Id.Name = "stripMenuBlink1Id";
            this.stripMenuBlink1Id.Size = new System.Drawing.Size(157, 22);
            this.stripMenuBlink1Id.Text = "serial";
            // 
            // stripMenuKey
            // 
            this.stripMenuKey.Name = "stripMenuKey";
            this.stripMenuKey.Size = new System.Drawing.Size(157, 22);
            this.stripMenuKey.Text = "key";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(154, 6);
            // 
            // stripMenuResetAlerts
            // 
            this.stripMenuResetAlerts.Name = "stripMenuResetAlerts";
            this.stripMenuResetAlerts.Size = new System.Drawing.Size(157, 22);
            this.stripMenuResetAlerts.Text = "Reset Alerts";
            this.stripMenuResetAlerts.Click += new System.EventHandler(this.stripMenuResetAlerts_Click);
            // 
            // stripMenuExit
            // 
            this.stripMenuExit.Name = "stripMenuExit";
            this.stripMenuExit.Size = new System.Drawing.Size(157, 22);
            this.stripMenuExit.Text = "Exit";
            this.stripMenuExit.Click += new System.EventHandler(this.stripMenuExit_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(154, 6);
            // 
            // stripMenuStartMinimized
            // 
            this.stripMenuStartMinimized.CheckOnClick = true;
            this.stripMenuStartMinimized.Name = "stripMenuStartMinimized";
            this.stripMenuStartMinimized.Size = new System.Drawing.Size(157, 22);
            this.stripMenuStartMinimized.Text = "Start Minimized";
            this.stripMenuStartMinimized.Click += new System.EventHandler(this.stripMenuStartMinimized_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(839, 617);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(855, 655);
            this.MinimumSize = new System.Drawing.Size(855, 655);
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Blink1Control";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Resize += new System.EventHandler(this.Form1_Resize);
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NotifyIcon notifyIcon1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem stripMenuBlink1Status;
        private System.Windows.Forms.ToolStripMenuItem stripMenuBlink1Id;
        private System.Windows.Forms.ToolStripMenuItem stripMenuExit;
        private System.Windows.Forms.ToolStripMenuItem stripMenuResetAlerts;
        private System.Windows.Forms.ToolStripMenuItem stripMenuKey;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem stripMenuStartMinimized;
    }
}

