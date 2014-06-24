import click
from blink1.blink1 import blink1


@click.command()
@click.option('--color', default='white', help='What colour to set the Blink(1)')
def shine(color):
    with blink1(False) as b1:
        b1.fade_to_color(100, color)


if __name__ == '__main__':
    flash()