#!/bin/bash
pip install --upgrade wheel
python setup.py develop sdist bdist_wheel upload
