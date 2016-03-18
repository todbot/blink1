using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

namespace ThingM.Blink1.ColorManager
{
    public class ColorWheel : IDisposable
    {

        // These resources should be disposed
        // of when you're done with them.
        private Graphics g;

        private Region colorRegion;

        private Region brightnessRegion;

        private Bitmap colorImage;

        public delegate void ColorChangedEventHandler(object sender, ColorChangedEventArgs e);

        public ColorChangedEventHandler ColorChanged;

        // Keep track of the current mouse state. 
        public enum MouseState
        {
            MouseUp,

            ClickOnColor,

            DragInColor,

            ClickOnBrightness,

            DragInBrightness,

            ClickOutsideRegion,

            DragOutsideRegion,
        }

        private MouseState currentState = MouseState.MouseUp;

        // The code needs to convert back and forth between 
        // degrees and radians. There are 2*PI radians in a 
        // full circle, and 360 degrees. This constant allows
        // you to convert back and forth.
        private const double DEGREES_PER_RADIAN = 180.0 / Math.PI;

        // COLOR_COUNT represents the number of distinct colors
        // used to create the circular gradient. Its value 
        // is somewhat arbitrary -- change this to 6, for 
        // example, to see what happens. 1536 (6 * 256) seems 
        // a good compromise -- it's enough to get a full 
        // range of colors, but it doesn't overwhelm the processor
        // attempting to generate the image. The color wheel
        // contains 6 sections, and each section displays 
        // 256 colors. Seems like a reasonable compromise.
        private const int COLOR_COUNT = 6 * 256;

        private Point centerPoint;

        private int radius;

        private Rectangle colorRectangle;

        private Rectangle brightnessRectangle;

        private Rectangle selectedColorRectangle;

        private int brightnessX;

        private double brightnessScaling;

        // selectedColor is the actual value selected
        // by the user. fullColor is the same color, 
        // with its brightness set to 255.
        private Color selectedColor = Color.White;

        private Color fullColor;

        private ColorHandler.RGB RGB;

        private ColorHandler.HSV HSV;

        // Locations for the two "pointers" on the form.
        private Point colorPoint;

        private Point brightnessPoint;

        private int brightness;

        private int brightnessMin;

        private int brightnessMax;

        public ColorWheel(Rectangle colorRectangle, Rectangle brightnessRectangle, Rectangle selectedColorRectangle)
        {

            // Caller must provide locations for color wheel
            // (colorRectangle), brightness "strip" (brightnessRectangle)
            // and location to display selected color (selectedColorRectangle).

            using (GraphicsPath path = new GraphicsPath())
            {
                // Store away locations for later use. 
                this.colorRectangle = colorRectangle;
                this.brightnessRectangle = brightnessRectangle;
                this.selectedColorRectangle = selectedColorRectangle;

                // Calculate the center of the circle.
                // Start with the location, then offset
                // the point by the radius.
                // Use the smaller of the width and height of
                // the colorRectangle value.
                this.radius = (int)Math.Min(colorRectangle.Width, colorRectangle.Height) / 2;
                this.centerPoint = colorRectangle.Location;
                this.centerPoint.Offset(radius, radius);

                // Start the pointer in the center.
                this.colorPoint = this.centerPoint;

                // Create a region corresponding to the color circle.
                // Code uses this later to determine if a specified
                // point is within the region, using the IsVisible 
                // method.
                path.AddEllipse(colorRectangle);
                colorRegion = new Region(path);

                // set { the range for the brightness selector.
                this.brightnessMin = this.brightnessRectangle.Top;
                this.brightnessMax = this.brightnessRectangle.Bottom;

                // Create a region corresponding to the
                // brightness rectangle, with a little extra 
                // "breathing room". 

                path.AddRectangle(new Rectangle(brightnessRectangle.Left, brightnessRectangle.Top - 10, brightnessRectangle.Width + 10, brightnessRectangle.Height + 20));
                // Create region corresponding to brightness
                // rectangle. Later code uses this to 
                // determine if a specified point is within
                // the region, using the IsVisible method.
                brightnessRegion = new Region(path);

                // Set the location for the brightness indicator "marker".
                // Also calculate the scaling factor, scaling the height
                // to be between 0 and 255. 
                brightnessX = brightnessRectangle.Left + brightnessRectangle.Width;
                brightnessScaling = (double)255 / (brightnessMax - brightnessMin);

                // Calculate the location of the brightness
                // pointer. Assume it's at the highest position.
                brightnessPoint = new Point(brightnessX, brightnessMax);

                // Create the bitmap that contains the circular gradient.
                CreateGradient();
            }
        }

