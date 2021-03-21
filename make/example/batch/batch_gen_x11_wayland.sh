#!/bin/bash

# get into the right folder
cd "$(dirname "$0")"
cd ../gen

# if we have "1" as an argument we link globox dynamically
if [ -n "$1" ] && [ "$1" -eq 1 ] ; then
link=2
else
link=1
fi

echo -e "$link\n" | ./gen_x11.sh
echo -e "$link\n" | ./gen_wayland.sh
