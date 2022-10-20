#!/usr/bin/env bash
geometryVersion="5.0"
geometryVariantVersion="2.0"
vertexVersion="5.0"
vertexVariantVersion="1.0"
decaysVersion="1.4"
decaysVariantVersion="1.0"
simulationVersion="2.4"
simulationVariantVersion="2.0"

geomSourceLayout="RealisticFlat"
# geomSourceLayout="RealisticSNRS1"
if [ ${geomSourceLayout} = "RealisticFlat" ]; then
    vertexGenerator="real_flat_source_full_foils_surface"
elif [ ${geomSourceLayout} = "RealisticSNRS1" ] ; then
    vertexGenerator="real_snrs1_source_full_foils_surface"
else
    vertexGenerator="free_spot"
fi
primaryGenerator="Co60"

simulationResourcesDir="$(pwd)"
falaiseResourcesDir="${simulationResourcesDir}/../../.."
simulationProfile="$(pwd)/simulation.profile"
origPwd="$(pwd)"
if [ ! -d ${falaiseResourcesDir} ]; then
    echo >&2 "[error] Directory does not exist '${falaiseResourcesDir}' !"
    cd ${origPwd}
    exit 1
fi
cd ${falaiseResourcesDir}
falaiseResourcesDir="$(pwd)"
cd ${origPwd}

decaysResourcesSubdir="snemo/demonstrator/decays"
simulationResourcesSubdir="snemo/demonstrator/simulation"
vertexResourcesSubdir="snemo/demonstrator/vertex"
geometryResourcesSubdir="snemo/demonstrator/geometry"
workDir="$(pwd)/_work.d"

falaiseBuildDir="${falaiseResourcesDir}/../_build.d/develop/BuildProducts"
if [ ! -d ${falaiseBuildDir} ]; then
    echo >&2 "[error] Falaise build directory does not exist '${falaiseBuildDir}' !"
    exit 1
fi
cd ${falaiseBuildDir}
falaiseBuildDir="$(pwd)"
cd ${origPwd}

falaiseLibDir="${falaiseBuildDir}/lib"
if [ ! -d ${falaiseLibDir} ]; then
    echo >&2 "[error] Falaise lib directory does not exist '${falaiseLibDir}' !"
    exit 1
fi
cd ${falaiseLibDir}
falaiseLibDir="$(pwd)"
cd ${origPwd}

echo >&2 "[info] simulationVersion         = '${simulationVersion}'"
echo >&2 "[info] decaysVersion             = '${decaysVersion}'"
echo >&2 "[info] vertexVersion             = '${vertexVersion}'"
echo >&2 "[info] geometryVersion           = '${geometryVersion}'"
echo >&2 "[info] simulationResourcesSubdir = '${simulationResourcesSubdir}'"
echo >&2 "[info] vertexResourcesSubdir     = '${vertexResourcesSubdir}'"
echo >&2 "[info] geometryResourcesSubdir   = '${geometryResourcesSubdir}'"
echo >&2 "[info] decaysResourcesSubdir     = '${decaysResourcesSubdir}'"
echo >&2 "[info] falaiseResourcesDir       = '${falaiseResourcesDir}'"
echo >&2 "[info] falaiseBuildDir           = '${falaiseBuildDir}'"
echo >&2 "[info] falaiseLibDir             = '${falaiseLibDir}'"
# exit 0

cat >&2 <<EOF

=================
Variant inspector
=================

EOF
simulationProfileLoad="${falaiseResourcesDir}/${simulationResourcesSubdir}/testing/simulation-1.profile"
echo >&2 "[info] simulationProfileLoad = '${simulationProfileLoad}'"
echo >&2 "[info] vertexGenerator  = '${vertexGenerator}'"
echo >&2 "[info] primaryGenerator = '${primaryGenerator}'"
# exit 1
#    --variant-load="${simulatioProfileLoad}" 
#     --variant-set="primary_events:generator=${primaryGenerator}" 
    # --variant-set="geometry:layout/if_basic/source_layout=${geomSourceLayout}" \
    # --variant-set="vertexes:generator=${vertexGenerator}" \
#
bxvariant_inspector \
    --datatools::resource-path="falaise@${falaiseResourcesDir}" \
    --logging="debug" \
    --variant-config="@falaise:${simulationResourcesSubdir}/variants/${simulationVariantVersion}/SimulationVariantRepository.conf" \
    --variant-gui \
    --variant-ui-writable-at-start \
    --variant-store="${simulationProfile}"
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux variant inspector failed !"
    exit 1
fi
# exit 0
reset

cat >&2 <<EOF

===================
Geant4 production
===================

EOF

if [ ! -d ${workDir} ]; then
    mkdir -p ${workDir}
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Work dir not created!"
	exit 1
    fi
fi
bxg4_production \
    --logging-priority "fatal" \
    --datatools::logging "trace" \
    --load-dll "Falaise@${falaiseLibDir}" \
    --datatools::resource-path "falaise@${falaiseResourcesDir}" \
    --variant-config "@falaise:${simulationResourcesSubdir}/variants/${simulationVariantVersion}/SimulationVariantRepository.conf" \
    --variant-load "${simulationProfile}" \
    --batch \
    --config "@falaise:${simulationResourcesSubdir}/${simulationVersion}/SimulationManager.conf" \
    --vertex-generator-seed 0 \
    --event-generator-seed 0  \
    --g4-manager-seed 0       \
    --shpf-seed 0             \
    --output-prng-seeds-file "${workDir}/mc_g4_production.seeds" \
    --output-prng-states-file "${workDir}/mc_g4_production.states" \
    --number-of-events 10 \
    --number-of-events-modulo 1 \
    --output-data-file "${workDir}/falaise_simulation.xml.gz" \
    --output-data-format "bank" \
    --output-data-bank name "SD"
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux Geant4 production failed !"
    exit 1
fi



exit 0
