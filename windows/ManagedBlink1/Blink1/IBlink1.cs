// --------------------------------------------------------------------------------------------------------------------
// <copyright file="IBlink1.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   The Blink1 interface.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1
{
    using ThingM.Blink1.ColorProcessor;

    /// <summary>
    ///     The Blink1 interface.
    /// </summary>
    public interface IBlink1
    {
        #region Public Properties

        /// <summary>
        ///     Gets or sets a value indicating whether we are applying a gamma correction on the color we send to the Blink(1) device.
        /// </summary>
        bool IsAdjustingGammaLevel { get; set; }

        /// <summary>
        ///     Gets a value indicating whether we are connected to the Blink(1) HID device.
        /// </summary>
        bool IsConnected { get; }

        #endregion

        #region Public Methods and Operators

        /// <summary>
        /// Activating the Blink(1) device to play presets after a certain amount of inactivity.
        /// </summary>
        /// <param name="numberOfMillisecond">
        /// The number of milliseconds of inactivity on the device.
        /// </param>
        void ActivateInactivityMode(ushort numberOfMillisecond);

        /// <summary>
        /// Set the Blink(1) device to blink with specific color.
        /// </summary>
        /// <param name="numberOfTime">
        /// Number of times the blink event will occur.
        /// </param>
        /// <param name="numberOfMillisecondOn">
        /// The number Of millisecond displaying the color.
        /// </param>
        /// <param name="numberOfMillisecondOff">
        /// The number Of millisecond Off.
        /// </param>
        /// <param name="colorProcessor">
        /// A color object that implements the IColorProcessor interface.
        /// </param>
        void Blink(ushort numberOfTime, ushort numberOfMillisecondOn, ushort numberOfMillisecondOff, IColorProcessor colorProcessor);

        /// <summary>
        /// Set the Blink(1) device to blink with a specific Red, Green and Blue (RGB) color.
        /// </summary>
        /// <param name="numberOfTime">
        /// Number of times the blink event will occur.
        /// </param>
        /// <param name="numberOfMillisecondOn">
        /// The number Of millisecond displaying the color.
        /// </param>
        /// <param name="numberOfMillisecondOff">
        /// The number Of millisecond Off.
        /// </param>
        /// <param name="red">
        /// The Red component of the RGB value.
        /// </param>
        /// <param name="green">
        /// The Green component of the RGB value.
        /// </param>
        /// <param name="blue">
        /// The Blue component of the RGB value.
        /// </param>
        void Blink(ushort numberOfTime, ushort numberOfMillisecondOn, ushort numberOfMillisecondOff, ushort red, ushort green, ushort blue);

        /// <summary>
        ///     Close the connection to the Blink(1) device.
        /// </summary>
        void Close();

        /// <summary>
        ///     Deactivate the Blink(1) device from playing presets after a certain amount of inactivity.
        /// </summary>
        void DeactivateInactivityMode();

        /// <summary>
        /// Fade to a color over a period of milliseconds.
        /// </summary>
        /// <param name="numberOfMillisecond">
        /// The number of milliseconds the fading will last.
        /// </param>
        /// <param name="colorProcessor">
        /// A color object that implements the IColorProcessor interface.
        /// </param>
        /// <param name="waitUntilFinished">
        /// Specify to wait (True) or not (False) for the color fade to be finished before giving back the control.
        /// </param>
        /// <returns>
        /// True if the color was successfully applied, False otherwise.
        /// </returns>
        bool FadeToColor(ushort numberOfMillisecond, IColorProcessor colorProcessor, bool waitUntilFinished);

        /// <summary>
        /// Fade to a specific Red, Green and Blue (RGB) color over a period of milliseconds.
        /// </summary>
        /// <param name="numberOfMillisecond">
        /// The number of milliseconds the fading will last.
        /// </param>
        /// <param name="red">
        /// The Red component of the RGB value.
        /// </param>
        /// <param name="green">
        /// The Green component of the RGB value.
        /// </param>
        /// <param name="blue">
        /// The Blue component of the RGB value.
        /// </param>
        /// <param name="waitUntilFinished">
        /// Specify to wait (True) or not (False) for the color fade to be finished before giving back the control.
        /// </param>
        /// <returns>
        /// True if the color was successfully applied, False otherwise.
        /// </returns>
        bool FadeToColor(ushort numberOfMillisecond, ushort red, ushort green, ushort blue, bool waitUntilFinished);

        /// <summary>
        ///     Get the version of the Blink(1) device.
        /// </summary>
        /// <returns>
        ///     The Blink(1) device version number.
        /// </returns>
        int GetVersion();

        /// <summary>
        ///     Open a connection to the first Blink(1) device available.
        /// </summary>
        void Open();

        /// <summary>
        /// Open a connection to a Blink(1) device by its HID path.
        /// </summary>
        /// <param name="devicePath">
        /// The Blink(1) HID device path.
        /// </param>
        void Open(string devicePath);

        /// <summary>
        /// Play presets from the Blink(1) device starting at a specific position.
        /// </summary>
        /// <param name="position">
        /// The starting position of the preset in the list of presets of the device.
        /// </param>
        /// <returns>
        /// True if the play preset command was successfully sent to the device, False otherwise.
        /// </returns>
        bool PlayPreset(ushort position);

        /// <summary>
        /// Read a preset from the Blink(1) device.
        /// </summary>
        /// <param name="position">
        /// The position of the preset in the list of presets of the device.
        /// </param>
        /// <returns>
        /// The preset found at the specified position.
        /// </returns>
        Blink1Preset ReadPreset(uint position);

        /// <summary>
        /// Save a preset in the Blink(1) device, so it can be played back at a later time.
        /// </summary>
        /// <param name="blink1Preset">
        /// The presets for the Blink(1) device.
        /// </param>
        /// <param name="position">
        /// The position of the preset in the list of presets of the device.
        /// </param>
        /// <returns>
        /// True if the preset was saved successfully.
        /// </returns>
        bool SavePreset(Blink1Preset blink1Preset, uint position);

        /// <summary>
        /// Set the Blink(1) device to display a specific color.
        /// </summary>
        /// <param name="colorProcessor">
        /// A color object that implements the IColorProcessor interface.
        /// </param>
        /// <returns>
        /// True if the color was successfully applied, False otherwise.
        /// </returns>
        bool SetColor(IColorProcessor colorProcessor);

        /// <summary>
        /// Set the Blink(1) device to display a specific Red, Green and Blue (RGB) color.
        /// </summary>
        /// <param name="red">
        /// The Red component of the RGB value.
        /// </param>
        /// <param name="green">
        /// The Green component of the RGB value.
        /// </param>
        /// <param name="blue">
        /// The Blue component of the RGB value.
        /// </param>
        /// <returns>
        /// True if the color was successfully applied, False otherwise.
        /// </returns>
        bool SetColor(ushort red, ushort green, ushort blue);

        /// <summary>
        ///     Stop playing presets from the Blink(1) device.
        /// </summary>
        /// <returns>
        ///     True if the stop playing preset command was successfully sent to the device, False otherwise.
        /// </returns>
        bool StopPlayingPreset();

        #endregion
    }
}