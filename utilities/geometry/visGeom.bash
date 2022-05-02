#!/usr/bin/env bash

which bxquery > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] No Bayeux !"
    exit 1
fi

flsimConfExe=$(which flsimulate-configure)
if [ "x${flsimConfExe}" = "x" ]; then
    echo >&2 "[error] No Falaise !"
    exit 1
fi

falaiseInstallDir=$(dirname ${flsimConfExe})/..
echo >&2 "[info] Falaise install dir : ${falaiseInstallDir}"
falaiseVersion="$(flsimulate --version | grep Falaise | cut -d':' -f2 | cut -d'(' -f1 | tr -d ' ' )"

echo >&2 "[info] Falaise version : ${falaiseVersion}"

bxgeomInspExe=$(which bxgeomtools_inspector)

echo >&2 "[info] Geometry inspector : ${bxgeomInspExe}"

bxgeomtools_inspector \
    --logging "debug" \
    --datatools::resource-path "falaise@${falaiseInstallDir}/share/Falaise-${falaiseVersion}/resources" \
    --load-dll "Falaise@${falaiseInstallDir}/lib" \
    --variant-config "@falaise:snemo/demonstrator/geometry/GeometryVariantRepository.conf" \
    --manager-config "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" \
    --variant-load "profile.conf"


exit 0
