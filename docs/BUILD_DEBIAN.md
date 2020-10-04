# Building on Ubuntu/Debian or a derivative that
These are the build instructions for Debian, Kubuntu, Lubuntu, Tails, Ubuntu,
Xubuntu, et cetera. Just make sure your system uses APT as its package manager.

## Dependencies
Make sure you have the following packages installed:
 - [clang](https://clang.llvm.org/)
 - [git](https://git-scm.com/)
 - [libssl-dev](https://openssl.org/)
 - [make](https://www.gnu.org/software/make/)
 - [openssl](https://openssl.org/)

You can install the packages on Ubuntu with the following command:
```sh
sudo apt install clang git libssl-dev make openssl
```

## Getting the Code
Getting the code is as easy as:
```sh
git clone https://github.com/usadson/WebServer.git && cd WebServer
```
or using SSH:
```sh
git clone git@github.com:usadson/WebServer.git && cd WebServer
```

## Building
Building is done via CMake:
```sh
mkdir build
cd build
cmake ..
cmake --build .

# To start the server:
./server
```

## End
And that's all! Click [here](../README.md) to go back to the README document.

