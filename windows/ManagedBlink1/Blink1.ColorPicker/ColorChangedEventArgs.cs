using System;

namespace ThingM.Blink1.ColorManager
{
    public class ColorChangedEventArgs : EventArgs
    {

        private ColorHandler.RGB mRGB;

        private ColorHandler.HSV mHSV;

        public ColorChangedEventArgs(ColorHandler.RGB RGB, ColorHandler.HSV HSV)
        {
            mRGB = RGB;
            mHSV = HSV;
        }

        public ColorHandler.RGB RGB
        {
            get
            {
                return mRGB;
            }
        }

        public ColorHandler.HSV HSV
        {
            get
            {
                return mHSV;
            }
        }
    }
}