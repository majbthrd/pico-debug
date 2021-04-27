## Install pyOCD

You must already have python3 and pip installed prior to these instructions.  Be sure to also read the limitations section at the end of this document.

Option #1: install the latest stable package for your system

```
pip install -U pyocd
```

At least on my system at the time of writing, this installed pyOCD v0.30.3, which did not support writing RP2040 flash properly.  So, I would encourage trying the second option...

Option #2: install the latest prerelease code (suggested)

```
python3 -mpip install --pre -U git+https://github.com/pyocd/pyOCD.git
```

After the installation, confirm the pyOCD version with the following command:

```
pyocd -V
```

## Install GDB

```
$ sudo apt install gdb-multiarch
```

## Use GDB, pyOCD, and pico-debug to debug 'blinky'

If you have not already built blinky (or whichever app that you might choose to debug), some [sample instructions are here](building.md).

Obtain "pico-debug-gimmecache.uf2" from the Releases at:

https://github.com/majbthrd/pico-debug

Boot the RP2040 with the BOOTSEL button pressed, copy over pico-debug-gimmecache.uf2 to the RP2040, and it immediately reboots as a CMSIS-DAP adapter (ready for pyOCD to talk to it).

```
pyocd gdbserver -t rp2040_core0
```

Your output should look something like this:

```
0001147:INFO:board:Target type is rp2040_core0
0001205:INFO:dap:DP IDR = 0x0bc12477 (v2 MINDP rev0)
0001219:INFO:ap:AHB-AP#0 IDR = 0x04770031 (AHB-AP var3 rev0)
0001240:INFO:rom_table:AHB-AP#0 Class 0x1 ROM table #0 @ 0xe00ff000 (designer=43b part=4c0)
0001249:INFO:rom_table:[0]<e000e000:SCS-M0+ class=14 designer=43b part=008>
0001255:INFO:rom_table:[1]<e0001000:DWT-M0+ class=14 designer=43b part=00a>
0001261:INFO:rom_table:[2]<e0002000:BPU class=14 designer=43b part=00b>
0001265:INFO:cortex_m:CPU core #0 is Cortex-M0+ r0p1
0001270:INFO:dwt:2 hardware watchpoints
0001274:INFO:fpb:4 hardware breakpoints, 0 literal comparators
0001288:INFO:server:Semihost server started on port 4444 (core 0)
0001315:INFO:gdbserver:GDB server started on port 3333 (core 0)
0014740:INFO:gdbserver:Client connected to port 3333!
0014966:INFO:gdbserver:Attempting to load argon
0014966:INFO:gdbserver:Attempting to load freertos
0014967:INFO:gdbserver:Attempting to load rtx5
0014967:INFO:gdbserver:Attempting to load threadx
0014968:INFO:gdbserver:Attempting to load zephyr
```

This pyOCD terminal needs to be left open. So go ahead and open another terminal; in this one we’ll attach a gdb instance to pyOCD:

```
$ cd ~/pico/pico-examples/build/picoboard/blinky
$ gdb-multiarch picoboard_blinky.elf
```

Connect GDB to pyOCD:

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
(gdb) monitor reset
(gdb) continue
```

Or, if you want to set a breakpoint at main() before running the executable:

```
(gdb) monitor reset
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

## pyOCD limitations

With the "0.30.4.dev30" version of pyOCD that I tried, "monitor reset" does not stop execution after reset.  Moreover, pyOCD seems to defeat all breakpoints that were set before the "monitor reset" command.

The user has to "continue" before pyOCD even realizes that the target is already running, and then and only then can the user stop execution to set breakpoints **again**.  This does limit the utility of pyOCD at the time of writing.

