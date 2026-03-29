# Bilateral-Teleoperation-Wireless-Communication
Repo for J&amp;J Competition primarily for communication aspects between arms.
---------------------
## Before running code you need the following dependencies:
1. CMake
2. mjbots/moteus C++ library
---------------------
## Something to note is that you need to find USB devices like so 
```
#for Linux/WSL:
lsusb

#for MacOS:
ls /dev/tty*
```
---------------------
## Instructions to run client/server arms:
```
mkdir [client | server]/build && cd [client | server]/build
cmake ..
make
./[client | server] --force-transport fdcanusb --fdcanusb <route-to-your-usb-dev> 
```
