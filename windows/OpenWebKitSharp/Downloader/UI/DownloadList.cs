using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using MyDownloader.Core;
using System.Collections;
using System.Diagnostics;
using System.IO;
using MyDownloader.Core.Common;
using MyDownloader.Extension.Video;
using System.Web;
using MyDownloader.Extension.AutoDownloads;
using MyDownloader.App.Controls;
using MyDownloader.Core.UI;
using MyDownloader.Extension.WindowsIntegration.ClipboardMonitor;

namespace MyDownloader.App.UI
{
    public partial class DownloadList : UserControl, IClipboardDataHandler
    {
        delegate void ActionDownloader(Downloader d, ListViewItem item);

        Hashtable mapItemToDownload = new Hashtable();
        Hashtable mapDownloadToItem = new Hashtable();

        ListViewItem lastSelection = null;
        AutoDownloadsExtension scheduler;
        ClipboardMonitor clipMonitor;

        public DownloadList()
        {
            InitializeComponent();

            DownloadManager.Instance.DownloadAdded += new EventHandler<DownloaderEventArgs>(Instance_DownloadAdded);
            DownloadManager.Instance.DownloadRemoved += new EventHandler<DownloaderEventArgs>(Instance_DownloadRemoved);
            DownloadManager.Instance.DownloadEnded += new EventHandler<DownloaderEventArgs>(Instance_DownloadEnded);

            for (int i = 0; i < DownloadManager.Instance.Downloads.Count; i++)
            {
                AddDownload(DownloadManager.Instance.Downloads[i]);
            }

            lvwDownloads.SmallImageList = FileTypeImageList.GetSharedInstance();
        }

        private void DownloadList_Load(object sender, EventArgs e)
        {
            DownloadManager.Instance.BeginAddBatchDownloads += new EventHandler(Instance_BeginAddBatchDownloads);
            DownloadManager.Instance.EndAddBatchDownloads += new EventHandler(Instance_EndAddBatchDownloads);

            scheduler = (AutoDownloadsExtension)App.Instance.GetExtensionByType(typeof(AutoDownloadsExtension));
            clipMonitor = new ClipboardMonitor(this, this.Handle);

            this.Disposed += delegate(object snd, EventArgs ea)
            {
                clipMonitor.Dispose();
            };
        }

        void Instance_EndAddBatchDownloads(object sender, EventArgs e)
        {
            this.BeginInvoke((MethodInvoker)lvwDownloads.EndUpdate);
        }

        void Instance_BeginAddBatchDownloads(object sender, EventArgs e)
        {
            this.BeginInvoke((MethodInvoker)lvwDownloads.BeginUpdate);
        }

        string lastClipboardURL;

        #region IClipboardDataHandler Members

        protected override void WndProc(ref Message m)
        {
            if (clipMonitor == null || !clipMonitor.FilterMessage(ref m))
            {
                base.WndProc(ref m);
            }
        }

        public void HandleClipboardData(IDataObject data)
        {
            NewDownloadFromClipboardData();
        }

        #endregion

        public void NewDownloadFromData(string url, string filename)
        {
            VideoDownloadExtension ext = (VideoDownloadExtension)App.Instance.GetExtensionByType(typeof(VideoDownloadExtension));

            if (ext.GetHandlerByURL(url) != null)
            {
                ((VideoDownloadUIExtension)ext.UIExtension).ShowNewVideoDialog(url, false);
            }
            else
            {
                NewFileDownload(url, filename, false);
            }
        }

        public void NewDownloadFromClipboardData()
        {
            string url = ClipboardHelper.GetURLOnClipboard();

            if (string.IsNullOrEmpty(url) || lastClipboardURL == url)
            {
                return;
            }

            lastClipboardURL = url;

            VideoDownloadExtension ext = (VideoDownloadExtension)App.Instance.GetExtensionByType(typeof(VideoDownloadExtension));

            if (ext.GetHandlerByURL(url) != null)
            {
                ((VideoDownloadUIExtension)ext.UIExtension).ShowNewVideoDialog(null, false);
            }
            else
            {
                NewFileDownload(null, null, false);
            }
        }

        public void NewVideoDownload()
        {
            VideoDownloadExtension ext = (VideoDownloadExtension)App.Instance.GetExtensionByType(typeof(VideoDownloadExtension));
            ((VideoDownloadUIExtension)ext.UIExtension).ShowNewVideoDialog(null, true);
        }

