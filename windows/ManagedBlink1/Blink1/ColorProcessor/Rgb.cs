// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Rgb.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   Color in a Red, Green and Blue (RGB) format.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1.ColorProcessor
{
    /// <summary>
    ///     Color in a Red, Green and Blue (RGB) format.
    /// </summary>
    public class Rgb : IColorProcessor
    {
        #region Fields

        /// <summary>
        ///     The blue component of the color.
        /// </summary>
        private ushort blue;

        /// <summary>
        ///     The green component of the color.
        /// </summary>
        private ushort green;

        /// <summary>
        ///     The red component of the color.
        /// </summary>
        private ushort red;

        #endregion

        #region Constructors and Destructors

        /// <summary>
        /// Initializes a new instance of the <see cref="Rgb"/> class.
        /// </summary>
        /// <param name="red">
        /// The red component of the color.
        /// </param>
        /// <param name="green">
        /// The green component of the color.
        /// </param>
        /// <param name="blue">
        /// The blue component of the color.
        /// </param>
        public Rgb(ushort red, ushort green, ushort blue)
        {
            this.Red = red;

            this.Green = green;

            this.Blue = blue;
        }

        #endregion

        #region Public Properties

        /// <summary>
        ///     Gets or sets the blue component of the color.
        /// </summary>
        public ushort Blue
        {
            get
            {
                return this.blue;
            }

            set
            {
                this.blue = (value > (ushort)255) ? (ushort)255 : value;
            }
        }

        /// <summary>
        ///     Gets or sets the green component of the color.
        /// </summary>
        public ushort Green
        {
            get
            {
                return this.green;
            }

            set
            {
                this.green = (value > (ushort)255) ? (ushort)255 : value;
            }
        }

        /// <summary>
        ///     Gets or sets the red component of the color.
        /// </summary>
        public ushort Red
        {
            get
            {
                return this.red;
            }

            set
            {
                this.red = (value > (ushort)255) ? (ushort)255 : value;
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
            return this;
        }

        #endregion
    }
}