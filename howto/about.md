## What is a debugger?

Many ARM chips (including the RP2040 on the Raspberry Pi Pico) include debugging capability as part of their hardware.  Since it is part of the chip, it is often known as OCD (On-Chip Debug).

This debugging capability is substantially more that mere downloading.  It makes it possible to single step through code, watch variable values, and set breakpoints (tell the CPU to stop and wait when it reaches prescribed points in the code).

Normally, access to this debug functionality is achieved via 2 or 5 hardware pins (SWD or JTAG) known in ARM-speak as a DAP (Debug Access Port).  These signals must be wired up to external hardware.

A "Debug Unit" in ARM-speak (often referred to as "debug pod") is the device that is connected between the Host PC and these hardware pins on the target device.  The "Debug Unit" makes it possible for PC software running on the Host to download, single step code, watch variables, and set breakpoints on the ARM target.  This "Debug Unit" is normally additional hardware that must be purchased.

![PC-debugunit-target](https://user-images.githubusercontent.com/12226419/109394274-cf918400-78eb-11eb-8ca4-764ce2c557da.png)

With pico-debug, however, the "Debug Unit" is virtual.  It runs directly on the very same RP2040 that is being debugged, and it uses the existing USB connection.

Although the PC to Debug Unit connection is almost universally USB these days, it is important to know that there are different protocols over that USB connection, and these protocols depend on which Debug Unit you use.

Many Debug Units have proprietary protocols that are specific to that particular Debug Unit.  Given their proprietary nature, host software has to be specially written for each of these.  This limits which software can be used for debugging.

However, pico-debug uses CMSIS-DAP, which is an open protocol written by ARM.  CMSIS-DAP can work without drivers on any OS.  Anyone can make their own CMSIS-DAP Debug Unit, and any host PC software written to work with CMSIS-DAP can work with any CMSIS-DAP Debug Unit.  Since the protocol has existed since 2013, there is already support from many software tools.

## What runs on the PC?

There are three main activities on the PC:
    1) compiler: compiling/building code
    2) debugger: debugging code
    3) remote server: code to bridge the debugger to the debug unit

Some software solutions are all-in-one and do all three of these tasks within one software tool:

| All-in-one |
| :--------- |
| [Rowley Crossworks for ARM](https://www.rowley.co.uk/arm/index.htm) |
| [Keil MDK](https://www.arm.com/products/development-tools/embedded-and-software/keil-mdk) / [Keil MDK-Lite](https://www2.keil.com/mdk5/editions/lite) |

Other approaches rely on three different pieces of PC software to work together.  You can mix and match which software does each of the three tasks:

| compiler/build |
| :------------- |
| GCC (C) |
| MicroPython (Python) |

| debugger |
| :------- |
| [GDB](https://www.gnu.org/software/gdb/) |
| [Visual Studio Code](https://code.visualstudio.com/) |
| [SEGGER Embedded Studio](https://www.segger.com/products/development-tools/embedded-studio/) |
| [Eclipse Embedded CDT](https://projects.eclipse.org/projects/iot.embed-cdt) |

| remote server |
| :------------ |
| OpenOCD |
| pyOCD |

The compiler produces an .elf file, which can then be used for debugging.  It includes both the program and debugging information.  The .elf file tells the debugging software what the names of all the variables and functions are, and it links back to the source code files to tell the debugging software which portions of ARM code correspond to which lines of source code.

When the software is divided into three tasks as above, the protocol between 'debugging' and 'debug bridge' is called "GDB" (not by coincidence the same as the name of one of the debugger options).

