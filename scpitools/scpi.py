#!/bin/python
""" SCPI Tools console script

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

import argparse
import time
import logging
import glob
from .scpidevice import ScpiDevice

# TODO provide a resource manager, which can scan for devices on various interfaces

def main():
    SLEEP_COMMAND = 'SLEEP:'

    parser = argparse.ArgumentParser()
    parser.add_argument('queries', nargs='*', default='*IDN?')
    parser.add_argument('--devices', nargs='?', default='/dev/usbtmc*', help="Path to device, i.e. /dev/usbtmc1. Supports wildcards i.e. /dev/usbtmc[0-9]*")
    parser.add_argument('-d', '--debug', action='store_true', help="Enable debug output")
    args = parser.parse_args()
    
    # Option: debug, Enable debug output
    if args.debug:
        level = logging.DEBUG
    else:
        level = logging.WARNING
    logging.basicConfig(level=level)
    
    results = []

    queries = args.queries
    if isinstance(queries, str):
        queries = [queries]

    for device in glob.glob(args.devices):
        
            result = {'device':device}
            for query in queries:
                logging.info(query)
                if query.startswith(SLEEP_COMMAND):
                    seconds = float(query.removeprefix(SLEEP_COMMAND))
                    logging.debug(f"Sleep for {seconds} seconds...")
                    time.sleep(seconds)
                else:
                    response = ScpiDevice(device, timeout=5).query(query)

                    if query in result.keys():
                        query+="_"
                    
                    result.update({query:response})

            results.append(result)
                

    print(results)

if __name__ == "__main__":
    exit(main())