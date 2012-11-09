using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using System.Web;
using System.Diagnostics;

using MyDownloader.Core;
using MyDownloader.Core.Common;
using MyDownloader.App.SingleInstancing;
using MyDownloader.Extension.AutoDownloads;
using MyDownloader.Core.Extensions;
using MyDownloader.Extension.Video;
using MyDownloader.Extension.WindowsIntegration.ClipboardMonitor;

namespace MyDownloader.App.UI
{
    public partial class MainDownloadForm : Form, ISingleInstanceEnforcer
    {

        public MainDownloadForm()
        {
            InitializeComponent();

            downloadList1.SelectionChange += new EventHandler(downloadList1_SelectionChange);
            downloadList1.UpdateUI();           
        }

        void downloadList1_SelectionChange(object sender, EventArgs e)
        {
            int cnt = downloadList1.SelectedCount;

            bool isSelected = cnt > 0;
            bool isSelectedOnlyOne = cnt == 1;

            toolStart.Enabled = isSelected;
            toolPause.Enabled = isSelected;
            toolRemove.Enabled = isSelected;

            toolMoveSelectionsDown.Enabled = isSelected;
            toolMoveSelectionsUp.Enabled = isSelected;
        }        

        private void tmrRefresh_Tick(object sender, EventArgs e)
        {
            string strRate;
                strRate = String.Format("{0:0.##} kbps", DownloadManager.Instance.TotalDownloadRate / 1024.0);
            downloadList1.UpdateList();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            LoadViewSettings();
        }

        private void toolNewDownload_Click(object sender, EventArgs e)
        {
            downloadList1.NewFileDownload(null, null, true);
        }

        private void toolStart_Click(object sender, EventArgs e)
        {
            downloadList1.StartSelections();
        }

        private void toolPause_Click(object sender, EventArgs e)
        {
            downloadList1.Pause();
        }

        private void toolPauseAll_Click(object sender, EventArgs e)
        {
            downloadList1.PauseAll();
        }

        private void toolRemove_Click(object sender, EventArgs e)
        {
            downloadList1.RemoveSelections();
        }

        private void toolRemoveCompleted_Click(object sender, EventArgs e)
        {
            downloadList1.RemoveCompleted();
        }

        private void toolAbout_Click(object sender, EventArgs e)
        {
        }

        #region ISingleInstanceEnforcer Members

        public void OnMessageReceived(MessageEventArgs e)
        {
            string[] args = (string[])e.Message;

            if (args.Length == 2 && args[0] == "/sw")
            {
                this.BeginInvoke((MethodInvoker) delegate { downloadList1.NewDownloadFromData(args[1], null); });
            }
            else
            {
                downloadList1.AddDownloadURLs(ResourceLocation.FromURLArray(args), 1, null, 0);
            }
        }

        public void OnNewInstanceCreated(EventArgs e)
        {
            this.Focus();
        }

        #endregion

        private void LoadViewSettings()
        {
            downloadList1.LoadSettingsView();

            toolStripMain.Visible = true;
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void viewMenuClickClick(object sender, EventArgs e)
        {
            ToolStripMenuItem menu = ((ToolStripMenuItem)sender);
            menu.Checked = !menu.Checked;

            LoadViewSettings();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Settings.Default.Save();
        }

        private void showHideToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ShowHideForm();
        }

        public void ShowHideForm()
        {
            if (this.Visible)
            {
                HideForm();
            }
            else
            {
                ShowForm();
                LoadViewSettings();
            }
        }

        public void ShowForm()
        {
            this.ShowInTaskbar = true;
            this.Visible = true;
            this.WindowState = FormWindowState.Normal;
        }

        public void HideForm()
        {
            this.ShowInTaskbar = false;
            this.Visible = false;
        }

        private void showHideToolStripMenuItem_Click(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                ShowHideForm();
            }
        }

        private void newVideoDownloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            downloadList1.NewVideoDownload();
        }

        private void toolStripScheduler_Click(object sender, EventArgs e)
        {
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            downloadList1.MoveSelectionsUp();
        }

        private void toolStripButton3_Click(object sender, EventArgs e)
        {
            downloadList1.MoveSelectionsDown();
        }

        private void selectAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            downloadList1.SelectAll();
        }
    }
}