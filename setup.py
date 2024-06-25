from setuptools import setup

setup(
    name='engine',
    version='0.0.1',
    install_requires=[
        'Click',
    ],
    entry_points={
        'console_scripts': [
            'test_engine = engine.main:cli',
        ],
    },
)
