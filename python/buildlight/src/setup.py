#! /usr/bin/env python
import os

from setuptools import setup


PROJECT_ROOT, _ = os.path.split(__file__)
REVISION = '0.0.1'
PROJECT_NAME = 'blink1buildlight'
PROJECT_AUTHORS = "Salim Fadhley"
PROJECT_EMAILS = 'salimfadhley@gmail.com'
PROJECT_URL = "https://bitbucket.org/salimfadhley/flasher"
SHORT_DESCRIPTION = 'Build-Light controler designed to work with Blink(1) Light'

try:
    DESCRIPTION = open(os.path.join(PROJECT_ROOT, "readme.rst")).read()
except IOError:
    DESCRIPTION = SHORT_DESCRIPTION

setup(
    name=PROJECT_NAME.lower(),
    version=REVISION,
    author=PROJECT_AUTHORS,
    author_email=PROJECT_EMAILS,
    packages=['blink1buildlight', 'blink1buildlight_tests'],
    zip_safe=True,
    include_package_data=False,
    install_requires=['blink1', 'pyzmq', 'tornado'],
    test_suite='nose.collector',
    tests_require=['mock', 'nose', 'coverage'],
    url=PROJECT_URL,
    description=SHORT_DESCRIPTION,
    long_description=DESCRIPTION,
    license='MIT',
    entry_points={
        'console_scripts': [
            'bl_controller = blink1buildlight.subscriber:main',
            'bl_proxy = blink1buildlight.proxy:main',
            'bl_demo = blink1buildlight.demo:main',
            'bl_set_throb = blink1buildlight.tools:set_throb',
            'bl_set_flash = blink1buildlight.tools:set_flash'
        ]
    },
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Environment :: Console',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Natural Language :: English',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Testing',
    ],
)