        public void NewFileDownload(string url, string fn, bool modal, bool dialog = true)
        {
            if (modal)
            {
                if (dialog == false)
                {
                    using (NewDownloadForm newDownload = new NewDownloadForm())
                    {
                        if (!String.IsNullOrEmpty(url))
                        {
                            newDownload.DownloadLocation = ResourceLocation.FromURL(url);
                            newDownload.LocalFile = fn;
                        }

                        newDownload.Show();
                    }
                }
                else
                {
                    using (NewDownloadDialog newDownload = new NewDownloadDialog())
                    {
                        if (!String.IsNullOrEmpty(url))
                        {
                            newDownload.DownloadLocation = ResourceLocation.FromURL(url);
                            newDownload.LocalFile = fn;
                        }

                        newDownload.ShowDialog();
                    }
                }
            }
            else
            {
                NewDownloadForm newDownload = new NewDownloadForm();

                if (!String.IsNullOrEmpty(url))
                {
                    newDownload.DownloadLocation = ResourceLocation.FromURL(url);
                    newDownload.LocalFile = fn;
                }

                newDownload.ShowInTaskbar = true;
                newDownload.MinimizeBox = true;
                newDownload.Show();
                newDownload.Focus();
                newDownload.TopMost = true;
            }
        }

        public void StartScheduler(bool enabled)
        {
            scheduler.Active = enabled;
        }

        public bool SchedulerStarted()
        {
            return scheduler.Active;
        }

        public void StartSelections()
        {
            DownloadsAction(
                delegate(Downloader d, ListViewItem item)
                {
                    d.Start();
                }
            );
        }

        public void Pause()
        {
            DownloadsAction(
                delegate(Downloader d, ListViewItem item)
                {
                    d.Pause();
                }
            );
        }

        public void PauseAll()
        {
            DownloadManager.Instance.PauseAll();
            UpdateList();
        }

        public void RemoveSelections()
        {
            if (MessageBox.Show("Are you sure that you want to remove selected downloads?",
                this.ParentForm.Text, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                try
                {
                    lvwDownloads.ItemSelectionChanged -= new ListViewItemSelectionChangedEventHandler(lvwDownloads_ItemSelectionChanged);
                    DownloadManager.Instance.DownloadRemoved -= new EventHandler<DownloaderEventArgs>(Instance_DownloadRemoved);

                    DownloadsAction(
                        delegate(Downloader d, ListViewItem item)
                        {
                            lvwDownloads.Items.RemoveAt(item.Index);
                            DownloadManager.Instance.RemoveDownload(d);
                        }
                    );
                }
                finally
                {
                    lvwDownloads.ItemSelectionChanged += new ListViewItemSelectionChangedEventHandler(lvwDownloads_ItemSelectionChanged);
                    lvwDownloads_ItemSelectionChanged(null, null);

                    DownloadManager.Instance.DownloadRemoved += new EventHandler<DownloaderEventArgs>(Instance_DownloadRemoved);
                }
            }
        }

        public void SelectAll()
        {
            using (DownloadManager.Instance.LockDownloadList(false))
            {
                lvwDownloads.BeginUpdate();
                try
                {
                    lvwDownloads.ItemSelectionChanged -= new ListViewItemSelectionChangedEventHandler(lvwDownloads_ItemSelectionChanged);
                    
                    for (int i = 0; i < lvwDownloads.Items.Count; i++)
                    {
                        lvwDownloads.Items[i].Selected = true;
                    }
                }
                finally
                {
                    lvwDownloads.ItemSelectionChanged += new ListViewItemSelectionChangedEventHandler(lvwDownloads_ItemSelectionChanged);
                    lvwDownloads_ItemSelectionChanged(null, null);

                    lvwDownloads.EndUpdate();
                }
            }
        }

        public void RemoveCompleted()
        {
            lvwDownloads.BeginUpdate();
            try
            {
                DownloadManager.Instance.ClearEnded();
                UpdateList();
            }
            finally
            {
                lvwDownloads.EndUpdate();
            }
        }

        public void AddDownloadURLs(
            ResourceLocation[] args,
            int segments,
            string path,
            int nrOfSubfolders)
        {
            if (args == null) return;

            if (path == null)
            {
                path = PathHelper.GetWithBackslash(Core.Settings.Default.DownloadFolder);
            }
            else
            {
                path = PathHelper.GetWithBackslash(path);
            }

            try
            {
                DownloadManager.Instance.OnBeginAddBatchDownloads();

                foreach (ResourceLocation rl in args)
                {
                    Uri uri = new Uri(rl.URL);

                    string fileName = uri.Segments[uri.Segments.Length - 1];
                    fileName = HttpUtility.UrlDecode(fileName).Replace("/", "\\");
                    DownloadManager.Instance.Add(
                        rl,
                        null,
                        path + fileName,
                        segments,
                        false);
                }
            }
            finally
            {
                DownloadManager.Instance.OnEndAddBatchDownloads();
            }
        }

        private void lvwDownloads_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            UpdateSegments();

            UpdateUI();
        }

