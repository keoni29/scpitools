# Python SCPI tools

Control instruments with SCPI commands.

1. Supported Interfaces
2. Installing
3. Console script
4. Python module
5. Common SCPI commands

# Supported Interfaces

## USB
Windows: Not available

Linux: Using usbtmc driver. Read write to /dev/usbtmc*

Create a new udev rule
```bash
sudo echo 'KERNEL=="usbtmc[0-9]*",MODE="0660", GROUP="usbtmc"' > 99-usbtmc.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

Create a group and add your user to it

```bash
sudo groupadd usbtmc
sudo usermod -a -G usbtmc "$USER"
```

Now log out and log back in.

Check if you are part of the group usbtmc.

```bash
id
```

You should see something like this:

```
uid=1000(spamuser) gid=1000(spamuser) groups=1000(spamuser)27(sudo)100(users),1002(usbtmc)
```

## Serial
Using pyserial

Windows: COM*

Install serial device drivers.

Linux: /dev/tty*

Add yourself to the dialout group and relog. Check with "id" command to see if you are now part of the dialout group.

```bash
sudo usermod -a -G dialout "$USER"
```

# Installing

Optional: create a virtual environment and activate it
``` bash
python -m venv .venv
source .venv/bin/activate
```

Install using
```bash
python setup.py install
```

# Console script

The basic console script structure is
scpi [--devices [DEVICES]] [queries ...]

Where DEVICES is the path to your device, i.e. /dev/usbtmc0, or multiple devices using wildcards like /dev/usbtmc[0-9]*

The list of queries will be sent to each device. If no query is entered, the default query "*IDN?" is sent.

## Scan for instruments
To scan for instruments, you can use

```bash
scpi --devices=/dev/usbtmc*
```

This sends the "*IDN?" query to all usbtmc devices.

If your device is /dev/usbtmc5, this should return something like
```python
[{'device': '/dev/usbtmc5', '*IDN?': 'Agilent Technologies,34405A,MY52240109,1.47-3.13'}]
```

## Measure voltage
To measure DC voltage

```bash
scpi MEAS:VOLT:DC?
```
## Sleep between queries
Sometimes you need to introduce a delay between two commands. For this, the SLEEP:[seconds] command is introduced. This does not actually get sent to the instrument, but is interpreted by the program. To sleep for 0.5 seconds between two measurements
```
scpi MEAS:VOLT:DC? SLEEP:0.5 MEAS:VOLT:DC?
```

Should return something like
```
[{'device': '/dev/usbtmc5', 'MEAS:VOLT:DC?': '+6.31000000E-03', 'MEAS:VOLT:DC?_': '+1.48500000E-02'}]
```
Notice how the second query is suffixed with an underscore. This is because python dicts cannot have multiple keys with the same name. For each subsequent query an underscore is added to the end of the key.

## Watch a measurement

Most linux distros include the "watch" command. This is useful for watching a measured value change. You can set the watch interval in seconds using the -n parameter.


```bash
watch -n 0.5 scpi MEAS:VOLT:DC?
```
# Python module

To connect to a single device use the following example. Make sure to change /dev/usbtmc0 to your device.

```python
import scpitools as scpi

device = scpi.ScpiDevice('/dev/usbtmc0')
print(device.query('*IDN?'))
```

More examples are located in the ./examples/ directory.