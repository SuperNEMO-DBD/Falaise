#!/usr/bin/env bash

flreconstruct \
    --verbosity "debug" \
    -p "pipeline1.conf" \
    -i "TTD.brio" \
    -o "PTD.brio" \
    > flrec1.log 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] flreconstruct failed!"
    exit 1
fi

flvisualize \
    --variant-profile "profile.conf" \
    -i "PTD.brio"

exit 0

