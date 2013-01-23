// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Blink1Info.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   Blink(1) device information.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1
{
    using System.Collections.Generic;
    using System.Linq;

    using HidLibrary;

    /// <summary>
    ///     Blink(1) device information.
    /// </summary>
    public static class Blink1Info
    {
        #region Public Methods and Operators

        /// <summary>
        ///     Get a list of Blink(1) HID device path.
        /// </summary>
        /// <returns>
        ///     A list of string representing the HID path of the Blink(1) device(s) found.
        /// </returns>
        public static List<string> GetDevicePath()
        {
            List<string> devicePaths = new List<string>();

            IEnumerable<HidDevice> hidDevices = HidDevices.Enumerate(Blink1Constant.VendorId, Blink1Constant.ProductId);

            foreach (HidDevice hidDevice in hidDevices.ToArray())
            {
                devicePaths.Add(hidDevice.DevicePath);

                hidDevice.Dispose();
            }

            devicePaths.Sort();

            return devicePaths;
        }

        #endregion
    }
}