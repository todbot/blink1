// --------------------------------------------------------------------------------------------------------------------
// <copyright file="IColorProcessor.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   The Color interface.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1.ColorProcessor
{
    /// <summary>
    /// The Color interface.
    /// </summary>
    public interface IColorProcessor
    {
        #region Public Methods and Operators

        /// <summary>
        ///     Convert the color to the RGB format.
        /// </summary>
        /// <returns>
        ///     The color in a RGB format.
        /// </returns>
        Rgb ToRgb();

        #endregion
    }
}