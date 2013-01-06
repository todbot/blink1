using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace WebKit
{
    /// <summary>
    /// Provides strings that will be used by OpenWebkitSharp.
    /// </summary>
    public static class LanguageLoader
    {
        /// <summary>
        /// Returns the specified string.
        /// </summary>
        /// <param name="str">The string to return.</param>
        /// <returns></returns>
        public static string GetStr(string str)
        {
            try
            {
                return File.ReadAllText(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\OpenWebkitSharp Strings\\" + str + ".ini", Encoding.Default);
            }
            catch
            {
                Initialize();
                return File.ReadAllText(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\OpenWebkitSharp Strings\\" + str + ".ini", Encoding.Default);
            }
        }
        /// <summary>
        /// Sets the specified string.
        /// </summary>
        /// <param name="str">The string to set.</param>
        /// <param name="value">The new value of the string.</param>
        public static void SetStr(string str, string value)
        {
            File.WriteAllText(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\OpenWebkitSharp Strings\\" + str + ".ini", value, Encoding.UTF8);
        }

        /// <summary>
        /// Returns true if the LanguageLoader has been initialized. Otherwise false.
        /// </summary>
        public static bool HasBeenInitialized()
        {
            return Directory.Exists(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\OpenWebKitSharp Strings\\") && Directory.GetFiles(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\OpenWebkitSharp Strings\\").Length > 0;
        }

        /// <summary>
        /// Sets the default values of all strings.
        /// </summary>
        public static void Initialize()
        {
            if (HasBeenInitialized() == false)
            {
                System.IO.Directory.CreateDirectory(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\OpenWebKitSharp Strings\\");
                SetLanguageToEnglish(); // Set the strings to the default English strings.
            }
        }

        /// <summary>
        /// Sets the language values from a specified INI file.
        /// </summary>
        public static void SetLanguageFromINIFile(string path)
        {
            string[] filedata = File.ReadAllLines(path, Encoding.Default);
            try
            {
                string ACI = filedata[0];
                string ACA = filedata[1];
                string ACD = filedata[2];
                string back = filedata[3];
                string forward = filedata[4];
                string reload = filedata[5];
                string stop = filedata[6];
                string views = filedata[7];
                string searchwg = filedata[8];
                string copy = filedata[9];
                string htmld = filedata[10];
                string allfiles = filedata[11];
                string renderingpage = filedata[12];
                string connecting = filedata[13];
                string completed = filedata[14];
                string print = filedata[15];
                string saveas = filedata[16];
                string inspector;
                try
                {
                    inspector = filedata[17];
                }
                catch { inspector = "Inspect Element"; }
                ActivationContextInitError = ACI;
                ActivationContextDeactivError = ACD;
                ActivationContextActivError = ACA;
                NavBack = back;
                NavForward = forward;
                Reload = reload;
                Stop = stop;
                ViewSource = views;
                SearchWithGoogle = searchwg;
                Copy = copy;
                HTMLDoc = htmld;
                AllFiles = allfiles;
                RenderingPage = renderingpage;
                ConnectingWith = connecting;
                Completed = completed;
                Print = print;
                SaveAs = saveas;
                ShowInspector = inspector;
            }
            catch(Exception e)
            {
                throw new Exception("The file you selected is not valid. (" + e.Message + ")");
            }
        }

        /// <summary>
        /// Sets the language to English by loading the English.ini file located in the "LanguageLoader.resources" folder.
        /// </summary>
        public static void SetLanguageToEnglish()
        {
            SetLanguageFromINIFile(System.Windows.Forms.Application.StartupPath + @"\LanguageLoader.resources\English.ini");
        }

        /// <summary>
        /// Sets the language to German by loading the German.ini file located in the "LanguageLoader.resources" folder.
        /// </summary>
        public static void SetLanguageToGerman()
        {
            SetLanguageFromINIFile(System.Windows.Forms.Application.StartupPath + @"\LanguageLoader.resources\German.ini");
        }

        /// <summary>
        /// Sets the language to Greek by loading the Greek.ini file located in the "LanguageLoader.resources" folder.
        /// </summary>
        public static void SetLanguageToGreek()
        {
            SetLanguageFromINIFile(System.Windows.Forms.Application.StartupPath + @"\LanguageLoader.resources\Greek.ini");
        }

        /// <summary>
        /// Sets the language to Russian by loading the Russian.ini file located in the "LanguageLoader.resources" folder.
        /// </summary>
        public static void SetLanguageToRussian()
        {
            SetLanguageFromINIFile(System.Windows.Forms.Application.StartupPath + @"\LanguageLoader.resources\Russian.ini");
        }

        /// <summary>
        /// Sets the language to Finnish by loading the Finnish.ini file located in the "LanguageLoader.resources" folder.
        /// </summary>
        public static void SetLanguageToFinnish()
        {
            SetLanguageFromINIFile(System.Windows.Forms.Application.StartupPath + @"\LanguageLoader.resources\Finnish.ini");
        }

        /// <summary>
        /// Sets the language to Indonesian by loading the Indonesian.ini file located in the "LanguageLoader.resources" folder.
        /// </summary>
        public static void SetLanguageToIndonesian()
        {
            SetLanguageFromINIFile(System.Windows.Forms.Application.StartupPath + @"\LanguageLoader.resources\Indonesian.ini");
        }

        /// <summary>
        /// Sets the language to Na'vi by loading the Na'vi.ini file located in the "LanguageLoader.resources" folder.
        /// </summary>
        public static void SetLanguageToNavi()
        {
            SetLanguageFromINIFile(System.Windows.Forms.Application.StartupPath + @"\LanguageLoader.resources\Na'vi.ini");
        }

        public static string ShowInspector
        {
            get
            {
                return GetStr("Insp");
            }
            set
            {
                SetStr("Insp", value);
            }
        }
        public static string ActivationContextInitError
        {
            get
            {
                return GetStr("ActivationContextInitError");
            }
            set
            {
                SetStr("ActivationContextInitError", value);
            }
        }
        public static string ActivationContextActivError
        {
            get
            {
                return GetStr("ActivationContextActivError");
            }
            set
            {
                SetStr("ActivationContextActivError", value);
            }
        }
        public static string ActivationContextDeactivError
        {
            get
            {
                return GetStr("ActivationContextDeactivError");
            }
            set
            {
                SetStr("ActivationContextDeactivError", value);
            }
        }
        public static string NavBack
        {
            get
            {
                return GetStr("NavBack");
            }
            set
            {
                SetStr("NavBack", value);
            }
        }
        public static string NavForward
        {
            get
            {
                return GetStr("NavForward");
            }
            set
            {
                SetStr("NavForward", value);
            }
        }
        public static string Reload
        {
            get
            {
                return GetStr("Reload");
            }
            set
            {
                SetStr("Reload", value);
            }
        }
        public static string Stop
        {
            get
            {
                return GetStr("Stop");
            }
            set
            {
                SetStr("Stop", value);
            }
        }
        public static string ViewSource
        {
            get
            {
                return GetStr("ViewSource");
            }
            set
            {
                SetStr("ViewSource", value);
            }
        }
        public static string SearchWithGoogle
        {
            get
            {
                return GetStr("SearchWithGoogle");
            }
            set
            {
                SetStr("SearchWithGoogle", value);
            }
        }
        public static string Copy
        {
            get
            {
                return GetStr("Copy");
            }
            set
            {
                SetStr("Copy", value);
            }
        }
        public static string AllFiles
        {
            get
            {
                return GetStr("AllFiles");
            }
            set
            {
                SetStr("AllFiles", value);
            }
        }
        public static string HTMLDoc
        {
            get
            {
                return GetStr("HTMLDoc");
            }
            set
            {
                SetStr("HTMLDoc", value);
            }
        }
        public static string RenderingPage
        {
            get
            {
                return GetStr("RenderingPage");
            }
            set
            {
                SetStr("RenderingPage", value);
            }
        }
        public static string ConnectingWith
        {
            get
            {
                return GetStr("ConnectingWith");
            }
            set
            {
                SetStr("ConnectingWith", value);
            }
        }
        public static string Completed
        {
            get
            {
                return GetStr("Completed");
            }
            set
            {
                SetStr("Completed", value);
            }
        }
        public static string Print
        {
            get
            {
                return GetStr("Print");
            }
            set
            {
                SetStr("Print", value);
            }
        }
        public static string SaveAs
        {
            get
            {
                return GetStr("SaveAs");
            }
            set
            {
                SetStr("SaveAs", value);
            }
        }
    }
}