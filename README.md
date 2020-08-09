# WebServer
![C/C++ CI](https://github.com/usadson/WebServer/workflows/C/C++%20CI/badge.svg)

A C++ WebServer.

## What is it?
Wizard Web Server is the official name. This is the repository for a C++ web
server.

## What is it not?
A project for anyone to use. This may sound selfish, but (atm) I only intend to
use this server myself. This means that I won't guarantee API/configuration
stability.

This doens't mean that it isn't usable for anyone else. I have tried to make
code as clear as possible, by using proper names and documenting complex
behavior. If you are interested in it, look in [main.cpp](main.cpp) for
configuring your own server.

If you need a web server, it's probably better to download nginx or something
alike :-)

## Basic Requirements
For this project, you need to have the OpenSSL library installed. Make that you
have openssl, but also libssl/openssl-dev, or something alike installed. This
project utilizes the clang/LLVM compiler infrastructure. Also, this project is
only for POSIX a.ka. UNIX-like operating systems. No Windows support, since
nobody runs servers on Windows and WSL exists. Most systems have `pkg-config`
installed by default, but if you run into problems mentioning this utility,
make sure it is installed.

## Build Instructions
The following sections are about building the software. There are specific
building instructions to ease the installation. For Debian, Ubuntu or a
derivative, click [here](docs/BUILD_DEBIAN.md). For FreeBSD, click
[here](docs/BUILD_FREEBSD.md). For any other system, make sure you have the
right packages installed, but not that the package names can differ per
distribution of your operating system.

## Dependencies
Make sure you have the following packages installed:
 - [clang](https://clang.llvm.org/)
 - [git](https://git-scm.com/)
 - [libssl-dev](https://openssl.org/)
 - [make](https://www.gnu.org/software/make/)
 - [openssl](https://openssl.org/)
 - [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)

## Building
`make`. I haven't found a modern, well-thought-through and easy-to-use building
system, so I'll stick with Makefiles for now. It should work out of the box.

## Running
As the project is currently in development stages, no system(ctl/d/etc.) support
at the moment. Simply run the executable with `./server`.

To stop the server, you can force the server to stop immediately by hitting
control + c, but a better thing to do is to shutdown the server gracefully by
typing something and then hitting enter (making a non-blank newline). This
should trigger shutdown. If it doesn't work, try the control + c method.

## Configuring
Configuring this server is done at compile-time, except for the certificates and
private keys used by TLS. This compile-time configuration is done for security,
performance and personal preference reasons. The configuration can be found in
code and is well documented.

See:
 * [http/configuration.hpp](http/configuration.hpp)
 * [security/policies.hpp](security/policies.hpp)
 * [security/tls_configuration.hpp](security/tls_configuration.hpp)
 * [main.cpp](main.cpp)

## Licensing
This project is [licensed](COPYING) under a 2-Clause BSD-like license, which
*roughly* means that you are free to use it as you wish, as long as you credit
the authors of this project and possibly the authors of the dependencies used
with this software.
