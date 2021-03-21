#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../..

cp .gitea .gitmodules
git submodule sync
git submodule update --init --remote
git submodule update --init --recursive --remote
