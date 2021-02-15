## one way to use Visual Studio Code with pico-debug

Before following the steps below, follow [these pico-sdk and pico-examples instructions](building.md).  This gives you all the build tools that are known compatible with pico-debug.

Download and install [Visual Studio Code](https://code.visualstudio.com/).

Run Visual Studio Code, and select the pull-down menu View -> Extensions (or click the Tetris block looking icon on the far left).  Search for and click "Install" for these modules:

```
C/C++ Extension Pack (by Microsoft)
Cortex-Debug (by marus25)
CMake Tools (by Microsoft)
```

For the (pico-examples) project that you want to build, add a .vscode subdirectory.  So, for the picoboard blinky example in pico-examples:

```
mkdir ~/pico/pico-examples/build/picoboard/blinky/.vscode
```

Create a text file named launch.json in the newly-created directory, and paste in the following text:

```
{
     "version": "0.2.0",
     "configurations": [
         {   "name": "Pico Debug",
             "device": "RP2040",
             "gdbPath": "arm-none-eabi-gdb",
             "cwd": "${workspaceRoot}",
             "executable": "${workspaceRoot}/picoboard_blinky.elf",
             "request": "launch",
             "type": "cortex-debug",
             "servertype": "openocd",
             "configFiles": [
                 "interface/cmsis-dap.cfg",
                 "target/rp2040-core0.cfg"
             ],
             "openOCDLaunchCommands": [
                 "transport select swd",
                 "adapter speed 4000"
             ],
             "svdFile": "${env:PICO_SDK_PATH}/src/rp2040/hardware_regs/rp2040.svd",
             "runToMain": true,
             "postRestartCommands": [
                 "break main",
                 "continue"
             ]
         }
     ]
 }
```

IMPORTANT: for the "executable" setting, you will need to customize "picoboard_blinky.elf" to suit the filenames of the project.

In Visual Studio Code, select the pull-down menu File -> Open Folder and choose the project folder than you are working on.  For the picoboard blinky example, that would be navigating to ~/pico/pico-examples/build/picoboard/blinky and clicking the OK button.

At this point, Visual Studio Code shows an easy to miss drop-down box at the top of the window to select a kit (that will disappear if you don’t immediately select something).  If you miss it (which seems easy to do), repeat the File -> Open Folder step above.  The "kit" that you need to select is "GCC for arm-none-eabi", which indicates the compiler used to compile the RP2040 Pico code.

You can confirm the correct compiler has been selected by looking at the blue ribbon at the bottom of the window and looking for a wrench/spanner icon.  The text to the right of the wrench/spanner should say "GCC for arm-none-eabi" followed by a version number.

If you haven’t already done so, boot the Pico with the BOOTSEL button pressed, and copy over to the RP2040 the [pico-debug-gimmecache.uf2](https://github.com/majbthrd/pico-debug) file.

In Visual Studio Code, select the pull-down menu Run -> Start Debugging (or click the play button with a bug on the far left).

At this point, the ribbon at the bottom of the window turns orange and program execution at main() should be ready to begin.  You are debugging at this point!

pico-debug should keep running until you unplug the Pico, so you should be free to debug your code, make changes, recompile, and then run Visual Studio Code again to debug without needing to reload the pico-debug-gimmecache.uf2 file.

