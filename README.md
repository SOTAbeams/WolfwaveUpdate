# WolfwaveUpdate

GUI for updating the firmware of a [WOLFWAVE](https://www.sotabeams.co.uk/wolfwave-advanced-audio-processor/).

License: GPLv2

## Running

Instructions and compiled executables can be found at <http://www.wolfwave.co.uk/firmware> 

### Windows

On Windows, drivers need to be installed before this program will work. See <http://www.wolfwave.co.uk/firmware> for instructions.

The config program as published on wolfwave.co.uk is statically linked, so should not need any extra DLLs.

### Linux

No drivers should be needed.

As an alternative to this GUI, you can also update the firmware from the command line using [dfu-util](http://dfu-util.sourceforge.net/):

```
dfu-util -a 0 -d 0483:df11 -s :leave -D wolfwave-yyyymmdd.dfu
```

## Building

### Dependencies

* cmake
* g++
* git-core
* Qt
* libusb

### Getting the source code

    git clone https://github.com/SOTAbeams/WolfwaveUpdate.git
    cd WolfwaveUpdate
    git submodule update --init

### Linux

    mkdir build
    cd build
    cmake ..
    make

Remember to run `git submodule update --init` first.
 
### Windows

This program uses some modern C++ features. This means it might or might not successfully compile in Visual Studio.


### Libraries used

* [libFirmwareUpdate++](https://github.com/SOTAbeams/libFirmwareUpdatepp) - GNU GPLv2

