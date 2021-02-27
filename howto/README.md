## About debugging

This [executive summary](about.md) might be of interest, particularly if you are new to debugging.

## building Pico applications

For applications that use pico-sdk (pico-examples, etc. ... this is true of nearly all code), please follow the instructions in [building.md](building.md).

## debugging Pico applications with pico-debug

For OpenOCD users (including [Visual Studio Code](https://code.visualstudio.com/), etc.), after following the above steps on building Pico applications, please follow the instructions in [openocd.md](openocd.md).  For Visual Studio Code users, then also read [vscode.md](vscode.md).

Or, if you are a [Rowley Crossworks for ARM](https://www.rowley.co.uk/arm/index.htm) user, take a look at the pico-crossworks example in [pico-demos](https://github.com/majbthrd/pico-demos).

Or, if you are a [SEGGER Embedded Studio](https://www.segger.com/products/development-tools/embedded-studio/) user, take a look at the pico-ses example in [pico-demos](https://github.com/majbthrd/pico-demos).  Before debugging a target with SEGGER Embedded Studio, you will need to run openocd as shown in the [openocd.md](openocd.md) instructions.

