#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../../..

# generate example binaries
make -f makefile_example_windows_egl_native
