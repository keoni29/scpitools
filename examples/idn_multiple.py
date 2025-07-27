""" Request basic info from multiple scpi devices.

    MIT License

    Copyright (c) 2025 Koen van Vliet

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
"""
import scpitools as scpi
import glob

timeout = 2

for device_group in ['/dev/ttyS[0-9]*', '/dev/ttyUSB[0-9]*', '/dev/ttyACM[0-9]*', '/dev/usbtmc[0-9]*']:
    for device_path in glob.glob(device_group):
        try:
            device = scpi.ScpiDevice(device_path, timeout=timeout)
            response = device.query('*IDN?')
            print(f'"{device_path}","*IDN?"', f'response is "{response}"' if response else 'response timed out')
        except:
            # Ignore devices that cannot be configured
            pass