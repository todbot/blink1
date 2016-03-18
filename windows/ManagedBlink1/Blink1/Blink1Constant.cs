// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Blink1Constant.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   Constants for the Library for communicating with Blink(1) USB devices.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1
{
    /// <summary>
    ///     Constants for the Library for communicating with Blink(1) USB devices.
    /// </summary>
    public static class Blink1Constant
    {
        #region Constants

        /// <summary>
        ///     The maximum number of presets supported by a Blink(1) device:
        ///       1) for blink(1) mk1, 12 lines
        ///       2) for blink(1) mk2, 32 lines  
        /// </summary>
        public const int NumberOfPreset = 32;

        /// <summary>
        ///     The product id (493).
        /// </summary>
        public const int ProductId = 0x01ED;

        /// <summary>
        ///     The vendor id (10168).
        /// </summary>
        public const int VendorId = 0x27B8;

        #endregion
    }
}