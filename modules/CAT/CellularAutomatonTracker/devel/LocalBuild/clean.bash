#!/usr/bin/env bash

opwd=$(pwd)

if [ -d __install ]; then
    rm -fr __install
fi
if [ -d __build ]; then
    rm -fr __build
fi
exit 0
# end