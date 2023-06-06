# Protocol implementations using SCL library

This repository contains sample implementations of MPC protocols using the SCL library.

## Setup

Obtain and install SCL. See the README in the SCL repository
[here](https://github.com/anderspkd/secure-computation-library). The code in
this repository requires the latest version 6.2.0 in order to run

## Building

Once SCL have been installed, navigate to any protocol sub-folder, and build:

```
$ cmake . -B build
```

After building the code, there should be an executable called `run.x` in the
directory.

```
$ cd build
build/$ ./run.x -help
```
