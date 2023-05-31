#!/bin/bash
set -ex

workDir="$1"

flsimulate -c ${workDir}/issue201_sim.conf -o issue201_sim.brio
flreconstruct -i issue201_sim.brio -p ${workDir}/../testStandardPipelineURN.conf  -o issue201_reco.brio
flreconstruct -i issue201_reco.brio -p ${workDir}/issue201_reco.conf -o issue201_reco_2.brio

# For developers' test only:
# cp issue201_sim.brio issue201_reco.brio issue201_reco_2.brio /tmp/${USER}/
# tree  /tmp/${USER}/
