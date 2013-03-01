// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Hsb.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   Color in a Hue, Saturation and Brightness (HSB) format.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1.ColorProcessor
{
    using System;

    /// <summary>
    ///     Color in a Hue, Saturation and Brightness (HSB) format.
    /// </summary>
    public class Hsb : IColorProcessor
    {
        #region Fields

        /// <summary>
        ///     The brightness component of the color.
        /// </summary>
        private ushort brightness;

        /// <summary>
        ///     The hue component of the color.
        /// </summary>
        private ushort hue;

        /// <summary>
        ///     The saturation component of the color.
        /// </summary>
        private ushort saturation;

        #endregion

        #region Constructors and Destructors

        /// <summary>
        /// Initializes a new instance of the <see cref="Hsb"/> class.
        /// </summary>
        /// <param name="hue">
        /// The hue component of the color.
        /// </param>
        /// <param name="saturation">
        /// The saturation component of the color.
        /// </param>
        /// <param name="brightness">
        /// The brightness component of the color.
        /// </param>
        public Hsb(ushort hue, ushort saturation, ushort brightness)
        {
            this.Hue = hue;

            this.Saturation = saturation;

            this.Brightness = brightness;
        }

        #endregion

        #region Public Properties

        /// <summary>
        ///     Gets or sets the brightness component of the color.
        /// </summary>
        public ushort Brightness
        {
            get
            {
                return this.brightness;
            }

            set
            {
                this.brightness = (value > (ushort)100) ? (ushort)100 : value;
            }
        }

        /// <summary>
        ///     Gets or sets the hue component of the color.
        /// </summary>
        public ushort Hue
        {
            get
            {
                return this.hue;
            }

            set
            {
                this.hue = (value > (ushort)360) ? (ushort)360 : value;
            }
        }

        /// <summary>
        ///     Gets or sets the saturation component of the color.
        /// </summary>
        public ushort Saturation
        {
            get
            {
                return this.saturation;
            }

            set
            {
                this.saturation = (value > (ushort)100) ? (ushort)100 : value;
            }
        }

        #endregion

        #region Public Methods and Operators

        /// <summary>
        ///     Convert the color to the RGB format.
        /// </summary>
        /// <returns>
        ///     The color in a RGB format.
        /// </returns>
        public Rgb ToRgb()
        {
            double red = 0;
            double green = 0;
            double blue = 0;

            if (this.saturation.Equals(0) || this.Brightness.Equals(0))
            {
                red = green = blue = 0;
            }
            else
            {
                double sectorDegree = this.hue / 60.0;

                int sectorNumber = (int)Math.Floor(sectorDegree);

                double fractionalSector = sectorDegree - sectorNumber;

                double pp = (this.brightness / 100.0) * (1.0 - (this.saturation / 100.0));

                double qq = (this.brightness / 100.0) * (1.0 - ((this.saturation / 100.0) * fractionalSector));

                double tt = (this.brightness / 100.0) * (1.0 - ((this.saturation / 100.0) * (1 - fractionalSector)));

                switch (sectorNumber)
                {
                    case 0:
                        red = this.brightness / 100.0;
                        green = tt;
                        blue = pp;
                        break;

                    case 1:
                        red = qq;
                        green = this.brightness / 100.0;
                        blue = pp;
                        break;

                    case 2:
                        red = pp;
                        green = this.brightness / 100.0;
                        blue = tt;
                        break;

                    case 3:
                        red = pp;
                        green = qq;
                        blue = this.brightness / 100.0;
                        break;

                    case 4:
                        red = tt;
                        green = pp;
                        blue = this.brightness / 100.0;
                        break;

                    case 5:
                        red = this.brightness / 100.0;
                        green = pp;
                        blue = qq;
                        break;
                }
            }

            return new Rgb(
                Convert.ToUInt16(double.Parse(string.Format("{0:0.00}", red * 255.0))),
                Convert.ToUInt16(double.Parse(string.Format("{0:0.00}", green * 255.0))),
                Convert.ToUInt16(double.Parse(string.Format("{0:0.00}", blue * 255.0))));
        }

        #endregion
    }
}