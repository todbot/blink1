// --------------------------------------------------------------------------------------------------------------------
// <copyright file="HtmlHexadecimal.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   HTML Color in a hexadecimal (#FFFFFF) format.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1.ColorProcessor
{
    using System;
    using System.Globalization;
    using System.Text.RegularExpressions;

    /// <summary>
    ///     HTML Color in a hexadecimal (#FFFFFF) format.
    /// </summary>
    public class HtmlHexadecimal : IColorProcessor
    {
        #region Fields

        /// <summary>
        ///     The HTML color in a hexadecimal (#FFFFFF) format.
        /// </summary>
        private string htmlColor;

        #endregion

        #region Constructors and Destructors

        /// <summary>
        /// Initializes a new instance of the <see cref="HtmlHexadecimal"/> class.
        /// </summary>
        /// <param name="htmlColor">
        /// The HTML color in a hexadecimal (#FFFFFF) format.
        /// </param>
        public HtmlHexadecimal(string htmlColor)
        {
            this.HtmlColor = htmlColor;
        }

        #endregion

        #region Public Properties

        /// <summary>
        ///     Gets or sets the HTML color in a hexadecimal (#FFFFFF) format.
        /// </summary>
        public string HtmlColor
        {
            get
            {
                return this.htmlColor;
            }

            set
            {
                if (this.IsFormatValid(value) == false)
                {
                    throw new ArgumentException("Color format is invalid. Valid format is #FFFFFF.");
                }

                this.htmlColor = value;
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
            ushort red = ushort.Parse(this.htmlColor.Substring(1, 2), NumberStyles.AllowHexSpecifier);
            ushort green = ushort.Parse(this.htmlColor.Substring(3, 2), NumberStyles.AllowHexSpecifier);
            ushort blue = ushort.Parse(this.htmlColor.Substring(5, 2), NumberStyles.AllowHexSpecifier);

            Rgb rgb = new Rgb(red, green, blue);

            return rgb;
        }

        #endregion

        #region Methods

        /// <summary>
        /// Validate if the string format match the HTML hexadecimal #FFFFFF format.
        /// </summary>
        /// <param name="color">
        /// The HTML color.
        /// </param>
        /// <returns>
        /// True if the format is valid, false otherwise.
        /// </returns>
        private bool IsFormatValid(string color)
        {
            if (string.IsNullOrEmpty(color))
            {
                return false;
            }

            Regex htmlHexColorRegex = new Regex(@"^\#([a-fA-F0-9]{6})$");

            return htmlHexColorRegex.IsMatch(color);
        }

        #endregion
    }
}