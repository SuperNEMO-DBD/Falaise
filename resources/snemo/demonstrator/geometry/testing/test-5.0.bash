#!/usr/bin/env bash

geometryVersion="5.0"
geometryVariantVersion="2.0"
geometryResourcesDir="$(pwd)"
falaiseResourcesDir="${geometryResourcesDir}/../../.."
geometryProfile="$(pwd)/geometry.profile"
origPwd="$(pwd)"
if [ ! -d ${falaiseResourcesDir} ]; then
    echo >&2 "[error] Directory does not exist '${falaiseResourcesDir}' !"
    cd ${origPwd}
    exit 1
fi
cd ${falaiseResourcesDir}
falaiseResourcesDir="$(pwd)"
cd ${origPwd}

geometryResourcesSubdir="snemo/demonstrator/geometry"

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

echo >&2 "[info] geometryVersion         = '${geometryVersion}'"
echo >&2 "[info] geometryResourcesSubdir = '${geometryResourcesSubdir}'"
echo >&2 "[info] falaiseResourcesDir     = '${falaiseResourcesDir}'"
echo >&2 "[info] falaiseBuildDir         = '${falaiseBuildDir}'"
echo >&2 "[info] falaiseLibDir           = '${falaiseLibDir}'"

cat >&2 <<EOF

=================
Variant inspector
=================

EOF
bxvariant_inspector \
    --datatools::resource-path="falaise@${falaiseResourcesDir}" \
    --logging="debug" \
    --variant-config="@falaise:${geometryResourcesSubdir}/variants/${geometryVariantVersion}/GeometryVariantRepository.conf" \
    --variant-gui \
    --variant-ui-writable-at-start \
    --variant-store="${geometryProfile}" 
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux variant inspector failed !"
    exit 1
fi

# exit 1
reset
cat >&2 <<EOF

===================
Geomtools inspector
===================

EOF

bxgeomtools_inspector \
    --logging "trace" \
    --datatools::logging "trace" \
    --datatools::resource-path "falaise@${falaiseResourcesDir}" \
    --load-dll "Falaise@${falaiseLibDir}" \
    --interactive \
    --variant-config "@falaise:${geometryResourcesSubdir}/variants/${geometryVariantVersion}/GeometryVariantRepository.conf" \
    --variant-load "${geometryProfile}" \
    --manager-config "@falaise:${geometryResourcesSubdir}/${geometryVersion}/GeometryManager.conf" 
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux geomtools inspector failed !"
    exit 1
fi

exit 0
