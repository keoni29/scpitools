""" MIT License

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

import serial

def ScpiDevice(device, *args, **kwargs):
    if device.startswith('/dev/tty'):
        return SerialDevice(device, *args, **kwargs)
    elif device.startswith('/dev/usbtmc'):
        return UsbTmcDevice(device, *args, **kwargs)
    else:
        raise ValueError(f"Unsupported device type {device}")


class _ScipiDevice():
    def query(self, command):
        self.write(command)
        return self.read()

class SerialDevice(_ScipiDevice):
    def __init__(self, *args, **kwargs):
        self.ser = serial.Serial(*args, **kwargs)
    
    def write(self, command):
        self.ser.write((command + '\n').encode('utf-8'))
    
    def read(self):
        response = bytearray()
        while True:
            b = self.ser.read(1)
            if not b:
                break  # EOF or timeout
            response += b
            if b == b'\n':
                break

        return response.decode().strip()

class UsbTmcDevice(_ScipiDevice):
    # TODO: implement timeout time
    def __init__(self, device, *args, **kwargs):
        self.device = device
    
    def write(self, command):
        with open(self.device, 'wb') as dev:
            dev.write((command + '\n').encode('utf-8'))

    def read(self):
        with open(self.device, 'rb', buffering=0) as dev:
            response = bytearray()
            while True:
                b = dev.read(1)
                if not b:
                    break  # EOF or timeout
                response += b
                if b == b'\n':
                    break
            return response.decode().strip()