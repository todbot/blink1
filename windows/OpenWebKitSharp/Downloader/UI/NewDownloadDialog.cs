using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using MyDownloader.Core.Common;
using MyDownloader.Core;
using MyDownloader.Core.UI;
using MyDownloader.Extension.Protocols;
using ICSharpCode.SharpZipLib.Zip;
using MyDownloader.Extension.Zip;
using System.IO;
using System.Threading;
using WebKit;

namespace MyDownloader.App.UI
{
    public partial class NewDownloadDialog : Form
    {
        public NewDownloadDialog()
        {
            InitializeComponent();
            locationMain.UrlChanged += new EventHandler(locationMain_UrlChanged);
        }

        void locationMain_UrlChanged(object sender, EventArgs e)
        {
            try
            {
                Uri u = new Uri(locationMain.ResourceLocation.URL);
                string fn = u.Segments[u.Segments.Length - 1];
                if (!fn.EndsWith("/"))
                {
                    txtFilename.Text = u.Segments[u.Segments.Length - 1];
                }
                else
                    txtFilename.Text = "Unrecognized Download.file";
            }
            catch
            {
                txtFilename.Text = string.Empty;
            }
        }

        public ResourceLocation DownloadLocation
        {
            get
            {
                return locationMain.ResourceLocation;
            }
            set
            {
                locationMain.ResourceLocation = value;
            }
        }

        public ResourceLocation[] Mirrors
        {
            get
            {
                MyDownloader.Core.ResourceLocation[] mirrors = new MyDownloader.Core.ResourceLocation[lvwLocations.Items.Count];

                for (int i = 0; i < lvwLocations.Items.Count; i++)
                {
                    ListViewItem item = lvwLocations.Items[i];
                    mirrors[i] = MyDownloader.Core.ResourceLocation.FromURL(
                            item.SubItems[0].Text, 
                            BoolFormatter.FromString(item.SubItems[1].Text), 
                            item.SubItems[2].Text, 
                            item.SubItems[3].Text);
                }

                return mirrors;
            }
        }


        public int Segments
        {
            get
            {
                return 1;
            }
        }

        public bool StartNow
        {
            get
            {
                return true;
            }
        }

        private void lvwLocations_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            bool hasSelected = lvwLocations.SelectedItems.Count > 0;
            btnRemove.Enabled = hasSelected;
            if (hasSelected)
            {
                ListViewItem item = lvwLocations.SelectedItems[0];
                locationAlternate.ResourceLocation = MyDownloader.Core.ResourceLocation.FromURL(
                    item.SubItems[0].Text, BoolFormatter.FromString(item.SubItems[1].Text), item.SubItems[2].Text, item.SubItems[3].Text);
            }
            else
            {
                locationAlternate.ResourceLocation = null;
            }
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            for (int i = lvwLocations.Items.Count - 1; i >= 0; i--)
            {
                if (lvwLocations.Items[i].Selected)
                {
                    lvwLocations.Items.RemoveAt(i);
                }
            }
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            ResourceLocation rl = locationAlternate.ResourceLocation;

            if (lvwLocations.SelectedItems.Count > 0)
            {
                ListViewItem item = lvwLocations.SelectedItems[0];
                item.SubItems[0].Text = rl.URL;
                item.SubItems[1].Text = BoolFormatter.ToString(rl.Authenticate);
                item.SubItems[2].Text = rl.Login;
                item.SubItems[3].Text = rl.Password;
            }
            else
            {
                ListViewItem item = new ListViewItem();
                item.Text = rl.URL;
                item.SubItems.Add(BoolFormatter.ToString(rl.Authenticate));
                item.SubItems.Add(rl.Login);
                item.SubItems.Add(rl.Password);
                lvwLocations.Items.Add(item);
            }
        }
        public string LocalFile;
        string folderpath = GlobalPreferences.DownloadsFolder;
        private void btnOK_Click(object sender, EventArgs e)
        {
            string SaveFilePath = string.Empty;
            folderpath = GlobalPreferences.DownloadsFolder;
            if (folderpath != string.Empty)
            {
                if (folderpath.EndsWith("\\"))
                    SaveFilePath = folderpath + txtFilename.Text;
                else
                    SaveFilePath = folderpath + "\\" + txtFilename.Text;
            }
            else
            {
                System.Windows.Forms.FolderBrowserDialog f = new FolderBrowserDialog();
                f.Description = "Please select the folder where you want the file to be saved.";
                if (f.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    SaveFilePath = f.SelectedPath + "\\" + txtFilename.Text;
                }
                else
                {
                    this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
                    return;
                }
            }
            this.LocalFile = SaveFilePath;
            try
            {
                ResourceLocation rl = this.DownloadLocation;

                rl.BindProtocolProviderType();

                if (rl.ProtocolProviderType == null)
                {
                    MessageBox.Show("Invalid URL format, please check the location field.",
                        AppManager.Instance.Application.MainForm.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);

                    DialogResult = DialogResult.None;
                    return;
                }

                ResourceLocation[] mirrors = this.Mirrors;

                if (mirrors != null && mirrors.Length > 0)
                {
                    foreach (ResourceLocation mirrorRl in mirrors)
                    {
                        mirrorRl.BindProtocolProviderType();

                        if (mirrorRl.ProtocolProviderType == null)
                        {
                            MessageBox.Show("Invalid mirror URL format, please check the mirror URLs.",
                                AppManager.Instance.Application.MainForm.Text,
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Error);

                            DialogResult = DialogResult.None;
                            return;
                        }
                    }
                }
                Downloader download = DownloadManager.Instance.Add(
                    rl,
                    mirrors,
                    this.LocalFile,
                    this.Segments,
                    this.StartNow);

                Close();
            }
            catch (Exception)
            {
                DialogResult = DialogResult.None;

                MessageBox.Show("Unknow error, please check your input data.",
                    AppManager.Instance.Application.MainForm.Text,
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void chkChooseZIP_CheckedChanged(object sender, EventArgs e)
        {

        }


        private void NewDownloadForm_FormClosing(object sender, FormClosingEventArgs e)
        {
        }

        private void NewDownloadForm_Load(object sender, EventArgs e)
        {
        }

        private void folderBrowser1_Load(object sender, EventArgs e)
        {
           
        }

        private void NewDownloadForm_Load_1(object sender, EventArgs e)
        {

        }
    }
}