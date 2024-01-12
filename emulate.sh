#!/bin/bash

if [ -n "$1" ]; then
    echo -n "fff $1" | qemu-system-arm -M lm3s811evb -serial stdio -kernel gcc/RTOSApp.axf
else
    qemu-system-arm -M lm3s811evb -serial stdio -kernel gcc/RTOSApp.axf
fi