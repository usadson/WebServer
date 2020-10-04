# Building on FreeBSD
These are the build instructions for FreeBSD.

## Dependencies
Make sure you have the following packages installed:
 - [clang](https://clang.llvm.org/)
 - [git](https://git-scm.com/)
 - [openssl-devel](https://openssl.org/)
 - [make](https://www.gnu.org/software/make/)
 - [openssl](https://openssl.org/)

You can install the packages on FreeBSD with the following command:
```sh
sudo pkg install git openssl-devel
```

`clang`, `make` and `openssl` are installed by default on FreeBSD, so these are
omitted from the install list. `openssl-devel` is `libssl-dev` on other platforms.

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

