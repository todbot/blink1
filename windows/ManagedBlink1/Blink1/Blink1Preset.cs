// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Blink1Preset.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   Presets for a Blink(1) device.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1
{
    using ThingM.Blink1.ColorProcessor;

    /// <summary>
    ///     Presets for a Blink(1) device.
    /// </summary>
    public sealed class Blink1Preset
    {
        #region Fields

        /// <summary>
        ///     The number of milliseconds the fading will last.
        /// </summary>
        private ushort millisecond;

        #endregion

        #region Constructors and Destructors

        /// <summary>
        /// Initializes a new instance of the <see cref="Blink1Preset"/> class.
        /// </summary>
        /// <param name="millisecond">
        /// The number of milliseconds the fading will last.
        /// </param>
        /// <param name="rgb">
        /// The RGB color.
        /// </param>
        public Blink1Preset(ushort millisecond, Rgb rgb)
        {
            this.Millisecond = millisecond;

            this.Rgb = rgb;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Blink1Preset"/> class.
        /// </summary>
        /// <param name="millisecond">
        /// The number of milliseconds the fading will last.
        /// </param>
        /// <param name="colorProcessor">
        /// A color object that implements the IColorProcessor interface.
        /// </param>
        public Blink1Preset(ushort millisecond, IColorProcessor colorProcessor)
        {
            this.Millisecond = millisecond;

            this.Rgb = colorProcessor.ToRgb();
        }

        #endregion

        #region Public Properties

        /// <summary>
        ///     Gets or sets the position of the preset in the list of presets of the device.
        /// </summary>
        public ushort Millisecond
        {
            get
            {
                return this.millisecond;
            }

            set
            {
                this.millisecond = (value > 60000) ? (ushort)60000 : ((value < 500) ? (ushort)500 : value);
            }
        }

        /// <summary>
        ///     Gets or sets the RGB color.
        /// </summary>
        public Rgb Rgb { get; set; }

        #endregion
    }
}