        protected void OnColorChanged(ColorHandler.RGB RGB, ColorHandler.HSV HSV)
        {
            ColorChangedEventArgs e = new ColorChangedEventArgs(RGB, HSV);
            ColorChanged(this, e);
        }

        public Color Color
        {
            get
            {
                return selectedColor;
            }
        }

        void IDisposable.Dispose()
        {
            // Dispose of graphic resources
            if (colorImage != null) colorImage.Dispose();
            if (colorRegion != null) colorRegion.Dispose();
            if (brightnessRegion != null) brightnessRegion.Dispose();
            if (g != null) g.Dispose();
        }

        public void SetMouseUp()
        {
            // Indicate that the user has
            // released the mouse.
            currentState = MouseState.MouseUp;
        }

        public void Draw(Graphics g, ColorHandler.HSV HSV)
        {
            // Given HSV values, update the screen.
            this.g = g;
            this.HSV = HSV;
            CalcCoordsAndUpdate(this.HSV);
            UpdateDisplay();
        }

        public void Draw(Graphics g, ColorHandler.RGB RGB)
        {
            // Given RGB values, calculate HSV and then update the screen.
            this.g = g;
            this.HSV = ColorHandler.RGBtoHSV(RGB);
            CalcCoordsAndUpdate(this.HSV);
            UpdateDisplay();
        }

        public void Draw(Graphics g, Point mousePoint)
        {
            // You've moved the mouse. 
            // Now update the screen to match.

            double distance;
            int degrees;
            Point delta;
            Point newColorPoint;
            Point newBrightnessPoint;
            Point newPoint;

            // Keep track of the previous color pointer point, 
            // so you can put the mouse there in case the 
            // user has clicked outside the circle.
            newColorPoint = colorPoint;
            newBrightnessPoint = brightnessPoint;

            // Store this away for later use.
            this.g = g;

            if (currentState == MouseState.MouseUp)
            {
                if (!mousePoint.IsEmpty)
                {
                    if (colorRegion.IsVisible(mousePoint))
                    {
                        // Is the mouse point within the color circle?
                        // If so, you just clicked on the color wheel.
                        currentState = MouseState.ClickOnColor;
                    }
                    else if (brightnessRegion.IsVisible(mousePoint))
                    {
                        // Is the mouse point within the brightness area?
                        // You clicked on the brightness area.
                        currentState = MouseState.ClickOnBrightness;
                    }
                    else
                    {
                        // Clicked outside the color and the brightness
                        // regions. In that case, just put the 
                        // pointers back where they were.
                        currentState = MouseState.ClickOutsideRegion;
                    }
                }
            }

            switch (currentState)
            {
                case MouseState.ClickOnBrightness:
                case MouseState.DragInBrightness:
                    // Calculate new color information
                    // based on the brightness, which may have changed.
                    newPoint = mousePoint;
                    if (newPoint.Y < brightnessMin)
                    {
                        newPoint.Y = brightnessMin;
                    }
                    else if (newPoint.Y > brightnessMax)
                    {
                        newPoint.Y = brightnessMax;
                    }
                    newBrightnessPoint = new Point(brightnessX, newPoint.Y);
                    brightness = (int)((brightnessMax - newPoint.Y) * brightnessScaling);
                    HSV.value = brightness;
                    RGB = ColorHandler.HSVtoRGB(HSV);
                    break;

                case MouseState.ClickOnColor:
                case MouseState.DragInColor:
                    // Calculate new color information
                    // based on selected color, which may have changed.
                    newColorPoint = mousePoint;

                    // Calculate x and y distance from the center,
                    // and then calculate the angle corresponding to the
                    // new location.
                    delta = new Point(mousePoint.X - centerPoint.X, mousePoint.Y - centerPoint.Y);
                    degrees = CalcDegrees(delta);

                    // Calculate distance from the center to the new point 
                    // as a fraction of the radius. Use your old friend, 
                    // the Pythagorean theorem, to calculate this value.
                    distance = Math.Sqrt(delta.X * delta.X + delta.Y * delta.Y) / radius;

                    if (currentState == MouseState.DragInColor)
                    {
                        if (distance > 1)
                        {
                            // Mouse is down, and outside the circle, but you 
                            // were previously dragging in the color circle. 
                            // What to do?
                            // In that case, move the point to the edge of the 
                            // circle at the correct angle.
                            distance = 1;
                            newColorPoint = GetPoint(degrees, radius, centerPoint);
                        }
                    }

                    // Calculate the new HSV and RGB values.
                    HSV.Hue = (int)(degrees * 255 / 360);
                    HSV.Saturation = (int)(distance * 255);
                    HSV.value = brightness;
                    RGB = ColorHandler.HSVtoRGB(HSV);
                    fullColor = ColorHandler.HSVtoColor(HSV.Hue, HSV.Saturation, 255);
                    break;
            }
            selectedColor = ColorHandler.HSVtoColor(HSV);

            // Raise an event back to the parent form,
            // so the form can update any UI it's using 
            // to display selected color values.
            OnColorChanged(RGB, HSV);

            // On the way out, set the new state.
            switch (currentState)
            {
                case MouseState.ClickOnBrightness:
                    currentState = MouseState.DragInBrightness;
                    break;
                case MouseState.ClickOnColor:
                    currentState = MouseState.DragInColor;
                    break;
                case MouseState.ClickOutsideRegion:
                    currentState = MouseState.DragOutsideRegion;
                    break;
            }

            // Store away the current points for next time.
            colorPoint = newColorPoint;
            brightnessPoint = newBrightnessPoint;

            // Draw the gradients and points. 
            UpdateDisplay();
        }

