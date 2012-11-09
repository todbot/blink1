/*
 * Copyright (c) 2009, Peter Nelson (charn.opcode@gmail.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 *   
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
*/

using System;
using System.Runtime.InteropServices;

namespace WebKit
{
    /// <summary>
    /// Win32 API functions.
    /// </summary>
    /// <remarks>
    /// See the MSDN Windows SDK documentation for more information about 
    /// these functions.
    /// </remarks>
    internal static class NativeMethods
    {
        public static int INVALID_HANDLE_VALUE = (-1);

        [DllImport("Kernel32.dll", SetLastError = true)]
        public static extern bool ActivateActCtx(IntPtr hActCtx, out uint lpCookie);

        [DllImport("Kernel32.dll", SetLastError = true)]
        public static extern IntPtr CreateActCtx(ref ACTCTX actctx);

        [DllImport("Kernel32.dll", SetLastError = true)]
        public static extern bool DeactivateActCtx(uint dwFlags, uint lpCookie);

        [DllImport("Kernel32.dll", SetLastError = true)]
        public static extern void ReleaseActCtx(IntPtr hActCtx);

        [DllImport("user32.dll")]
        public static extern bool MoveWindow(IntPtr hwnd, int x, int y, int width, int height, 
            [MarshalAs(UnmanagedType.Bool)] bool repaint);

        [DllImport("user32.dll")]
        public static extern IntPtr SetFocus(IntPtr hwnd);

        [DllImport("user32.dll")]
        public static extern IntPtr SendMessage(IntPtr hwnd, uint msg, IntPtr wParam, IntPtr lParam);

        [StructLayout(LayoutKind.Sequential)]
        public struct ACTCTX
        {
            public int cbSize;
            public uint dwFlags;
            public string lpSource;
            public ushort wProcessorArchitecture;
            public ushort wLangId;
            public string lpAssemblyDirectory;
            public string lpResourceName;
            public string lpApplicationName;
            public IntPtr hModule;
        }
    }
}
