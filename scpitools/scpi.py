#!/bin/python

import glob

def send_scpi_command(device_path, command):
    with open(device_path, 'wb') as dev:
        dev.write((command + '\n').encode('utf-8'))

    with open(device_path, 'rb', buffering=0) as dev:
        response = bytearray()
        while True:
            b = dev.read(1)
            if not b:
                break  # EOF or timeout
            response += b
            if b == b'\n':
                break
        return response.decode().strip()

def main():
    import argparse
    import time
    import logging
    

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
    
    instruments = []

    queries = args.queries
    if isinstance(queries, str):
        queries = [queries]

    for device in glob.glob(args.devices):
        
            instrument = {'device':device}
            for query in queries:
                logging.info(query)
                if query.startswith(SLEEP_COMMAND):
                    seconds = float(query.removeprefix(SLEEP_COMMAND))
                    logging.debug(f"Sleep for {seconds} seconds...")
                    time.sleep(seconds)
                else:
                    response = send_scpi_command(device, query)
                    if query in instrument.keys():
                        query+="_"
                    
                    instrument.update({query:response})

            instruments.append(instrument)
                

    print(instruments)

if __name__ == "__main__":
    exit(main())