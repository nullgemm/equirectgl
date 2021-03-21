#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

cp res/globox/lib/globox/macos/lib$1.dylib bin/libglobox.dylib
