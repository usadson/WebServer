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

## Building
`make`. I haven't found a modern, well-thought-through building system, so I'll
stick with it for now. It should work out of the box.

## Running
As the project is currently in development stages, no system(ctl/d/etc.) support
atm. Simply run the executable with `./server`. You may need to `chmod +x server`
it.

To stop the server, you can force the server to stop immediately by hitting
control + c, but a better thing to do is to shutdown the server gracefully by
typing something and then hitting enter (making a non-blank newline). This
should trigger shutdown. If it doesn't work, try the control + c method.

## Licensing
This project is [licensed](COPYING) under a 2-Clause BSD-like license, which
*roughly* means that you are free to use it as you wish, as long as you credit
me.
