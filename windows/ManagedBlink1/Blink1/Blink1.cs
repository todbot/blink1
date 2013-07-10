// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Blink1.cs" company="None, it's free for all.">
//   Copyright (c) None, it's free for all. All rights reserved.
// </copyright>
// <summary>
//   Library for communicating with Blink(1) USB devices.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace ThingM.Blink1
{
    using System;
    using System.Linq;
    using System.Threading;

    using HidLibrary;

    using ThingM.Blink1.ColorProcessor;

    /// <summary>
    ///     Library for communicating with Blink(1) USB devices.
    /// </summary>
    public class Blink1 : IDisposable, IBlink1
    {
        #region Fields

        /// <summary>
        ///     Status variable used only during the dispose of this class.
        /// </summary>
        private bool disposed;

        /// <summary>
        ///     The Human Interface Device used to communicate with the Blink(1) device.
        /// </summary>
        private HidDevice hidDevice;

        #endregion

        #region Constructors and Destructors

        /// <summary>
        ///     Initializes a new instance of the <see cref="Blink1" /> class.
        /// </summary>
        public Blink1()
        {
            this.IsAdjustingGammaLevel = false;
        }

        /// <summary>
        ///     Finalizes an instance of the <see cref="Blink1" /> class.
        /// </summary>
        ~Blink1()
        {
            this.Dispose(false);
        }

        #endregion

        #region Public Properties

        /// <summary>
        ///     Gets or sets a value indicating whether we are applying a gamma correction on the color we send to the Blink(1) device.
        /// </summary>
        public bool IsAdjustingGammaLevel { get; set; }

        /// <summary>
        ///     Gets a value indicating whether we are connected to the Blink(1) HID device.
        /// </summary>
        public bool IsConnected
        {
            get
            {
                return this.hidDevice != null && this.hidDevice.IsConnected;
            }
        }

        #endregion

        #region Public Methods and Operators

        /// <summary>
        /// Activating the Blink(1) device to play presets after a certain amount of inactivity.
        /// </summary>
        /// <param name="numberOfMillisecond">
        /// The number of milliseconds of inactivity on the device.
        /// </param>
        public void ActivateInactivityMode(ushort numberOfMillisecond)
        {
            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            byte[] buffers = new byte[this.hidDevice.Capabilities.FeatureReportByteLength];

            buffers[0] = Convert.ToByte(1);
            buffers[1] = Convert.ToByte('D');
            buffers[2] = Convert.ToByte(1);
            buffers[3] = Convert.ToByte((numberOfMillisecond / 10) >> 8);
            buffers[4] = Convert.ToByte((numberOfMillisecond / 10) % 0xff);

            this.hidDevice.WriteFeatureData(buffers);
        }

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
        public void Blink(ushort numberOfTime, ushort numberOfMillisecondOn, ushort numberOfMillisecondOff, IColorProcessor colorProcessor)
        {
            if (colorProcessor == null)
            {
                throw new ArgumentNullException("colorProcessor", "Argument must not be null.");
            }

            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            Rgb rgb = colorProcessor.ToRgb();

            this.Blink(numberOfTime, numberOfMillisecondOn, numberOfMillisecondOff, rgb.Red, rgb.Green, rgb.Blue);
        }

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
        public void Blink(ushort numberOfTime, ushort numberOfMillisecondOn, ushort numberOfMillisecondOff, ushort red, ushort green, ushort blue)
        {
            red = (red > (ushort)255) ? (ushort)255 : red;

            green = (green > (ushort)255) ? (ushort)255 : green;

            blue = (blue > (ushort)255) ? (ushort)255 : blue;

            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            for (int counter = 0; counter < numberOfTime; counter++)
            {
                this.SetColor(red, green, blue);

                Thread.Sleep(numberOfMillisecondOn);

                this.SetColor(0, 0, 0);

                Thread.Sleep(numberOfMillisecondOff);
            }
        }

        /// <summary>
        ///     Close the connection to the Blink(1) device.
        /// </summary>
        public void Close()
        {
            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            this.StopPlayingPreset();

            this.SetColor(0, 0, 0);

            this.hidDevice.CloseDevice();

            this.hidDevice.Dispose();

            this.hidDevice = null;
        }

        public void Complete()
        {
            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            this.hidDevice.CloseDevice();

            this.hidDevice.Dispose();

            this.hidDevice = null;
        }

        /// <summary>
        ///     Deactivate the Blink(1) device from playing presets after a certain amount of inactivity.
        /// </summary>
        public void DeactivateInactivityMode()
        {
            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            byte[] buffers = new byte[this.hidDevice.Capabilities.FeatureReportByteLength];

            buffers[0] = Convert.ToByte(1);
            buffers[1] = Convert.ToByte('D');
            buffers[2] = Convert.ToByte(0);

            this.hidDevice.WriteFeatureData(buffers);
        }

        /// <summary>
        ///     Dispose object's resources.
        /// </summary>
        public void Dispose()
        {
            this.Dispose(true);
        }

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
        public bool FadeToColor(ushort numberOfMillisecond, IColorProcessor colorProcessor, bool waitUntilFinished)
        {
            if (colorProcessor == null)
            {
                throw new ArgumentNullException("colorProcessor", "Argument must not be null.");
            }

            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            Rgb rgb = colorProcessor.ToRgb();

            return this.FadeToColor(numberOfMillisecond, rgb.Red, rgb.Green, rgb.Blue, waitUntilFinished);
        }

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
        public bool FadeToColor(ushort numberOfMillisecond, ushort red, ushort green, ushort blue, bool waitUntilFinished)
        {
            red = (red > (ushort)255) ? (ushort)255 : red;

            green = (green > (ushort)255) ? (ushort)255 : green;

            blue = (blue > (ushort)255) ? (ushort)255 : blue;

            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            byte[] buffers = new byte[this.hidDevice.Capabilities.FeatureReportByteLength];

            buffers[0] = Convert.ToByte(1);
            buffers[1] = Convert.ToByte('c');
            buffers[2] = Convert.ToByte(this.IsAdjustingGammaLevel ? this.AdjustGammaLevel(red) : red);
            buffers[3] = Convert.ToByte(this.IsAdjustingGammaLevel ? this.AdjustGammaLevel(green) : green);
            buffers[4] = Convert.ToByte(this.IsAdjustingGammaLevel ? this.AdjustGammaLevel(blue) : blue);
            buffers[5] = Convert.ToByte((numberOfMillisecond / 10) >> 8);
            buffers[6] = Convert.ToByte((numberOfMillisecond / 10) % 0xff);

            bool writeResult = this.hidDevice.WriteFeatureData(buffers);

            if (writeResult && waitUntilFinished)
            {
                Thread.Sleep(numberOfMillisecond);
            }

            return writeResult;
        }

        /// <summary>
        ///     Get the version of the Blink(1) device.
        /// </summary>
        /// <returns>
        ///     The Blink(1) device version number.
        /// </returns>
        public int GetVersion()
        {
            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            byte[] buffers = new byte[this.hidDevice.Capabilities.FeatureReportByteLength];

            buffers[0] = Convert.ToByte(1);
            buffers[1] = Convert.ToByte('v');

            bool writeResult = this.hidDevice.WriteFeatureData(buffers);

            if (writeResult == false)
            {
                throw new InvalidOperationException("Failed to communicate with the Blink(1) device.");
            }

            byte[] outputFeatureDatas;

            bool readResult = this.hidDevice.ReadFeatureData(Convert.ToByte(1), out outputFeatureDatas);

            if (readResult == false)
            {
                throw new InvalidOperationException("Something went wrong while reading the version number from the Blink(1) device.");
            }

            return ((outputFeatureDatas[3] - '0') * 100) + (outputFeatureDatas[4] - '0');
        }

        /// <summary>
        ///     Open a connection to the first Blink(1) device available.
        /// </summary>
        public void Open()
        {
            this.hidDevice = HidDevices.Enumerate(Blink1Constant.VendorId, Blink1Constant.ProductId).FirstOrDefault();

            if (this.hidDevice == null)
            {
                throw new InvalidOperationException("No Blink(1) device found.");
            }

            this.hidDevice.OpenDevice();
        }

        /// <summary>
        /// Open a connection to a Blink(1) device by its HID path.
        /// </summary>
        /// <param name="devicePath">
        /// The Blink(1) HID device path.
        /// </param>
        public void Open(string devicePath)
        {
            if (string.IsNullOrEmpty(devicePath))
            {
                throw new ArgumentNullException("devicePath", "Argument must not be null or empty.");
            }

            this.hidDevice = HidDevices.GetDevice(devicePath);

            if (this.hidDevice == null)
            {
                throw new InvalidOperationException("No Blink(1) device found.");
            }

            this.hidDevice.OpenDevice();
        }

        /// <summary>
        /// Play presets from the Blink(1) device starting at a specific position.
        /// </summary>
        /// <param name="position">
        /// The starting position of the preset in the list of presets of the device.
        /// </param>
        /// <returns>
        /// True if the play preset command was successfully sent to the device, False otherwise.
        /// </returns>
        public bool PlayPreset(ushort position)
        {
            if (position >= Blink1Constant.NumberOfPreset)
            {
                throw new ArgumentOutOfRangeException("position", string.Format("Valid positions are 0-{0} inclusively.", Blink1Constant.NumberOfPreset - 1));
            }

            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            byte[] buffers = new byte[this.hidDevice.Capabilities.FeatureReportByteLength];

            buffers[0] = Convert.ToByte(1);
            buffers[1] = Convert.ToByte('p');
            buffers[2] = Convert.ToByte(1);
            buffers[3] = Convert.ToByte(position);

            return this.hidDevice.WriteFeatureData(buffers);
        }

        /// <summary>
        /// Read a preset from the Blink(1) device.
        /// </summary>
        /// <param name="position">
        /// The position of the preset in the list of presets of the device.
        /// </param>
        /// <returns>
        /// The preset found at the specified position.
        /// </returns>
        public Blink1Preset ReadPreset(uint position)
        {
            if (position >= Blink1Constant.NumberOfPreset)
            {
                throw new ArgumentOutOfRangeException("position", string.Format("Valid positions are 0-{0} inclusively.", Blink1Constant.NumberOfPreset - 1));
            }

            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            byte[] buffers = new byte[this.hidDevice.Capabilities.FeatureReportByteLength];

            buffers[0] = Convert.ToByte(1);
            buffers[1] = Convert.ToByte('R');
            buffers[2] = Convert.ToByte(0);
            buffers[3] = Convert.ToByte(0);
            buffers[4] = Convert.ToByte(0);
            buffers[5] = Convert.ToByte(0);
            buffers[6] = Convert.ToByte(0);
            buffers[7] = Convert.ToByte(position);

            bool writeResult = this.hidDevice.WriteFeatureData(buffers);

            if (writeResult == false)
            {
                throw new InvalidOperationException("Failed to communicate with the Blink(1) device.");
            }

            byte[] outputFeatureDatas;

            bool readResult = this.hidDevice.ReadFeatureData(Convert.ToByte(1), out outputFeatureDatas);

            if (readResult == false)
            {
                throw new InvalidOperationException("Something went wrong while reading the preset from the Blink(1) device.");
            }

            ushort red = Convert.ToUInt16(outputFeatureDatas[2]);
            ushort green = Convert.ToUInt16(outputFeatureDatas[3]);
            ushort blue = Convert.ToUInt16(outputFeatureDatas[4]);
            int millisecond = ((outputFeatureDatas[5] << 8) + (outputFeatureDatas[6] & 0xff)) * 10;

            return new Blink1Preset((ushort)millisecond, new Rgb(red, green, blue));
        }

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
        public bool SavePreset(Blink1Preset blink1Preset, uint position)
        {
            if (blink1Preset == null)
            {
                throw new ArgumentNullException("blink1Preset", "Argument must not be null.");
            }

            if (position >= Blink1Constant.NumberOfPreset)
            {
                throw new ArgumentOutOfRangeException("position", string.Format("Valid positions are 0-{0} inclusively.", Blink1Constant.NumberOfPreset - 1));
            }

            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            byte[] buffers = new byte[this.hidDevice.Capabilities.FeatureReportByteLength];

            buffers[0] = Convert.ToByte(1);
            buffers[1] = Convert.ToByte('P');
            buffers[2] = Convert.ToByte(blink1Preset.Rgb.Red);
            buffers[3] = Convert.ToByte(blink1Preset.Rgb.Green);
            buffers[4] = Convert.ToByte(blink1Preset.Rgb.Blue);
            buffers[5] = Convert.ToByte((blink1Preset.Millisecond / 10) >> 8);
            buffers[6] = Convert.ToByte((blink1Preset.Millisecond / 10) % 0xff);
            buffers[7] = Convert.ToByte(position);

            return this.hidDevice.WriteFeatureData(buffers);
        }

        /// <summary>
        /// Set the Blink(1) device to display a specific color.
        /// </summary>
        /// <param name="colorProcessor">
        /// A color object that implements the IColorProcessor interface.
        /// </param>
        /// <returns>
        /// True if the color was successfully applied, False otherwise.
        /// </returns>
        public bool SetColor(IColorProcessor colorProcessor)
        {
            if (colorProcessor == null)
            {
                throw new ArgumentNullException("colorProcessor", "Argument must not be null.");
            }

            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            Rgb rgb = colorProcessor.ToRgb();

            return this.SetColor(rgb.Red, rgb.Green, rgb.Blue);
        }

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
        public bool SetColor(ushort red, ushort green, ushort blue)
        {
            red = (red > (ushort)255) ? (ushort)255 : red;

            green = (green > (ushort)255) ? (ushort)255 : green;

            blue = (blue > (ushort)255) ? (ushort)255 : blue;

            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            byte[] buffers = new byte[this.hidDevice.Capabilities.FeatureReportByteLength];

            buffers[0] = Convert.ToByte(1);
            buffers[1] = Convert.ToByte('n');
            buffers[2] = Convert.ToByte(this.IsAdjustingGammaLevel ? this.AdjustGammaLevel(red) : red);
            buffers[3] = Convert.ToByte(this.IsAdjustingGammaLevel ? this.AdjustGammaLevel(green) : green);
            buffers[4] = Convert.ToByte(this.IsAdjustingGammaLevel ? this.AdjustGammaLevel(blue) : blue);

            return this.hidDevice.WriteFeatureData(buffers);
        }

        /// <summary>
        ///     Stop playing presets from the Blink(1) device.
        /// </summary>
        /// <returns>
        ///     True if the stop playing preset command was successfully sent to the device, False otherwise.
        /// </returns>
        public bool StopPlayingPreset()
        {
            if (this.IsConnected == false)
            {
                throw new InvalidOperationException("No Blink(1) device connected.");
            }

            byte[] buffers = new byte[this.hidDevice.Capabilities.FeatureReportByteLength];

            buffers[0] = Convert.ToByte(1);
            buffers[1] = Convert.ToByte('p');
            buffers[2] = Convert.ToByte(0);
            buffers[3] = Convert.ToByte(0);

            return this.hidDevice.WriteFeatureData(buffers);
        }

        #endregion

        #region Methods

        /// <summary>
        /// Adjust the gamma level of one component of the RGB color.
        /// </summary>
        /// <param name="rbgColorComponent">
        /// One component of the RGB color.
        /// </param>
        /// <returns>
        /// The gamma level adjusted component of the RGB color.
        /// </returns>
        private ushort AdjustGammaLevel(ushort rbgColorComponent)
        {
            return Convert.ToUInt16(((1 << (rbgColorComponent / 32)) - 1) + ((1 << (rbgColorComponent / 32)) * ((rbgColorComponent % 32) + 1) + 15) / 32);
        }

        /// <summary>
        ///     Cleans up the managed and unmanaged resources.
        /// </summary>
        private void CleanUp()
        {
            if (this.hidDevice != null && this.hidDevice.IsConnected)
            {
                this.hidDevice.CloseDevice();
            }

            if (this.hidDevice != null)
            {
                this.hidDevice.Dispose();
            }
        }

        /// <summary>
        /// Manages the state of the dispose operation.
        /// </summary>
        /// <param name="disposing">
        /// The state of the dispose.
        /// </param>
        private void Dispose(bool disposing)
        {
            if (this.disposed)
            {
                return;
            }

            if (disposing)
            {
                this.CleanUp();

                GC.SuppressFinalize(this);
            }

            this.disposed = true;
        }

        #endregion
    }
}