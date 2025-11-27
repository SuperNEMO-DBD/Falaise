#!/usr/bin/env bash

# !!!! usable by frc only for local tests

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

falaiseBuildDir="${falaiseResourcesDir}/../Falaise.build/BuildProducts"
if [ -d "/opt/SW/SuperNEMO-DBD/Falaise/_build-dev.d/BuildProducts" ]; then
    echo >&2 "[info] Loading frc's special development stuff..."
    falaiseBuildDir="/opt/SW/SuperNEMO-DBD/Falaise/_build-dev.d/BuildProducts"
fi
if [ ! -d ${falaiseBuildDir} ]; then
    echo >&2 "[warning] Falaise build directory does not exist '${falaiseBuildDir}' ! Ask for user input..."
    read -p "Enter Falaise build directory: "
    falaiseBuildDir="${REPLY}"
fi
if [ ! -d ${falaiseBuildDir} ]; then
    echo >&2 "[error] Falaise build directory does not exist '${falaiseBuildDir}' ! Abort."
    exit 1
fi
cd ${falaiseBuildDir}
falaiseBuildDir="$(pwd)"
cd ${origPwd}

falaiseLibDir="${falaiseBuildDir}/lib"
if [ ! -d ${falaiseLibDir} ]; then
    echo >&2 "[warning] Falaise library build directory does not exist '${falaiseLibDir}' ! Trying 'lib64'..."
    falaiseLibDir="${falaiseBuildDir}/lib64"
fi
if [ ! -d ${falaiseLibDir} ]; then
    echo >&2 "[error] Falaise library build directory does not exist '${falaiseLibDir}' ! Abort."
    exit 1
fi
cd ${falaiseLibDir}
falaiseLibDir="$(pwd)"
cd ${origPwd}
falaiseDllOptions="--load-dll Falaise@${falaiseLibDir}" 

# which snrs-config > /dev/null 2>&1
# if [ $? -ne 0 ]; then
#     echo >&2 "[error] SNRS library has not been located! Abort."
#     exit 1 
# fi
# snrsLibDir=$(snrs-config --libdir)
# snrsDllOptions="--load-dll snrs@${snrsLibDir}"

echo >&2 "[info] geometryVersion         = '${geometryVersion}'"
echo >&2 "[info] geometryVariantVersion  = '${geometryVariantVersion}'"
echo >&2 "[info] geometryResourcesSubdir = '${geometryResourcesSubdir}'"
echo >&2 "[info] falaiseResourcesDir     = '${falaiseResourcesDir}'"
echo >&2 "[info] falaiseBuildDir         = '${falaiseBuildDir}'"
echo >&2 "[info] falaiseLibDir           = '${falaiseLibDir}'"
echo >&2 "[info] falaiseDllOptions       = '${falaiseDllOptions}'"
# echo >&2 "[info] snrsDllOptions          = '${snrsDllOptions}'"

# exit

cat >&2 <<EOF

=================
Variant inspector
=================

EOF
bxvariant_inspector \
    --datatools::resource-path="falaise@${falaiseResourcesDir}" \
    --logging="debug" \
    --variant-config="@falaise:${geometryResourcesSubdir}/variants/service/${geometryVariantVersion}/GeometryVariantRepository.conf" \
    --variant-gui \
    --variant-ui-writable-at-start \
    --variant-store="${geometryProfile}" 
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux variant inspector failed !"
    exit 1
fi

# exit 1
# reset
cat >&2 <<EOF

===================
Geomtools inspector
===================

EOF

bxgeomtools_inspector \
    --logging "trace" \
    --datatools::logging "trace" \
    --datatools::resource-path "falaise@${falaiseResourcesDir}" \
    ${falaiseDllOptions} \
    --interactive \
    --variant-config "@falaise:${geometryResourcesSubdir}/variants/service/${geometryVariantVersion}/GeometryVariantRepository.conf" \
    --variant-load "${geometryProfile}" \
    --manager-config "@falaise:${geometryResourcesSubdir}/${geometryVersion}/GeometryManager.conf" 
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux geomtools inspector failed !"
    exit 1
fi

exit 0
