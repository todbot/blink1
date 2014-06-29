#!/bin/bash
pip install --upgrade wheel
cd src
python setup.py develop sdist bdist_wheel upload