        private Point CalcBrightnessPoint(int brightness)
        {
            // Take the value for brightness (0 to 255), scale to the 
            // scaling used in the brightness bar, then add the value 
            // to the bottom of the bar. return the correct point at which 
            // to display the brightness pointer.
            return new Point(brightnessX, (int)(brightnessMax - brightness / brightnessScaling));
        }

        private void UpdateDisplay()
        {
            // Update the gradients, and place the 
            // pointers correctly based on colors and 
            // brightness.

            using (Brush selectedBrush = new SolidBrush(selectedColor))
            {
                // Draw the saved color wheel image.
                g.DrawImage(colorImage, colorRectangle);

                // Draw the "selected color" rectangle.
                g.FillRectangle(selectedBrush, selectedColorRectangle);

                // Draw the "brightness" rectangle.
                DrawLinearGradient(fullColor);
                // Draw the two pointers.
                DrawColorPointer(colorPoint);
                DrawBrightnessPointer(brightnessPoint);
            }
        }

        private void CalcCoordsAndUpdate(ColorHandler.HSV HSV)
        {
            // Convert color to real-world coordinates and then calculate
            // the various points. HSV.Hue represents the degrees (0 to 360), 
            // HSV.Saturation represents the radius. 
            // This procedure doesn't draw anything--it simply 
            // updates class-level variables. The UpdateDisplay
            // procedure uses these values to update the screen.

            // Given the angle (HSV.Hue), and distance from 
            // the center (HSV.Saturation), and the center, 
            // calculate the point corresponding to 
            // the selected color, on the color wheel.
            colorPoint = GetPoint((double)HSV.Hue / 255 * 360, (double)HSV.Saturation / 255 * radius, centerPoint);

            // Given the brightness (HSV.value), calculate the 
            // point corresponding to the brightness indicator.
            brightnessPoint = CalcBrightnessPoint(HSV.value);

            // Store information about the selected color.
            brightness = HSV.value;
            selectedColor = ColorHandler.HSVtoColor(HSV);
            RGB = ColorHandler.HSVtoRGB(HSV);

            // The full color is the same as HSV, except that the 
            // brightness is set to full (255). This is the top-most
            // color in the brightness gradient.
            fullColor = ColorHandler.HSVtoColor(HSV.Hue, HSV.Saturation, 255);
        }

        private void DrawLinearGradient(Color TopColor)
        {
            // Given the top color, draw a linear gradient
            // ranging from black to the top color. Use the 
            // brightness rectangle as the area to fill.
            using (LinearGradientBrush lgb = new LinearGradientBrush(brightnessRectangle, TopColor, Color.Black, LinearGradientMode.Vertical))
            {
                g.FillRectangle(lgb, brightnessRectangle);
            }
        }

