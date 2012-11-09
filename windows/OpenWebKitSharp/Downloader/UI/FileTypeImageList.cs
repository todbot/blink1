using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace MyDownloader.App.UI
{
    public class FileTypeImageList
    {
        private const string OpenFolderKey = "OpenFolderKey";
        private const string CloseFolderKey = "OpenFolderKey";

        private static ImageList instance;

        public static ImageList GetSharedInstance()
        {
            if (instance == null)
            {
                instance = new ImageList();
                instance.TransparentColor = Color.Black;
                instance.TransparentColor = Color.Transparent;
                instance.ColorDepth = ColorDepth.Depth32Bit;
                instance.ImageSize = new Size(16, 16);
            }

            return instance;
        }

        public static int GetImageIndexByExtention(string ext)
        {
            GetSharedInstance();

            ext = ext.ToLower();

            if (!instance.Images.ContainsKey(ext))
            {
                //Icon iconForFile = IconExtractor.Extract(ext);
                Icon iconForFile = IconReader.GetFileIconByExt(ext, IconReader.EnumIconSize.Small, false);

                instance.Images.Add(ext, iconForFile);
            }

            return instance.Images.IndexOfKey(ext); 
        }

        public static int GetImageIndexFromFolder(bool open)
        {
            string key;

            GetSharedInstance();

            if (open)
            {
                key = OpenFolderKey;
            }
            else
            {
                key = CloseFolderKey;
            }

            if (!instance.Images.ContainsKey(key))
            {
                Icon iconForFile = IconReader.GetFolderIcon(
                    IconReader.EnumIconSize.Small,
                    (open ? IconReader.EnumFolderType.Open : IconReader.EnumFolderType.Closed));
                
                instance.Images.Add(key, iconForFile);
            }

            return instance.Images.IndexOfKey(key);
        }
    }
}
