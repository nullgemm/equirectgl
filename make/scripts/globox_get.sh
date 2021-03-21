#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

# download target eglproxy release
version="v0.2.5"

curl -L "https://github.com/nullgemm/globox/releases/download/$version/globox_bin_$version.zip" -o \
res/globox.zip

cd res
unzip globox.zip

rm globox.zip
mv "globox_bin_$version" globox
