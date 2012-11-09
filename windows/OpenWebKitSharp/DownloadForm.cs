// Crated by George Tsoumalis
// Credits to dodgeball for extending it
//
// If you want to use a custom downloader you should set GlobalPreferences.WillHandleDownloadsManually = true
// and use the DownloadBegin event of the WebKitBrowser object to handle downloads
//


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using WebKit.Interop;

namespace WebKit
{
    public partial class DownloadForm : Form
    {
        Int64 Size;
        string File;
        string FileForDownloading;
        WebDownload download; 
        public DownloadForm()
        {
            InitializeComponent();
            if (System.IO.File.Exists(File))
            {
                System.IO.File.Delete(File);
            }
        }
        public void PassArguments(string filename, Int64 filesize, WebDownload d)
        {

            this.Size = filesize;
            this.File = filename;
            this.download = d;
            this.FileForDownloading = filename + ".download";
            label5.Text = "Size: " + Size.ToString();
            label2.Text = Path.GetFileName(File);
            progressBar1.Maximum = Convert.ToInt32(filesize);
            progressBar1.Minimum = 0;
            UpdateProgress.Enabled = true;
        }
        private void progressBar1_Click(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
        int UpdatedSize;
        void UpdateSize()
        {
            if (Size / 1000000000024L >= 1) {
                this.label5.Text = "Size: " + Size / 1000000000024L + " TB";
                this.label6.Text = "Status: " + UpdatedSize / 1000000000024L + " MB of " + Size / 1000000000024L + "MB";
} else if (Size / 1000000024 >= 1) {
    this.label5.Text = "Size: " + Size / 1000000024 + " GB";
    this.label6.Text = "Status: " + UpdatedSize / 1000000024 + " MB of " + Size / 1000000024 + "MB";
} else if (Size / 1000024 >= 1) {
    this.label5.Text = "Size: " + Size / 1000024 + " MB";
    this.label6.Text = "Status: " + UpdatedSize / 1000024 + " MB of " + Size / 1000024 + "MB";
} else if (Size / 1024 >= 1) {
    this.label5.Text = "Size: " + Size / 1024 + " KB";
    this.label6.Text = "Status: " + UpdatedSize / 1024 + " KB of " + Size / 1024 + "KB";
}
        }
        private void UpdateProgress_Tick(object sender, EventArgs e)
        {
            if (System.IO.File.Exists(FileForDownloading))
            {
                if (System.IO.File.Exists(File))
                {
                    System.IO.File.Delete(File);
                }
                label1.Visible = false;
                FileInfo f = new FileInfo(FileForDownloading);
                progressBar1.Value = Convert.ToInt32(f.Length);
                label3.Text = "Remaining: " + (Size - Convert.ToInt32(f.Length)).ToString() + " bytes";
                label4.Text = "Speed: " + (Convert.ToInt32(f.Length) - UpdatedSize).ToString() + " b/s";
                UpdatedSize = Convert.ToInt32(f.Length);
                UpdateSize();
            }
            else
            {
                if (System.IO.File.Exists(File))
                {
                    label1.Visible = false;
                    progressBar1.Value = progressBar1.Maximum;
                    label3.Text = "Completed!";
                    label4.Text = "";
                    button1.Enabled = true;
                    button2.Enabled = false;
                }
            }
        }

        private void DownloadForm_Load(object sender, EventArgs e)
        {
            this.Text = "Kryptonium - Downloader";
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                Process.Start(new ProcessStartInfo(File));
            }
            catch
            {
                MessageBox.Show("The file could not be opened");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void DownloadForm_FormClosed(object sender, FormClosingEventArgs e)
        {

            if (System.IO.File.Exists(FileForDownloading))
            {
                if (MessageBox.Show("Are you sure you want to cancel this download?", "Downloader", MessageBoxButtons.YesNoCancel) == System.Windows.Forms.DialogResult.Yes)
                {
                    download.cancel();
                    e.Cancel = false;
                    this.Close();
                }
                else
                {
                    e.Cancel = true;
                }
            }
               
        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        private void DownloadForm_FormClosed(object sender, FormClosedEventArgs e)
        {

        }
    }
}
