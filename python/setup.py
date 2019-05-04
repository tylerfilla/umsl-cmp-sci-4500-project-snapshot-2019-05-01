#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

from setuptools import find_packages, setup

setup(
    name='cozmonaut',
    version='0.1.0',
    url='https://github.com/cozmonauts/app',
    python_requires='>=3.6.0',
    packages=find_packages(),
    install_requires=[
        'cozmo',
    ],
)
