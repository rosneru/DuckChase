# Duckchase - Experiments with Amiga game development
Trying to create a simple game for the Amiga with *C++*. Main purpose is
rather to learn about the AmigaOS and its internals than creating a
game, 'though.

## C++-game in *src* directory
The Duckchase C++ game in directory *src* uses RAII and for that
exceptions are enabled and used heavily. While this should (in gcc 6.5)
have [no run time
overhead](http://eab.abime.net/showthread.php?p=1393192#post1394112) it
has two disadvantages to be noticed:

- Enabling exceptions and using RAII blew the binary size from 50k to 150k.

- Binary needs a full 32 bit processor to run. **Not working on 68000
  anymore**.

## The other src directories

Apart of the main src directory there are the two other directories
*src-experiments-asm* and *src-experiments-c* with some tests in
Assembly and C99 (vbcc).


# Build

The example can be build on Linux with *cmake* and [Bebbos gcc 6.5
toolchain](https://github.com/bebbo/amiga-gcc) or on Amiga with 
*StormC4*.

## Build with Linux
### Dependencies
The project is created with Debian buster on Windows 10 WSL with the 
following packages installed:

 - build-essentials
 - cmake
 - git
 - [ Bebbos gcc 6.5 toolchain](https://github.com/bebbo/amiga-gcc) 
which is expected to be installed in /opt


### Build
To build this project a Makefile must be created with cmake:

- Manually create a directory *build* next to the *src* directory

- Enter this directory and type.
<!-- -->
    cmake -DCMAKE_BUILD_TYPE=Release ..


- For preparing a debug Makefile with debug information, type:
<!-- -->
    cmake -DCMAKE_BUILD_TYPE=Debug ..

Then cmake is configured. Build can be started by simply typing 
    
    make 

from inside the build directory.

**NOTE:** Apart of the main binary *DuckChase* some other tools are also
build in this step.

After the build was successful the binaries are copied into the project
root directory.

## VSCode integration
The VSCode default build task (see tasks.json) has been updated to use
the *cmake* generated Makefile inside the *build* directory.

After cmake is prepared for debug or release build (see above)

From within VSCode the Build can be started with *Ctrl + Shift + b*.

## Build with Amiga

There's a project file included for StormC++4, so the project also can
be build on the Amiga.

An advantage of using StormC4 is that it has an working source level
debugger with variable display etc..might be handy sometimes.

### Dependencies

- [StormC4](https://www.amiga-shop.net/en/Amiga-Software/Amiga-Tools/StormC-v4::145.html)
- [STL implementation for StormC4](http://aminet.net/package/dev/c/amigastlport)

### Build and run

To build with StormC4 click on the button *Open project* and select the
file:

    src/DonkeyKong.¶

Press *F8* to start building.

After the build was successful click on *Run* or hit *F9* to start the
game.
