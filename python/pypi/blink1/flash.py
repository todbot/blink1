import time

import click
import webcolors
from blink1.blink1 import Blink1


@click.command()
@click.option('--on', default='white', help='Color to flash on')
@click.option('--off', default='black', help='Color to flash off')
@click.option('--duration', default=1, help='Length of each flash cycle in seconds')
@click.option('--repeat', default=5, help='Number of times to flash')
@click.option('--fade', default=0.1, help='Fade time in seconds')
def flash(on, off, duration, repeat, fade):
    blink1 = Blink1()


    for i in range(0, repeat):
        blink1.fade_to_color(fade * 1000, on)
        time.sleep(duration)
        blink1.fade_to_color(fade * 1000, off)
        time.sleep(duration)

    blink1.fade_to_rgb(fade * 1000, 0, 0, 0)


if __name__ == '__main__':
    flash()