        private int CalcDegrees(Point pt)
        {
            int degrees;

            if (pt.X == 0)
            {
                // The point is on the y-axis. Determine whether 
                // it's above or below the x-axis, and return the 
                // corresponding angle. Note that the orientation of the
                // y-coordinate is backwards. That is, A positive Y value 
                // indicates a point BELOW the x-axis.
                if (pt.Y > 0)
                {
                    degrees = 270;
                }
                else
                {
                    degrees = 90;
                }
            }
            else
            {
                // This value needs to be multiplied
                // by -1 because the y-coordinate
                // is opposite from the normal direction here.
                // That is, a y-coordinate that's "higher" on 
                // the form has a lower y-value, in this coordinate
                // system. So everything's off by a factor of -1 when
                // performing the ratio calculations.
                degrees = (int)(-Math.Atan((double)pt.Y / pt.X) * DEGREES_PER_RADIAN);

                // If the x-coordinate of the selected point
                // is to the left of the center of the circle, you 
                // need to add 180 degrees to the angle. ArcTan only
                // gives you a value on the right-hand side of the circle.
                if (pt.X < 0)
                {
                    degrees += 180;
                }

                // Ensure that the return value is 
                // between 0 and 360.
                degrees = (degrees + 360) % 360;
            }
            return degrees;
        }

        private void CreateGradient()
        {
            // Create a new PathGradientBrush, supplying
            // an array of points created by calling
            // the GetPoints method.
            using (PathGradientBrush pgb = new PathGradientBrush(GetPoints(radius, new Point(radius, radius))))
            {
                // Set the various properties. Note the SurroundColors
                // property, which contains an array of points, 
                // in a one-to-one relationship with the points
                // that created the gradient.
                pgb.CenterColor = Color.White;
                pgb.CenterPoint = new PointF(radius, radius);
                pgb.SurroundColors = GetColors();

                // Create a new bitmap containing
                // the color wheel gradient, so the 
                // code only needs to do all this 
                // work once. Later code uses the bitmap
                // rather than recreating the gradient.
                colorImage = new Bitmap(colorRectangle.Width, colorRectangle.Height, PixelFormat.Format32bppArgb);

                using (Graphics newGraphics = Graphics.FromImage(colorImage))
                {
                    newGraphics.FillEllipse(pgb, 0, 0, colorRectangle.Width, colorRectangle.Height);
                }
            }
        }

        private Color[] GetColors()
        {
            // Create an array of COLOR_COUNT
            // colors, looping through all the 
            // hues between 0 and 255, broken
            // into COLOR_COUNT intervals. HSV is
            // particularly well-suited for this, 
            // because the only value that changes
            // as you create colors is the Hue.
            Color[] Colors = new Color[COLOR_COUNT];

            for (int i = 0; i <= COLOR_COUNT - 1; i++) Colors[i] = ColorHandler.HSVtoColor((int)((double)(i * 255) / COLOR_COUNT), 255, 255);
            return Colors;
        }

        private Point[] GetPoints(double radius, Point centerPoint)
        {
            // Generate the array of points that describe
            // the locations of the COLOR_COUNT colors to be 
            // displayed on the color wheel.
            Point[] Points = new Point[COLOR_COUNT];

            for (int i = 0; i <= COLOR_COUNT - 1; i++) Points[i] = GetPoint((double)(i * 360) / COLOR_COUNT, radius, centerPoint);
            return Points;
        }

        private Point GetPoint(double degrees, double radius, Point centerPoint)
        {
            // Given the center of a circle and its radius, along
            // with the angle corresponding to the point, find the coordinates. 
            // In other words, convert from polar to rectangular coordinates.
            double radians = degrees / DEGREES_PER_RADIAN;

            return new Point((int)(centerPoint.X + Math.Floor(radius * Math.Cos(radians))), (int)(centerPoint.Y - Math.Floor(radius * Math.Sin(radians))));
        }

        private void DrawColorPointer(Point pt)
        {
            // Given a point, draw the color selector. 
            // The constant SIZE represents half
            // the width -- the square will be twice
            // this value in width and height.
            const int SIZE = 3;
            g.DrawRectangle(Pens.Black, pt.X - SIZE, pt.Y - SIZE, SIZE * 2, SIZE * 2);
        }

        private void DrawBrightnessPointer(Point pt)
        {
            // Draw a triangle for the 
            // brightness indicator that "points"
            // at the provided point.
            const int HEIGHT = 10;
            const int WIDTH = 7;

            Point[] Points = new Point[3];
            Points[0] = pt;
            Points[1] = new Point(pt.X + WIDTH, pt.Y + HEIGHT / 2);
            Points[2] = new Point(pt.X + WIDTH, pt.Y - HEIGHT / 2);
            g.FillPolygon(Brushes.Black, Points);
        }
    }
}