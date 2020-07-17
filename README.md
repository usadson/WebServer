# WebServer
![C/C++ CI](https://github.com/usadson/WebServer/workflows/C/C++%20CI/badge.svg)

A C++ WebServer.

## What is it?
Wizard Web Server is the official name. This is the repository for a C++ web
server.

## What is it not?
A project for anyone to use. This may sound selfish, but (atm) I only intend to
use this server myself. If you need a web server, go download nginx or something
:-)

## Basic Requirements
For this project, you need to have the OpenSSL library installed. Make that you
have openssl, but also libssl/openssl-dev, or something alike installed. This
project utilizes the clang/LLVM compiler infrastructure. Also, this project is
only for POSIX a.ka. UNIX-like operating systems. No Windows support, since
nobody runs servers on Linux, and WSL exists. Most systems have `pkg-config`
installed by default, but if you run into problems mentioning this utility,
make sure it is installed.

## Building
`make`. I haven't found a modern, well-thought-through building system, so I'll
stick with it for now. It should work out of the box.

## Running
As the project is currently in development stages, no system(ctl/d/etc.) support
atm. Simply run the executable with `./server`. You may need to `chmod +x server`
it.

## Licensing
This project is [licensed](COPYING) under a 2 Clause BSD-like license.
