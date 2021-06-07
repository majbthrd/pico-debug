## Pico SDK

If your local copy of Pico SDK was downloaded prior to 3 Jun 2021, you will need to reinstall it.  Versions prior to this are incompatible with pico-debug.

Pico SDK is available here: https://github.com/raspberrypi/pico-sdk

## Install the Toolchain

```
$ sudo apt update
$ sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
```

## Invoking cmake

From the pico directory we created earlier, cd into pico-examples and create a build directory:

```
$ cd ~/pico/pico-examples
$ mkdir build
$ cd build
```

Set PICO_SDK_PATH to point to the previously created pico-sdk directory:

```
$ export PICO_SDK_PATH=../../pico-sdk
```

Invoke cmake to create the files needed for building debug versions of examples:

```
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
Using PICO_SDK_PATH from environment ('../../pico-sdk')
PICO_SDK_PATH is ...
   .
   .
   .
-- Build files have been written to: ...
```

## Building of "picoboard_blinky"

```
$ cd ~/pico/pico-examples/build/picoboard/blinky
$ make -j4
```

At this point, you have picoboard_blinky.uf2 as well as all the files to optionally debug with openocd, gdb, and pico-debug.

