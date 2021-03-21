#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../gen

./gen_windows_mingw.sh
./gen_windows_mingw.sh

./gen_windows_msvc.sh
./gen_windows_msvc.sh
