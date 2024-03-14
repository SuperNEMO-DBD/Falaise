#!/usr/bin/env bash

uddDataPath=/data/SuperNEMO/UDD/
# Example script used to extract a few UDD events from an input brio file:
flreconstruct \
    --max-number-events 10 \
    -p extract_udd.conf \
    -i ${uddDataPath}/snemo_run-724_udd.brio \
    -o snemo_run-724_udd-10events.brio

exit 0
