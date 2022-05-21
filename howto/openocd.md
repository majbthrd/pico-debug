## pre-built openocd

Pre-built openocd binaries compatible with pico-debug [can be found here](https://github.com/earlephilhower/pico-quick-toolchain/releases/) for several operating systems.

## Build openocd yourself (Linux specific)

Users can choose between the authentic OpenOCD or Raspberry Pi's own dated OpenOCD fork which their documentation continues to point to.

Here are the steps to build the authentic OpenOCD:

```
$ sudo apt install automake autoconf build-essential texinfo libtool libhidapi-dev libusb-1.0-0-dev
$ git clone git://git.code.sf.net/p/openocd/code openocd
$ cd openocd
$ ./bootstrap
$ ./configure --enable-cmsis-dap
$ make -j4
$ sudo make install
```

However, users that are beholden to Raspberry Pi (and its proprietary pico-probe) will find that they must continue using the RPi fork, and should freshly download the source code to something no older than 2021 Jun 18 and re-build it to ensure compatibility with pico-debug.

For Mac users building openocd, please also look at [the suggestions here](https://github.com/majbthrd/pico-debug/issues/5).

For OpenBSD users, please also look at [the suggestions here](https://github.com/majbthrd/pico-debug/discussions/23).

## Install GDB

```
$ sudo apt install gdb-multiarch
```

## Use GDB, OpenOCD, and pico-debug to debug 'blinky'

If you have not already built blinky (or whichever app that you might choose to debug), some [sample instructions are here](building.md).

Obtain "pico-debug-gimmecache.uf2" from the Releases at:

https://github.com/majbthrd/pico-debug

Boot the RP2040 with the BOOTSEL button pressed, copy over pico-debug-gimmecache.uf2 to the RP2040, and it immediately reboots as a CMSIS-DAP adapter (ready for OpenOCD to talk to it).

```
$ openocd -f board/pico-debug.cfg
```

Your output should look something like this:

```
Info : Hardware thread awareness created
Info : RP2040 Flash Bank Command
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : CMSIS-DAP: SWD  Supported
Info : CMSIS-DAP: FW Version = 2.0.0
Info : CMSIS-DAP: Interface Initialised (SWD)
Info : SWCLK/TCK = 0 SWDIO/TMS = 0 TDI = 0 TDO = 0 nTRST = 0 nRESET = 0
Info : CMSIS-DAP: Interface ready
Info : clock speed 4000 kHz
Info : SWD DPIDR 0x0bc12477
Info : SWD DLPIDR 0x00000001
Info : rp2040.core0: hardware has 4 breakpoints, 2 watchpoints
Info : starting gdb server for rp2040.core0 on 3333
Info : Listening on port 3333 for gdb connections
```

This OpenOCD terminal needs to be left open. So go ahead and open another terminal; in this one we’ll attach a gdb instance to OpenOCD:

```
$ cd ~/pico/pico-examples/build/picoboard/blinky
$ gdb-multiarch picoboard_blinky.elf
```

Connect GDB to OpenOCD:

```
(gdb) target remote localhost:3333
```

and load picoboard_blinky.elf into flash:

```
(gdb) load
Loading section .boot2, size 0x100 lma 0x10000000
Loading section .text, size 0x4828 lma 0x10000100
Loading section .rodata, size 0xd6c lma 0x10004928
Loading section .binary_info, size 0x1c lma 0x10005694
Loading section .data, size 0xa4c lma 0x100056b0
Start address 0x100001e8, load size 24828
Transfer rate: 8 KB/sec, 4138 bytes/write.
```

and then start it running:

```
(gdb) monitor reset init
(gdb) continue
```

Or, if you want to set a breakpoint at main() before running the executable:

```
(gdb) monitor reset init
(gdb) b main
(gdb) continue
Continuing.
Note: automatically using hardware breakpoints for read-only addresses.

Breakpoint 1, main () at ../main.c:48
48	  sys_init();
```

before continuing after you have hit the breakpoint:

```
(gdb) continue
```

if the target is running, it can be interrupted with Ctrl-C, at which point another gdb prompt should appear:

```
(gdb)
```

To quit from gdb, type:

```
(gdb) quit
```

