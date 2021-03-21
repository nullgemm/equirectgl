#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

curl -L "https://github.com/randy408/libspng/archive/refs/heads/master.zip" -o \
res/libspng.zip

cd res
unzip libspng.zip

rm libspng.zip
mv "libspng-master" libspng
