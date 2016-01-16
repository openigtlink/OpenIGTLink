The OpenIGTLink Library
=======================

[![Build Status](https://travis-ci.org/openigtlink/OpenIGTLink.svg?branch=master)](https://travis-ci.org/openigtlink/OpenIGTLink)

This project provides a C/C++ implementation of the OpenIGTLink protocol defined in [OpenIGTLink Web Page](http://openigtlink.org/). 

 
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

## Sun OS

For Sun OS (Solaris), you may follow the instruction for Linux/Mac OS X described above. If you want to explicitly specify Sun CC instead of GNU CC, options for cmake look like:

    cmake -DCMAKE_C_COMPILER:STRING=/opt/SUNWspro/bin/cc -DCMAKE_CXX_COMPILER:STRING=/opt/SUNWspro/bin/CC -DBUILD_EXAMPLES:BOOL=ON ../OpenIGTLink

## Windows
* Download Git Windows client, if you don't have one already
* You will also need [CMake](http://www.cmake.org/HTML/Download.html) and a C/C++ compiler as [Microsoft Visual C++](http://www.microsoft.com/express/vc/)
* Download the source code from Git repository.
  * URL of repository: git://github.com/openigtlink/OpenIGTLink.git
  * Click OK
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

## Note on Windows
If you are planning to link the OpenIGTLink Library with IGSTK, or the 3D Slicer OpenIGTLinkIF module, please make sure that you use the Windows version of CMake (not the one contained in the Cygwin environment) and Visual Studio 2008. Since IGSTK and 3D Slicer are compiled with Visual Studio, the OpenIGTLink library built in cygwin environment using GCC cannot be linked with those software.

License
-------
The code is distributed as open source under [the new BSD liccense](http://www.opensource.org/licenses/bsd-license.php).



