#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

cp res/globox/lib/globox/windows/$1 bin/globox.dll
