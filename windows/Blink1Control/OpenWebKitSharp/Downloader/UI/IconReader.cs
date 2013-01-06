using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace MyDownloader.App.UI
{

    public class IconReader
    {
        ///
        /// used to get/return information about a file
        /// in SHGetFileInfo call
        ///
        [System.FlagsAttribute()]
        private enum EnumFileInfoFlags : uint
        {
            /// get large icon
            LARGEICON = 0x000000000,
            /// get small icon
            SMALLICON = 0x000000001,
            /// get open icon
            OPENICON = 0x000000002,
            /// get shell size icon
            SHELLICONSIZE = 0x000000004,
            /// pszPath is a pidl
            PIDL = 0x000000008,
            /// use passed dwFileAttribute
            USEFILEATTRIBUTES = 0x000000010,
            /// apply the appropriate overlays
            ADDOVERLAYS = 0x000000020,
            /// get the index of the overlay
            OVERLAYINDEX = 0x000000040,
            /// get icon
            ICON = 0x000000100,
            /// get display name
            DISPLAYNAME = 0x000000200,
            /// get type name
            TYPENAME = 0x000000400,
            /// get attributes
            ATTRIBUTES = 0x000000800,
            /// get icon location
            ICONLOCATION = 0x000001000,
            /// return exe type
            EXETYPE = 0x000002000,
            /// get system icon index
            SYSICONINDEX = 0x000004000,
            /// put a link overlay on icon
            LINKOVERLAY = 0x000008000,
            /// show icon in selected state
            SELECTED = 0x000010000,
            /// get only specified attributes
            ATTR_SPECIFIED = 0x000020000
        }

        ///
        /// maxumum length of path
        ///
        private const int conMAX_PATH = 260;

        ///
        /// looking for folder
        ///
        private const uint conFILE_ATTRIBUTE_DIRECTORY = 0x00000010;

        ///
        /// looking for file
        ///
        private const uint conFILE_ATTRIBUTE_NORMAL = 0x00000080;

        ///
        /// size of the icon
        ///
        public enum EnumIconSize
        {
            /// 32x32
            Large = 0,
            /// 16x16
            Small = 1
        }

        ///
        /// state of the folder
        ///
        public enum EnumFolderType
        {
            /// open folder
            Open = 0,
            /// closed folder
            Closed = 1
        }

        ///
        /// hold file/icon information
        /// see platformSDK SHFILEINFO
        ///
        ///
        /// be sure to call DestroyIcon [hIcon] when done
        ///
        [System.Runtime.InteropServices.StructLayout(
           System.Runtime.InteropServices.LayoutKind.Sequential)]
        private struct ShellFileInfo
        {

            public const int conNameSize = 80;
            public System.IntPtr hIcon;  // note to call DestroyIcon
            public int iIndex;
            public uint dwAttributes;

            [System.Runtime.InteropServices.MarshalAs(
               System.Runtime.InteropServices.UnmanagedType.ByValTStr,
               SizeConst = conMAX_PATH)]
            public string szDisplayName;

            [System.Runtime.InteropServices.MarshalAs(
               System.Runtime.InteropServices.UnmanagedType.ByValTStr,
               SizeConst = conNameSize)]
            public string szTypeName;
        };

        ///
        /// used to free a windows icon handle
        ///
        /// "hIcon">icon handle.
        [System.Runtime.InteropServices.DllImport("User32.dll")]
        private static extern int DestroyIcon(System.IntPtr hIcon);

        ///
        /// gets file information
        /// see platformSDK
        ///
        [System.Runtime.InteropServices.DllImport("Shell32.dll")]
        private static extern System.IntPtr SHGetFileInfo(
          string pszPath,
          uint dwFileAttributes,
          ref ShellFileInfo psfi,
          uint cbFileInfo,
          uint uFlags
        );

        public static System.Drawing.Icon GetFileIcon(string filePath,
            EnumIconSize size, bool addLinkOverlay)
        {
            EnumFileInfoFlags flags =
              EnumFileInfoFlags.ICON | EnumFileInfoFlags.USEFILEATTRIBUTES;

            // add link overlay if requested
            if (addLinkOverlay)
            {
                flags |= EnumFileInfoFlags.LINKOVERLAY;
            }

            // set size
            if (size == EnumIconSize.Small)
            {
                flags |= EnumFileInfoFlags.SMALLICON;
            }
            else
            {
                flags |= EnumFileInfoFlags.LARGEICON;
            }

            ShellFileInfo shellFileInfo = new ShellFileInfo();

            SHGetFileInfo(
              filePath,
              conFILE_ATTRIBUTE_NORMAL,
              ref shellFileInfo,
              (uint)System.Runtime.InteropServices.Marshal.SizeOf(shellFileInfo),
              (uint)flags);

            // deep copy
            System.Drawing.Icon icon =
            (System.Drawing.Icon)System.Drawing.Icon.FromHandle(shellFileInfo.hIcon).Clone();
            
            // release handle
            DestroyIcon(shellFileInfo.hIcon);

            return icon;
        }
        ///
        /// lookup and return an icon from windows shell
        ///
        /// "name">path to the file
        /// "size">large or small
        /// "linkOverlay">true to include the overlay link iconlet
        /// requested icon
        public static System.Drawing.Icon GetFileIconByExt(
          string fileExt,
          EnumIconSize size,
          bool addLinkOverlay)
        {
            string tempFile = Path.GetTempPath() + Guid.NewGuid().ToString("N") + fileExt;

            try
            {
                File.WriteAllBytes(tempFile, new byte[1] { 0 });

                return GetFileIcon(tempFile, size, addLinkOverlay);
            }
            finally
            {
                try
                {
                    File.Delete(tempFile);
                }
                catch (Exception)
                {
                }
            }            
        }

        ///
        ///  lookup and return an icon from windows shell
        ///
        /// "size">large or small
        /// "folderType">open or closed
        /// requested icon
        public static System.Drawing.Icon GetFolderIcon(
          EnumIconSize size,
          EnumFolderType folderType)
        {
            return GetFolderIcon(null, size, folderType);
        }

        ///
        /// lookup and return an icon from windows shell
        ///
        /// "folderPath">path to folder    
        /// "size">large or small
        /// "folderType">open or closed
        /// requested icon
        public static System.Drawing.Icon GetFolderIcon(
          string folderPath,
          EnumIconSize size,
          EnumFolderType folderType)
        {

            EnumFileInfoFlags flags =
              EnumFileInfoFlags.ICON | EnumFileInfoFlags.USEFILEATTRIBUTES;

            if (folderType == EnumFolderType.Open)
            {
                flags |= EnumFileInfoFlags.OPENICON;
            }

            if (EnumIconSize.Small == size)
            {
                flags |= EnumFileInfoFlags.SMALLICON;
            }
            else
            {
                flags |= EnumFileInfoFlags.LARGEICON;
            }

            ShellFileInfo shellFileInfo = new ShellFileInfo();
            SHGetFileInfo(
              folderPath,
              conFILE_ATTRIBUTE_DIRECTORY,
              ref shellFileInfo,
              (uint)System.Runtime.InteropServices.Marshal.SizeOf(shellFileInfo),
              (uint)flags);

            // deep copy
            System.Drawing.Icon icon =
              (System.Drawing.Icon)System.Drawing.Icon.FromHandle(shellFileInfo.hIcon).Clone();
            // release handle
            DestroyIcon(shellFileInfo.hIcon);
            return icon;
        }
    }
}
