import click
from blink1.blink1 import blink1


@click.command()
@click.option('--color', default='white', help='What colour to set the Blink(1)')
@click.option('--white_point', default='incandescent', help='The name of the white-point to use')
def shine(color, white_point):
    with blink1(switch_off=False, white_point=white_point) as b1:
        b1.fade_to_color(100, color)


if __name__ == '__main__':
    flash()