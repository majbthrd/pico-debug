## Description

RP2040 has two ARM Cortex-M0+ cores, and the second core normally remains dormant.

pico-debug runs on one core in a RP2040 and provides a USB CMSIS-DAP interface to debug the other core.  No hardware is added; it is as if there were a virtual debug pod built-in.

Boot the RP2040 with the BOOTSEL button pressed, copy over pico-debug.uf2, and it immediately reboots as a CMSIS-DAP adapter.  pico-debug loads as a RAM only .uf2 image, meaning that it is never written to flash and doesn't replace existing user code.

To cater to different user situations, there are two versions of pico-debug: **MAXRAM** and **GIMMECACHE**

Most users will prefer to use the **GIMMECACHE** version.

With **pico-debug-maxram**, *all* 264kBytes of SRAM on the RP2040 is available for running user code; pico-debug shoehorns itself entirely into the 16kBytes of XIP_SRAM (aka flash cache).

With **pico-debug-gimmecache**, 248kBytes (94% of total) of SRAM is available for running user code; pico-debug gives plenty of elbow room by occupying only 6% near the very top of SRAM, and unlike MAXRAM, leaves the flash cache operational.

If viewing this on github, pre-built binaries are available for download on the right under "Releases".

## How to use

Please read [howto/README.md](howto/README.md) for instructions on how to start using pico-debug.

## Caveats whilst using pico-debug

The executive summary is:

pico-debug uses the USB port to provide debugging to the user, so the user's app can't be simultaneously using the USB port! :)

The specifics are:

- MAXRAM only: the flash cache cannot be used by the user code, as pico-debug is using this memory
- GIMMECACHE only: SRAM 0x2003C000 to 0x2003FFFF must not be used by user code
- user code cannot reconfigure the PLL_USB, as the USB peripheral needs this
- the USB peripheral is used to provide the debugger, so the user code cannot use it as well

## License

TinyUSB and code specific to pico-debug is licensed under the [MIT license](https://opensource.org/licenses/MIT).

ARM's CMSIS_5 code is licensed under the [Apache 2.0 license](https://opensource.org/licenses/Apache-2.0).

