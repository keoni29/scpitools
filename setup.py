from setuptools import setup

setup(name='scpitools',
      version='1.0.0',
      description='Tools for testing SCPI devices',
      url='',
      author='Koen van Vliet',
      author_email='8by8mail@gmail.com',
      license='MIT',
      packages=['scpitools'],
      entry_points = {
        'console_scripts': ['scpi=scpitools.scpi:main']},
      zip_safe=False,
)
