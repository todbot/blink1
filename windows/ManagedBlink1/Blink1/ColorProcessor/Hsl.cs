// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Hsl.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   Color in a Hue, Saturation and Luminance (HSL) format.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1.ColorProcessor
{
    using System;

    /// <summary>
    ///     Color in a Hue, Saturation and Luminance (HSL) format.
    /// </summary>
    public class Hsl : IColorProcessor
    {
        #region Fields

        /// <summary>
        ///     The hue.
        /// </summary>
        private ushort hue;

        /// <summary>
        ///     The luminance.
        /// </summary>
        private ushort luminance;

        /// <summary>
        ///     The saturation.
        /// </summary>
        private ushort saturation;

        #endregion

        #region Constructors and Destructors

        /// <summary>
        /// Initializes a new instance of the <see cref="Hsl"/> class.
        /// </summary>
        /// <param name="hue">
        /// The hue component of the color.
        /// </param>
        /// <param name="saturation">
        /// The saturation component of the color.
        /// </param>
        /// <param name="luminance">
        /// the lightness component of the color.
        /// </param>
        public Hsl(ushort hue, ushort saturation, ushort luminance)
        {
            this.Hue = hue;

            this.Saturation = saturation;

            this.Luminance = luminance;
        }

        #endregion

        #region Public Properties

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
        ///     Gets or sets the luminance component of the color.
        /// </summary>
        public ushort Luminance
        {
            get
            {
                return this.luminance;
            }

            set
            {
                this.luminance = (value > (ushort)100) ? (ushort)100 : value;
            }
        }

        /// <summary>
        ///     Gets or sets saturation component of the color.
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
            if (this.luminance.Equals(100))
            {
                return new Rgb(255, 255, 255);
            }

            if (this.luminance.Equals(0))
            {
                return new Rgb(0, 0, 0);
            }

            double qq = ((this.luminance / 100.0) < 0.5) ? ((this.luminance / 100.0) * (1.0 + (this.saturation / 100.0))) : ((this.luminance / 100.0) + (this.saturation / 100.0) - ((this.luminance / 100.0) * (this.saturation / 100.0)));

            double pp = (2.0 * (this.luminance / 100.0)) - qq;

            double hk = this.hue / 360.0;

            double[] temps = new double[3];

            temps[0] = hk + (1.0 / 3.0);
            temps[1] = hk;
            temps[2] = hk - (1.0 / 3.0);

            for (int index0 = 0; index0 < 3; index0++)
            {
                if (temps[index0] < 0)
                {
                    temps[index0] += 1.0;
                }

                if (temps[index0] > 1)
                {
                    temps[index0] -= 1.0;
                }

                if ((temps[index0] * 6) < 1)
                {
                    temps[index0] = pp + ((qq - pp) * 6.0 * temps[index0]);
                }
                else if ((temps[index0] * 2.0) < 1)
                {
                    temps[index0] = qq;
                }
                else if ((temps[index0] * 3.0) < 2)
                {
                    temps[index0] = pp + ((qq - pp) * ((2.0 / 3.0) - temps[index0]) * 6.0);
                }
                else
                {
                    temps[index0] = pp;
                }
            }

            return new Rgb(
                Convert.ToUInt16(double.Parse(string.Format("{0:0.00}", temps[0] * 255.0))),
                Convert.ToUInt16(double.Parse(string.Format("{0:0.00}", temps[1] * 255.0))),
                Convert.ToUInt16(double.Parse(string.Format("{0:0.00}", temps[2] * 255.0))));
        }

        #endregion
    }
}