## About debugging

This [executive summary](about.md) might be of interest, particularly if you are new to debugging.

## building Pico applications

For applications that use pico-sdk (pico-examples, etc. ... this is true of nearly all code), please follow the instructions in [building.md](building.md).

## debugging Pico applications with pico-debug using the GDB protocol

You have a choice between this and "debugging Pico applications with pico-debug without using the GDB protocol" further below.

First make certain that you have followed the steps above in "building Pico applications".

For OpenOCD users, please follow the instructions in [openocd.md](openocd.md).

For pyOCD users, please follow the instructions in [pyocd.md](pyocd.md).

Then proceed to "debuggers that use GDB" immediately below.

## debuggers that use GDB

First make certain that you have your GDB implementation of choice running (OpenOCD, pyOCD, etc.).

For [Visual Studio Code](https://code.visualstudio.com/) users, then also read [vscode.md](vscode.md).

For [SEGGER Embedded Studio](https://www.segger.com/products/development-tools/embedded-studio/) users, take a look at the pico-ses example in [pico-demos](https://github.com/majbthrd/pico-demos).

## debugging Pico applications with pico-debug without using the GDB protocol

You have a choice between this and "debugging Pico applications with pico-debug using the GDB protocol" further above.

First make certain that you have followed the steps above in "building Pico applications".

If you are a [Rowley Crossworks for ARM](https://www.rowley.co.uk/arm/index.htm) user, v4.8.3 and higher is already capable of debugging RP2040 targets (including ones using pico-debug).

If you use [Keil MDK](https://www.arm.com/products/development-tools/embedded-and-software/keil-mdk) or [Keil MDK-Lite](https://www2.keil.com/mdk5/editions/lite), take a look at [Pico_Template](https://github.com/GorgonMeducer/Pico_Template).

