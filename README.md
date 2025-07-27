# Python SCPI tools

1. Device permissions
2. Installing
3. Usage examples
4. Common SCPI commands

# 1. Device permissions

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

# 2. Installing

You will need setuptools

```bash
python -m pip install setuptools
```

Install using
```bash
python setup.py install
```

# 3. Usage examples

## Scan for instruments
```bash
scpi
```

Should return something like
```python
[{'device': '/dev/usbtmc5', 'idn': 'Agilent Technologies,34405A,MY52240109,1.47-3.13'}]
```

# 4. Common SCPI commands
```
*CLS

Clears the instrument status byte by emptying the error queue and clearing all event registers. Also cancels any preceding *OPC command or query.
*ESE - Event Status Enable

Sets bits in the standard event status enable register.
*ESE? - Event Status Enable Query

Returns the results of the standard event enable register. The register is cleared after reading it.
*ESR? - Event Status Enable Register

Reads and clears event status enable register.
*IDN? - Identify

Returns a string that uniquely identifies the FieldFox. The string is of the form "Keysight Technologies",<model number>,<serial number>,<software revision> and so forth.
*OPC - Operation complete command

Generates the OPC message in the standard event status register when all pending overlapped operations have been completed (for example, a sweep, or a Default).
*OPC? - Operation complete query

Returns an ASCII "+1" when all pending overlapped operations have been completed.
*OPT? - Identify Options Query

Returns a string identifying the analyzer option configuration.
*RST - Reset

Executes a device reset and cancels any pending *OPC command or query. All trigger features in HOLD. The contents of the FieldFox non-volatile memory are not affected by this command.
*SRE - Service Request Enable

Before reading a status register, bits must be enabled. This command enables bits in the service request register. The current setting is saved in non-volatile memory.
*SRE? - Service Request Enable Query

Reads the current state of the service request enable register. The register is cleared after reading it.
*WAI - Wait

Prohibits the instrument from executing any new commands until all pending overlapped commands have been completed.
```