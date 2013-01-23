// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Cmyk.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   Color in a Cyan, Magenta, Yellow and Black (CYMK) format.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1.ColorProcessor
{
    using System;

    /// <summary>
    ///     Color in a Cyan, Magenta, Yellow and Black (CYMK) format.
    /// </summary>
    public class Cmyk : IColorProcessor
    {
        #region Fields

        /// <summary>
        ///     The black component of the color.
        /// </summary>
        private ushort black;

        /// <summary>
        ///     The cyan component of the color.
        /// </summary>
        private ushort cyan;

        /// <summary>
        ///     The magenta component of the color.
        /// </summary>
        private ushort magenta;

        /// <summary>
        ///     The yellow component of the color.
        /// </summary>
        private ushort yellow;

        #endregion

        #region Constructors and Destructors

        /// <summary>
        /// Initializes a new instance of the <see cref="Cmyk"/> class.
        /// </summary>
        /// <param name="cyan">
        /// The cyan component of the color.
        /// </param>
        /// <param name="magenta">
        /// The magenta component of the color.
        /// </param>
        /// <param name="yellow">
        /// The yellow component of the color.
        /// </param>
        /// <param name="black">
        /// The black component of the color.
        /// </param>
        public Cmyk(ushort cyan, ushort magenta, ushort yellow, ushort black)
        {
            this.Cyan = cyan;

            this.Magenta = magenta;

            this.Yellow = yellow;

            this.Black = black;
        }

        #endregion

        #region Public Properties

        /// <summary>
        ///     Gets or sets the black component of the color.
        /// </summary>
        public ushort Black
        {
            get
            {
                return this.black;
            }

            set
            {
                this.black = (value > (ushort)100) ? (ushort)100 : value;
            }
        }

        /// <summary>
        ///     Gets or sets the cyan component of the color.
        /// </summary>
        public ushort Cyan
        {
            get
            {
                return this.cyan;
            }

            set
            {
                this.cyan = (value > (ushort)100) ? (ushort)100 : value;
            }
        }

        /// <summary>
        ///     Gets or sets the magenta component of the color.
        /// </summary>
        public ushort Magenta
        {
            get
            {
                return this.magenta;
            }

            set
            {
                this.magenta = (value > (ushort)100) ? (ushort)100 : value;
            }
        }

        /// <summary>
        ///     Gets or sets the yellow component of the color.
        /// </summary>
        public ushort Yellow
        {
            get
            {
                return this.yellow;
            }

            set
            {
                this.yellow = (value > (ushort)100) ? (ushort)100 : value;
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
            ushort red = Convert.ToUInt16((1.0 - (this.cyan / 100.0)) * (1.0 - (this.black / 100.0)) * 255.0);

            ushort green = Convert.ToUInt16((1.0 - (this.magenta / 100.0)) * (1.0 - (this.black / 100.0)) * 255.0);

            ushort blue = Convert.ToUInt16((1.0 - (this.yellow / 100.0)) * (1.0 - (this.black / 100.0)) * 255.0);

            return new Rgb(red, green, blue);
        }

        #endregion
    }
}