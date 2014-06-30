"""
Python implementation of Tanner Helland's color color conversion code.
http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
"""

import math

# Aproximate colour temperatures for common lighting conditions.
COLOR_TEMPERATURES={
    'candle':1900,
    'sunrise':2000,
    'incandescent':2500,
    'tungsten':3200,
    'halogen':3350,
    'sunlight':5000,
    'overcast':6000,
    'shade':7000,
    'blue-sky':10000,
    'warm-fluorescent':2700,
    'fluorescent':37500,
    'cool-fluorescent':5000,
}

def correct_output(luminosity):
    """
    :param luminosity: Input luminosity
    :return: Luminosity limited to the 0 <= l <= 255 range.
    """
    if luminosity < 0:
        val = 0
    elif luminosity > 255:
        val = 255
    else:
        val = luminosity
    return round(val)

def kelvin_to_rgb(kelvin):
    """
    Convert a color temperature given in kelvin to an approximate RGB value.

    :param kelvin: Color temp in K
    :return: Tuple of (r, g, b), equivalent color for the temperature
    """
    temp = kelvin / 100

    # Calculate Red:
    if temp <= 66:
        red = 255
    else:
        red = 329.698727446 * ((temp - 60) ** -0.1332047592)

    # Calculate Green:

    if temp <= 66:
        green = 99.4708025861 * math.log(temp) - 161.1195681661
    else:
        green = 288.1221695283 * ((temp - 60) ** -0.0755148492)

    #Calculate Blue:
    if temp > 66:
        blue = 255
    elif temp <= 19:
        blue = 0
    else:
        blue = 138.5177312231 * math.log(temp - 10) - 305.0447927307

    return tuple(correct_output(c) for c in (red, green, blue))