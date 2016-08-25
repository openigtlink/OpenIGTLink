The OpenIGTLink Library
=======================

* Linux/Mac build: [![Build Status](https://travis-ci.org/openigtlink/OpenIGTLink.svg?branch=master)](https://travis-ci.org/openigtlink/OpenIGTLink)
* Windows build: [![Build status](https://ci.appveyor.com/api/projects/status/beo8cej2nxu55ex0?svg=true)](https://ci.appveyor.com/project/openigtlink/openigtlink)
* This project provides a C/C++ implementation of the OpenIGTLink protocol defined in [OpenIGTLink Web Page](http://openigtlink.org/). 

Build Instruction
-----------------

## Linux / Mac OS X
First, obtain the source code from the repository using Git. To simply download the code, run the following command from a terminal:

$ git clone https://github.com/openigtlink/OpenIGTLink.git

Then configure using CMake. The library requires CMake version higher than 2.4.

$ mkdir OpenIGTLink-build
$ cd OpenIGTLink-build
$ cmake -DBUILD_EXAMPLES:BOOL=ON ../OpenIGTLink
$ make

You may install the library into your disk (optional). The default target directory is /usr/local, but you can configure it from the CMake configuration screen. To install the files, run

$ make install

You might need super user access.

## Windows
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

## Other Platforms

Please refer [the build instruction page](http://openigtlink.org/library/build.html).

License
-------
The code is distributed as open source under [the new BSD liccense](http://www.opensource.org/licenses/bsd-license.php).


