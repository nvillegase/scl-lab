# Minimal example of a protocol using SCL

This repository contains a simple protocol example implemented using SCL.

## Setup

Obtain and install SCL. See the README in the SCL repository
[here](https://github.com/anderspkd/secure-computation-library). The code in
this repository requires the latest version 6.2.0 in order to run

## Building

Once SCL have been installed, it should be straightforward to build the code in
this repo.

```
$ cmake . -B build
```

After building the code, there should be an executable called `run.x` in the
directory.

```
$ cd build
build/$ ./run.x -help

Usage: ./run.x -id int -conf string [options ...]

Required arguments
 -id 'int'          ID of this party. 
 -conf 'string'     network config. 

build/$
```

An example of a network config is provided. See `3_parties.txt`. To run the
protocol, provided the required arguments listed above. For example,

```
build/$ ./run.x -id 0 -conf ../3_parties.txt
```

to run party 0.
