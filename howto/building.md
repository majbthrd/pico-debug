## Download the Pico SDK and examples

Create a subdirectory to hold the files that you will download:

```
$ cd ~/
$ mkdir pico
$ cd pico
```

Clone the pico-sdk and pico-examples git repositories:

*NOTE: a substitute for pico-sdk is presently utilized, since pico-sdk currently lacks important features.*

```
$ git clone https://github.com/majbthrd/pico-sdk.git --branch pll_init
$ cd pico-sdk
$ git submodule update --init
$ cd ..
$ git clone -b master https://github.com/raspberrypi/pico-examples.git
```

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

