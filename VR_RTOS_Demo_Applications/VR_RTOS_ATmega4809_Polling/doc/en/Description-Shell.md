
5.000 / 5.000
# Shell Description (ATMega 4809)

## General

The shell is a task that provides access to the HAL (Hardware Abstraction Layer) and certain functions of the VR-RTOS scheduler.

For example, all digital and analog I/O can be read and modified.

The shell communicates with the user via the RS232 task.
Interface parameters are: 38400, 8, N, 1

### Modes

Several modes are available.

1. Normal Mode
2. Production Mode (not implemented)
3. Test Mode
4. Manual Mode
5. Expert Mode
6. EMC Mode (not implemented)

##### Normal Mode
The shell is started in this mode. Only a few commands are available.

You can only switch to another mode from this mode. If you are in another mode, you must switch back to this mode to switch to a different mode.

Commands to switch to another mode:
- facton
- teston
- manuon
- expton

To switch back to normal mode:
- factoff
- testoff
- manuoff
- exptoff

All commands in normal mode are also available in all other modes.

- help - lists all commands supported in the active mode
- sys - displays various operating system parameters
- ver - displays various version data

##### Manual Mode
In this mode, all outputs (analog and digital) are controlled exclusively by the shell.
The normal program cannot control any outputs.
This is the typical mode for commissioning and repair.

##### Test Mode
This mode is the opposite of manual mode.
All outputs (analog and digital) are controlled by the normal program.
The shell has no control over the outputs.
The shell only controls the inputs; the normal program cannot read any inputs.


##### Expert Mode
In this mode, the normal program continues to run, but the user can make certain
interventions.

##### Production Mode
Currently not implemented.
Intended for saving production data or calibration in manufacturing, etc.

##### EMC Mode
Currently not implemented.
Intended for special program properties during EMC measurements, e.g., shortened cycle time, etc.

### List of all shell commands AnaIn
| Command | Mode | Explanation |
|----------|------------------------|----------------------------------------|
| ai | Expert, Manual, Test | Measures and displays all voltages and currents |
| ais | Test | Sets an AnaIn value |
| repai | Expert, Manual | Displays all AnaIn values ​​cyclically |

### List of all shell commands DigIn
| Command | Mode | Explanation |
|----------|-------------|----------------------------------------|
| di | Expert, Manual, Test | Reads and displays all DigIns |
| dir | Test | Sets a DigIn value to 0 |
| dis | Test | Sets a DigIn value to 1 |
| dit | Test | Toggles a DigIn value |
| repdi | Expert, Manual | Reads and displays all DigIns cyclically |

### List of all shell commands DigOut
| Command | Mode | Explanation |
|----------|-------------|----------------------------------------|
| do | Expert, Manual, Test | Reads and displays all DigOuts |
| dor | Expert, Manual | Set a DigOut value to 0 |
| dos | Expert, Manual | Set a DigOut value to 1 |
| dot | Expert, Manual | Toggle a DigOut value |
| repdo | Expert | Reads all DigOut values ​​and displays them cyclically |

### List of all shell commands PwmOut
| Command | Mode | Explanation |
|----------|-------------|-----------------------------------------|
| po | Manual | Reads all PwmOut values ​​and displays them |
| pos | Manual | Sets the PWM channel to the specified value |

### List of all other shell commands
| Command | Mode | Explanation |
|----------|-------------|----------------------------------------|
| help | All | Lists all commands available in the current mode |
| repall | Expert, Manual | Reads all AnaIn, DigIn, Digout, and PwmOut and displays them cyclically |
| rst | Expert | Restarts the MCU |
| start | Expert | Command to start a task |
| stop | Expert | Command to stop (=terminate) a task |
| sys | All | Displays various operating system parameters |
| trace | Expert | Command to start and stop trace output |
| ver | All | Displays various version data |
| wdog | Expert | Command to block the program. Triggers the watchdog |
| x | All | Terminates a repeat command
| ENTER | All |Repeat the last command
