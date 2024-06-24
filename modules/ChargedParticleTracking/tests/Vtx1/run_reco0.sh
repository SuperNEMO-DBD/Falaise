#!/usr/bin/env bash

if [ ! -L PTD_link.brio ]; then
    echo >&2 "[error] No link 'PTD_link.brio' exists!"
    exit 1
fi

flreconstruct \
    --verbosity "debug" \
    -p "pipeline0.conf" \
    -i "PTD_link.brio" \
    -o "TTD.brio" \
    > flrec0.log 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] flreconstruct failed!"
    exit 1
fi

# flvisualize \
#     --variant-profile "profile.conf" \
#     -i "test.brio"

exit 0

