#!/usr/bin/env bash

with_visu=0

which flsimulate > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] flsimulate is not available! Abort!"
    exit 1
fi
export FLTMPDIR="./_tmp.d"
mkdir -p ${FLTMPDIR}

echo >&2 "[info] Running flsimulate-configure..."
flsimulate-configure --no-gui \
    -t "urn:snemo:demonstrator:simulation:2.1" \
    -s "vertexes:generator=source_pads_bulk" \
    -s "primary_events:generator=Se82.0nubb" \
    -s "simulation:output_profile=source_details" \
    -o "${FLTMPDIR}/validate-issue16.vprofile"

cat ${FLTMPDIR}/validate-issue16.vprofile
echo >&2 ""

echo >&2 "[info] Running flsimulate..."
flsimulate -c config/simu.conf -o ${FLTMPDIR}/validate-issue16.xml

grep --max-count=1 "__source_SD" ${FLTMPDIR}/validate-issue16.xml
if [ $? -ne 0 ]; then
    echo >&2 "[error] flsimulate output file contains no '__visu.track' hits from the source pads! Abort!"
    exit 1
else
    echo >&2 "[info] flsimulate output file contains '__visu.track' hits from the source pads!"
fi
echo >&2 ""

if [ ${with_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	-i ${FLTMPDIR}/validate-issue16.xml \
	--focus-on-roi \
	--show-simulated-vertex 1 \
	--show-simulated-tracks 1 \
	--show-simulated-hits 1
fi

rm -fr ${FLTMPDIR}

exit 0

# end
