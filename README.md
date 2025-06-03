# Small Header Only Libraries
This repository contains several small header only libraries that contain 
functions that are useful but missing from the C standard library with an aim
towards being portable across a wide range of potential C platforms. All of 
these header libraries adhere to the ANSI C90 standard and where possible 
constrain themselves to the 'freestanding C' subset of the standard library.

These libraries came about as a result of the specific needs of the author and
as such are not as robust as other larger libraries offering similar solutions. 
If more functionality is desired one is advised to either expand these headers 
oneself or seek out those aforementioned alternatives. 

# Synopsis
## port* files
These files often only require the user to include them in a single C 
translation unit much like any other C header. 

eg:
    #include "portopt.h"

## macro* files
These files contain macros that expand into a dynamically generated API with a 
user defined namespace. The exact nature of this expansion is unique to the 
file and as such have their own manual pages in their respective directories.
In general however one usually will need to include the respective header file
and then define a macro describing the desired namespace and other pertinent 
information. 

eg:
    #include "macroThreadPool.h"
    #define MACRO_THREAD_POOL_COMPLETE(myPool, struct jobArgs, workerFunction);

Currently both macro dynamic headers have the option to define the resulting 
function prototype, their definitions, or both depending on the macro invoked. 
Please refer to the specific manual page for these files for more information.

# Summary
## portbit.h
A library created to attempt to address the platform dependant nature of 
bitfields in the C standard. The purpose of this library is to allow for the
creation of bitfields of arbitrary length and allow for the reading and writing
of variables of arbitrary length to said bitfields. Freestanding if an
alternative calloc function is provided. 

## portegg.h
A library for detecting host Endian-ness and converting between big and little
Endian formats. 

## portopt.h
A library that provides both a simple and more verbose command line argument
parsing framework. Freestanding compatible if logging is disabled. 

## portcfg.h
A library for parsing basic configuration files. Not freestanding as it 
requires I/O functions from stdio. 

## portint.h
A library intended to be a stopgap replacement for the basic types in stdint.h. 
This header attempts to ascertain variable bit widths from their respective 
limits in limits.h.

## macroThreadPool.h
A library to dynamically generate a thread pool that allows for the queuing of
a specified job. pthread.h or an applicable wrapper that provides the pthread 
API is required.

## cevMacro.h
A library to dynamically generate a dynamic array, or vector in C++ parlance, 
of a given data type. Can be made freestanding compatible if alternatives for
the allocation functions and memmove/memcpy are provided. See related manual
page for more information. 

# License
All the files in this repository are provided under the terms of the BSD 
4-Clause License. A copy of this license can be found in the body of each of 
the header files and also in LICENSE.txt. 

# Bugs
Please report any bugs along with pertinent environmental information to the 
bugs section of this repository. Feature suggestions or requests are not bugs. 

# Authors
Grauho <grauho@proton.me>

# Copyright 
Copyright (c) Grauho 2025, All Rights Reserved
