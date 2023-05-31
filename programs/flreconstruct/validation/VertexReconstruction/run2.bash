#!/usr/bin/env bash

test -L _profile.conf  && rm -f _profile.conf
test -L _pipeline.conf && rm -f _pipeline.conf 
# test -f flatSource-simu.brio && rm -f flatSource-simu.brio 
# test -f flatSource-reco.brio && rm -f flatSource-reco.brio 

which flsimulate > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] Cannot find flsimulate! Abort!"
    exit 1
fi

which flsimulate-configure > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] Cannot find flsimulate-configure! Abort!"
    exit 1
fi

which flreconstruct > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] Cannot find flreconstruct! Abort!"
    exit 1
fi

flatSourceProfile="profile-flat-source.conf"
recoPipeline="recopipeline.conf"

flsimulate-configure \
    --no-gui \
    -t "urn:snemo:demonstrator:simulation:2.4" \
    -v "urn:snemo:demonstrator:simulation:variants:service:2.0" \
    -s "geometry:layout/if_basic/magnetic_field=false" \
    -s "geometry:layout/if_basic/source_layout=RealisticSNRS1" \
    -s "primary_events:generator=electron.1MeV" \
    -s "vertexes:generator=free_spot" \
    -s "vertexes:generator/if_free_spot/x=0 mm" \
    -s "vertexes:generator/if_free_spot/y=30 mm" \
    -s "vertexes:generator/if_free_spot/z=50 mm" \
    -s "simulation:output_profile=all_details" \
    -o ${flatSourceProfile}
if [ $? -ne 0 ]; then
    echo >&2 "[error] flsimulate-configure failed! Abort!"
    exit 1
fi

nbEvents=10

ln -s ${flatSourceProfile} _profile.conf
flsimulate \
    -N ${nbEvents} \
    --config simu.conf \
    --output-file flatSource-simu.brio
if [ $? -ne 0 ]; then
    echo >&2 "[error] flsimulate failed! Abort!"
    exit 1
fi

ln -s ${recoPipeline} _pipeline.conf
flreconstruct \
    --verbosity "debug" \
    --config reco.conf \
    --input-file flatSource-simu.brio \
    --output-file flatSource-reco.brio
if [ $? -ne 0 ]; then
    echo >&2 "[error] flreconstruct failed! Abort!"
    exit 1
fi

flvisualize \
    --input-file flatSource-reco.brio \
    --variant-profile _profile.conf
if [ $? -ne 0 ]; then
    echo >&2 "[error] flvisualize failed! Abort!"
    exit 1
fi

test -L _pipeline.conf && rm -f _pipeline.conf 
test -L _profile.conf  && rm -f _profile.conf

exit 0
