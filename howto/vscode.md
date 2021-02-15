## pico-debug debugging with Visual Studio Code

There are a few [Visual Studio Code](https://code.visualstudio.com/) (aka VS Code) examples online for the RP2040 so far.  So far, they all assume picoprobe.

If you are already set on following one of these, adopt the changes below.  If you are starting fresh, [please try these instructions](vscode1.md).

## adapting existing instructions to switch from picoprobe to pico-debug

To adapt instructions from picoprobe to pico-debug, first make sure that you followed the earlier build steps and have downloaded and installed the newer pico-sdk and openocd in the howto instructions.  This is important.  The newer pico-sdk is needed to fix problems pico-sdk has with single device debugging, and the newer openocd is needed to support single device debugging.

For existing Visual Studio Code examples written for picoprobe, in the launch.js file, replace this picoprobe configuration:

```
             "configFiles": [
                 "/interface/picoprobe.cfg",
                 "/target/rp2040.cfg"
             ],
```

with

```
             "configFiles": [
                 "interface/cmsis-dap.cfg",
                 "target/rp2040-core0.cfg"
             ],
             "openOCDLaunchCommands": [
                 "transport select swd",
                 "adapter speed 4000"
             ],
```

