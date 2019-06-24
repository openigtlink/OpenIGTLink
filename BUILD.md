The OpenIGTLink Library Build Instruction
=========================================

Linux / Mac OS X
----------------

First, obtain the source code from the repository using Git. To simply download
the code, run the following command from a terminal:

~~~~
$ git clone https://github.com/openigtlink/OpenIGTLink.git
~~~~

Then configure using CMake. The library requires CMake version higher than 2.4.

~~~~
$ mkdir OpenIGTLink-build
$ cd OpenIGTLink-build
$ cmake -DBUILD_EXAMPLES:BOOL=ON ../OpenIGTLink
$ make
~~~~

You may install the library into your disk (optional). The default target
directory is /usr/local, but you can configure it from the CMake configuration
screen. To install the files, run

~~~~
$ make install
~~~~

You might need super user access.

Windows
-------
* Download the source code from Git repository.
* URL of repository: git://github.com/openigtlink/OpenIGTLink.git
* Run CMake
* Where is the source code: C:\Devel\OpenIGT\OpenIGTLink
* Where to build the binaries: C:\Devel\OpenIGT\OpenIGTLink-build
* Click "Configure" and select your compiler (usually just click "OK")
* Message: "Build directory does not exit, should I create it?" - click "OK"
* Click "Configure"
* Click "OK" to close CMake
* Start Visual C and compile the project (C:\Devel\OpenIGT\OpenIGTLink-build\OpenIGTLink.sln)
If all went OK you will have the executable and the library:
* C:\Devel\OpenIGT\OpenIGTLink-build\bin\debug\igtlSocketTest.exe
* C:\Devel\OpenIGT\OpenIGTLink-build\bin\debug\OpenIGTLink.lib

VideoStreaming
---------------
You might want to use OpenIGTLink library to perform video streaming. Currently OpenH264, H265, VP9 and AV1 are supported in the OpenIGTLink.

* Prerequisites

In the case of H264 build, NASM is needed for assembly code: workable version 2.10.06 or above for 32 bit, nasm can downloaded from http://www.nasm.us/ 
For Mac OSX 64-bit NASM needed to be below version 2.11.08 as nasm 2.11.08 
will introduce error when using RIP-relative addresses in Mac OSX 64-bit. If you have another NASM version installed, try to unlink the wrong version of NASM, 
and link to the version to below 2.11.08. Afterwards, check the nasm version with comman "which nasm" and "nasm -v".

In the case of VP9 build, The Yasm assembler is required to build optimized libraries for x86 platforms. 
Version 0.8.0 or newer is required, except Version 1.0.0.

In the case of H265 build, H265 have many implementations, the encoder used in library is x265 and the decoder is OpenHEVC.
OpenIGTLink library doesn't build H265 libraries, so the users need to download and compile the libraries by themselves. 
Afterwards, set the variables-"X265_INCLUDE_DIR, X265_LIBRARY_DIR, OPENHEVC_INCLUDE_DIR, OpenHEVC_LIBRARY"-correctly in cmake configuration.

In the case of AV1 build, the AV1 codec is cmakefied. Once the OpenIGTLink_USE_AV1 option is selected, the openigtlink libray will superbuild the codec and link automatically.
* Linux / Mac OS X

In the case of Linux and Mac, after installing the required program in the Prerequisites section,
the build of openigtlink with codec is very straightforward: 

~~~~
$ cmake -DBUILD_VIDEOSTREAM:BOOL=ON -DLINK_H264:BOOL=ON -DOpenIGTLink_PROTOCOL_VERSION_3:BOOL=ON ../OpenIGTLink
$ make
~~~~

or for VP9 configuration:

~~~~
$ cmake -DBUILD_VIDEOSTREAM:BOOL=ON -DOpenIGTLink_USE_VP9:BOOL=ON -DOpenIGTLink_PROTOCOL_VERSION_3:BOOL=ON ../OpenIGTLink
$ make
~~~~

* Windows Build

In the case of windows build, please refer to the following websites for H264, X265 and VP9 respectively.
Useful H264 build instructions:

https://github.com/cisco/openh264

Useful VP9 build instructions:

https://www.webmproject.org/code/build-prerequisites/ 
http://wiki.webmproject.org/ffmpeg/building-with-libvpx

OpenIGTLink provides binary library files for visual studio 12 2013 and visual studio 14 2015.
The libray will be automatically downloaded during the project build when user configure OpenIGTLink library using these cmake generators:
"Visual Studio 12 2013", "Visual Studio 12 2013 Win64", "Visual Studio 14 2015" and "Visual Studio 14 2015 Win64". 
For the rest cmake generators, the user need to provide the VP9_INCLUDE_DIR and VP9_BINARY_DIR, otherwize the video streaming feature will be deactivated.

Useful X265 build intructions:

https://bitbucket.org/multicoreware/x265/wiki/CrossCompile

Useful OpenHEVC build intructions:

http://openhevc.github.io/openHEVC/

WebSocket
---------------
You might want to use OpenIGTLink library to perform websocket communication. 

* Prerequisites
This functionality is only tested for Mac OSX platform and Windows platform (VS 2015, boost version 1.62). The websocketpp project needs to be build and installed first, below is a build example for Mac OSX platform, Windows build uses CMAKE GUI for configuration. 
~~~~
$ git clone https://github.com/zaphoyd/websocketpp
$ mkdir websocketpp-build
$ cd websocketpp-build
$ cmake -DCMAKE_INSTALL_PREFIX=${path-to-your-installation} ../websocketpp
$ make & make install
~~~~

Then go to the build directory of the OpenIGTLink, run these commands:

~~~~
$ cmake -DOpenIGTLink_USE_WEBSOCKET=ON -DBUILD_EXAMPLES=ON -Dwebsocketpp_DIR:PATH=${path-to-your-installation}/lib/cmake/websocketpp ${your-openigtlink_source}
$ make
$ ./bin/WebSocketServer ${your-openigtlink_source}/Examples/WebSocket 9002
~~~~

Open your browser and type: localhost:9002, there will be a connect button on the page, simply click it and you will see updating tracking data.

Other Platforms
---------------

Please see [Build Instruction](http://openigtlink.org/library/build.html).