        public void LoadSettingsView()
        {
            lvwDownloads.GridLines = false;
        }

        public void UpdateUI()
        {
            bool isSelected = lvwDownloads.SelectedItems.Count > 0;

            removeToolStripMenuItem.Enabled = isSelected;
            startToolStripMenuItem.Enabled = isSelected;
            pauseToolStripMenuItem.Enabled = isSelected;

            isSelected = lvwDownloads.SelectedItems.Count == 1;
            copyURLToClipboardToolStripMenuItem.Enabled = isSelected;

            showInExplorerToolStripMenuItem.Enabled = isSelected;
            openFileToolStripMenuItem.Enabled = isSelected && SelectedDownloaders[0].State == DownloaderState.Ended;

            OnSelectionChange();
        }

        public event EventHandler SelectionChange;

        protected virtual void OnSelectionChange()
        {
            if (SelectionChange != null)
            {
                SelectionChange(this, EventArgs.Empty);
            }
        }

        private void copyURLToClipboardToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(SelectedDownloaders[0].ResourceLocation.URL);
        }

        private void showInExplorerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process.Start("explorer.exe", String.Format("/select,{0}", SelectedDownloaders[0].LocalFile));
        }

        private void openFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                Process.Start(SelectedDownloaders[0].LocalFile);
            }
            catch (Exception)
            {
            }
        }

        public int SelectedCount
        {
            get
            {
                return lvwDownloads.SelectedItems.Count;
            }
        }

        public Downloader[] SelectedDownloaders
        {
            get
            {
                if (lvwDownloads.SelectedItems.Count > 0)
                {
                    Downloader[] downloaders = new Downloader[lvwDownloads.SelectedItems.Count];
                    for (int i = 0; i < downloaders.Length; i++)
                    {
                        downloaders[i] = mapItemToDownload[lvwDownloads.SelectedItems[i]] as Downloader;
                    }
                    return downloaders;
                }

                return null;
            }
        }

        void Instance_DownloadRemoved(object sender, DownloaderEventArgs e)
        {
            //try
            //{
            //    this.BeginInvoke((MethodInvoker)delegate()
            //    {
            //        ListViewItem item = mapDownloadToItem[e.Downloader] as ListViewItem;

            //        if (item != null)
            //        {
            //            if (item.Selected)
            //            {
            //                lastSelection = null;
            //                //lvwDownloads.SelectedItems.Clear();
            //            }

            //            mapDownloadToItem[e.Downloader] = null;
            //            mapItemToDownload[item] = null;

            //            item.Remove();
            //        }
            //    }
            //);
            //}
            //catch { }
        }

        void Instance_DownloadAdded(object sender, DownloaderEventArgs e)
        {
            if (IsHandleCreated)
            {
                this.BeginInvoke((MethodInvoker)delegate() { AddDownload(e.Downloader); });
            }
            else
            {
                AddDownload(e.Downloader);
            }
        }

        private void AddDownload(Downloader d)
        {
            d.RestartingSegment += new EventHandler<SegmentEventArgs>(download_RestartingSegment);
            d.SegmentStoped += new EventHandler<SegmentEventArgs>(download_SegmentEnded);
            d.SegmentFailed += new EventHandler<SegmentEventArgs>(download_SegmentFailed);
            d.SegmentStarted += new EventHandler<SegmentEventArgs>(download_SegmentStarted);
            d.InfoReceived += new EventHandler(download_InfoReceived);
            d.SegmentStarting += new EventHandler<SegmentEventArgs>(Downloader_SegmentStarting);

            string ext = Path.GetExtension(d.LocalFile);

            ListViewItem item = new ListViewItem();

            item.ImageIndex = FileTypeImageList.GetImageIndexByExtention(ext);
            item.Text = Path.GetFileName(d.LocalFile);

            // size
            item.SubItems.Add(ByteFormatter.ToString(d.FileSize));
            // completed
            item.SubItems.Add(ByteFormatter.ToString(d.Transfered));
            // progress
            item.SubItems.Add(String.Format("{0:0.##}%", d.Progress));
            // left
            item.SubItems.Add(TimeSpanFormatter.ToString(d.Left));
            // rate
            item.SubItems.Add("0");
            // added
            item.SubItems.Add(d.CreatedDateTime.ToShortDateString() + " " + d.CreatedDateTime.ToShortTimeString());
            // state
            item.SubItems.Add(d.State.ToString());
            // resume
            item.SubItems.Add(GetResumeStr(d));
            // url
            item.SubItems.Add(d.ResourceLocation.URL);

            mapDownloadToItem[d] = item;
            mapItemToDownload[item] = d;

            lvwDownloads.Items.Add(item);
        }

        private static string GetResumeStr(Downloader d)
        {
            return (d.RemoteFileInfo != null && d.RemoteFileInfo.AcceptRanges ? "Yes" : "No");
        }

        public void UpdateList()
        {
            for (int i = 0; i < lvwDownloads.Items.Count; i++)
            {
                ListViewItem item = lvwDownloads.Items[i];
                if (item == null) return;
                Downloader d = mapItemToDownload[item] as Downloader;
                if (d == null) return;

                DownloaderState state;

                if (item.Tag == null) state = DownloaderState.Working;
                else state = (DownloaderState)item.Tag;

                if (state != d.State ||
                    state == DownloaderState.Working ||
                    state == DownloaderState.WaitingForReconnect)
                {
                    item.SubItems[1].Text = ByteFormatter.ToString(d.FileSize);
                    item.SubItems[2].Text = ByteFormatter.ToString(d.Transfered);
                    item.SubItems[3].Text = String.Format("{0:0.##}%", d.Progress);
                    item.SubItems[4].Text = TimeSpanFormatter.ToString(d.Left);
                    item.SubItems[5].Text = String.Format("{0:0.##}", d.Rate / 1024.0);

                    if (d.LastError != null)
                    {
                        item.SubItems[7].Text = d.State.ToString() + ", " + d.LastError.Message;
                    }
                    else
                    {
                        if (String.IsNullOrEmpty(d.StatusMessage))
                        {
                            item.SubItems[7].Text = d.State.ToString();
                        }
                        else
                        {
                            item.SubItems[7].Text = d.State.ToString() + ", " + d.StatusMessage;
                        }
                    }

                    item.SubItems[8].Text = GetResumeStr(d);
                    item.SubItems[9].Text = d.ResourceLocation.URL;
                    item.Tag = d.State;
                }
            }

            UpdateSegments();
        }

        private void UpdateSegments()
        {
            try
            {

                if (lvwDownloads.SelectedItems.Count == 1)
                {
                    ListViewItem newSelection = lvwDownloads.SelectedItems[0];
                    Downloader d = mapItemToDownload[newSelection] as Downloader;

                    if (lastSelection == newSelection)
                    {
                    }
                    else
                    {
                        UpdateSegmentsInserting(d, newSelection);
                    }
                }
                else
                {
                    lastSelection = null;
                }
            }
            finally
            {
            }
        }        

        private void DownloadsAction(ActionDownloader action)
        {
            if (lvwDownloads.SelectedItems.Count > 0)
            {
                try
                {
                    lvwDownloads.BeginUpdate();

                    lvwDownloads.ItemSelectionChanged -= new ListViewItemSelectionChangedEventHandler(lvwDownloads_ItemSelectionChanged);

                    for (int i = lvwDownloads.SelectedItems.Count - 1; i >= 0; i--)
                    {
                        ListViewItem item = lvwDownloads.SelectedItems[i];
                        action((Downloader)mapItemToDownload[item], item);
                    }

                    lvwDownloads.ItemSelectionChanged += new ListViewItemSelectionChangedEventHandler(lvwDownloads_ItemSelectionChanged);
                    lvwDownloads_ItemSelectionChanged(null, null);
                }
                finally
                {
                    lvwDownloads.EndUpdate();
                    UpdateSegments();
                }
            }            
        }

        private void UpdateSegmentsWithoutInsert(Downloader d)
        {
            for (int i = 0; i < d.Segments.Count; i++)
            {
            }

        }

        private void UpdateSegmentsInserting(Downloader d, ListViewItem newSelection)
        {
            lastSelection = newSelection;


            List<Block> blocks = new List<Block>();

            for (int i = 0; i < d.Segments.Count; i++)
            {
                ListViewItem item = new ListViewItem();

                item.Text = d.Segments[i].CurrentTry.ToString();
                item.SubItems.Add(String.Format("{0:0.##}%", d.Segments[i].Progress));
                item.SubItems.Add(ByteFormatter.ToString(d.Segments[i].Transfered));
                item.SubItems.Add(ByteFormatter.ToString(d.Segments[i].TotalToTransfer));
                item.SubItems.Add(ByteFormatter.ToString(d.Segments[i].InitialStartPosition));
                item.SubItems.Add(ByteFormatter.ToString(d.Segments[i].EndPosition));
                item.SubItems.Add(String.Format("{0:0.##}", d.Segments[i].Rate / 1024.0));
                item.SubItems.Add(TimeSpanFormatter.ToString(d.Segments[i].Left));
                item.SubItems.Add(d.Segments[i].State.ToString());
                item.SubItems.Add(d.Segments[i].CurrentURL);

                blocks.Add(new Block(d.Segments[i].TotalToTransfer, (float)d.Segments[i].Progress));
            }

        }

        void download_InfoReceived(object sender, EventArgs e)
        {
            Downloader d = (Downloader)sender;
        }

        void Downloader_SegmentStarting(object sender, SegmentEventArgs e)
        {
        }

        void download_SegmentStarted(object sender, SegmentEventArgs e)
        {
        }

        void download_SegmentFailed(object sender, SegmentEventArgs e)
        {
        }

        void download_SegmentEnded(object sender, SegmentEventArgs e)
        {
        }

        void download_RestartingSegment(object sender, SegmentEventArgs e)
        {
        }

        void Instance_DownloadEnded(object sender, DownloaderEventArgs e)
        {
        }

        enum LogMode
        {
            Error,
            Information
        }

        private void clearLogToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void newDownloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NewFileDownload(null, null, true);
        }

        private void startToolStripMenuItem_Click(object sender, EventArgs e)
        {
            StartSelections();
        }

        private void pauseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Pause();
        }

        private void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RemoveSelections();
        }

        private void SwapElements(int idx)
        {
            ListViewItem it1 = lvwDownloads.Items[idx];
            ListViewItem it2 = lvwDownloads.Items[idx - 1];

            lvwDownloads.Items.RemoveAt(idx);
            lvwDownloads.Items.RemoveAt(idx - 1);

            lvwDownloads.Items.Insert(idx - 1, it1);
            lvwDownloads.Items.Insert(idx, it2);

            DownloadManager.Instance.SwapDownloads(idx, true);
        }

        public void MoveSelectionsDown()
        {
            int selCount = lvwDownloads.SelectedItems.Count;
            if (selCount > 0)
            {
                try
                {
                    lvwDownloads.BeginUpdate();

                    ListViewItem lvi = null;

                    using (DownloadManager.Instance.LockDownloadList(true))
                    {
                        for (int i = selCount - 1; i >= 0; i--)
                        {
                            lvi = lvwDownloads.SelectedItems[i];

                            if (lvi.Index < lvwDownloads.Items.Count - 1
                                && !lvwDownloads.Items[lvi.Index + 1].Selected)
                            {
                                SwapElements(lvi.Index + 1);
                            }
                        }
                    }

                    lvwDownloads.SelectedItems[selCount - 1].EnsureVisible();
                }
                finally
                {
                    lvwDownloads.EndUpdate();
                }
            }
        }

        public void MoveSelectionsUp()
        {
            int selCount = lvwDownloads.SelectedItems.Count;

            if (selCount > 0)
            {
                try
                {
                    lvwDownloads.BeginUpdate();

                    ListViewItem lvi = null;

                    using (DownloadManager.Instance.LockDownloadList(true))
                    {
                        for (int i = 0; i < selCount; i++)
                        {
                            lvi = lvwDownloads.SelectedItems[i];

                            if (lvi.Index > 0
                                && !lvwDownloads.Items[lvi.Index - 1].Selected)
                            {
                                SwapElements(lvi.Index);
                            }
                        }
                    }

                    lvwDownloads.SelectedItems[0].EnsureVisible();
                }
                finally
                {
                    lvwDownloads.EndUpdate();
                }
            }
        }

        public bool ClipboardMonitorEnabled
        {
            set
            {
                if (clipMonitor != null)
                {
                    clipMonitor.Enabled = value;
                }
            }
            get
            {
                if (clipMonitor != null)
                {
                    return clipMonitor.Enabled;
                }
                return false;
            }
        }

        private void popUpContextMenu_Opening(object sender, CancelEventArgs e)
        {
            UpdateUI();
        }

        private void lvwDownloads_DoubleClick(object sender, EventArgs e)
        {
            UpdateUI();

            openFileToolStripMenuItem_Click(sender, e);
        }
    }
}
