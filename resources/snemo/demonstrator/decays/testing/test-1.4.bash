#!/usr/bin/env bash

debug=false
decaysVersion="1.4"
decaysVariantVersion="1.0"
decaysResourcesDir="$(pwd)"
falaiseResourcesDir="${decaysResourcesDir}/../../.."
decaysProfile="$(pwd)/decays.profile"
decayGenList="$(pwd)/decay-generators.list"
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

echo >&2 "[info] decaysVersion         = '${decaysVersion}'"
echo >&2 "[info] decaysResourcesSubdir = '${decaysResourcesSubdir}'"
echo >&2 "[info] falaiseResourcesDir     = '${falaiseResourcesDir}'"
echo >&2 "[info] falaiseBuildDir         = '${falaiseBuildDir}'"
echo >&2 "[info] falaiseLibDir           = '${falaiseLibDir}'"
# exit 0

cat >&2 <<EOF

=================
Variant inspector
=================

EOF
bxvariant_inspector \
    --datatools::resource-path="falaise@${falaiseResourcesDir}" \
    --logging="debug" \
    --variant-config="@falaise:${decaysResourcesSubdir}/variants/${decaysVariantVersion}/DecaysVariantRepository.conf" \
    --variant-gui \
    --variant-ui-writable-at-start \
    --variant-store="${decaysProfile}"
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux variant inspector failed !"
    exit 1
fi

# exit 1
reset
cat >&2 <<EOF

===================
Genbb inspector
===================

EOF

bxgenbb_inspector \
    --debug \
    --load-dll "Falaise@${falaiseLibDir}" \
    --datatools::logging "fatal" \
    --datatools::resource-path "falaise@${falaiseResourcesDir}" \
    --variant-config "@falaise:${decaysResourcesSubdir}/variants/${decaysVariantVersion}/DecaysVariantRepository.conf" \
    --variant-load "${decaysProfile}" \
    --configuration "@falaise:${decaysResourcesSubdir}/${decaysVersion}/DecaysManager.conf" \
    --action "list"  \
    --list-print-mode "raw" \
    > ${decayGenList}
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux genbb inspector failed !"
    exit 1
fi
nbDecayGenerators=$(cat ${decayGenList} | wc -l)
echo >&2 "[info] Found decay generators = ${nbDecayGenerators}"
if [ ${nbDecayGenerators} -ne 74 ]; then
    echo >&2 "[error] Unexpected number of decay generators for version ${decaysVersion} !"
    exit 1
fi

function gen_decay_list_rst()
{
    local _decayLine=""
    local _lineCount=0
    local _decayGenRstFile="decay_generators-${decaysVersion}.rst"
    local _decayGenPdfFile="decay_generators-${decaysVersion}.pdf"
    cat > ${_decayGenRstFile} <<EOF
===========================================================================================================
Falaise - SuperNEMO Demonstrator - List of decay generators (version ${decaysVersion})
===========================================================================================================

* Decay version           : ${decaysVersion}
* Decay variant version   : ${decaysVariantVersion}
EOF
    echo '  ' >> ${_decayGenRstFile}
    
    local _lenNameMax=0
    local _lenDescMax=0
    local _lenGroupMax=0
    local _lenClassMax=0
    while read -r _decayLine; do
	let _lineCount=_lineCount+1
	test ${debug} == true && echo >&2 "[debug] Line #${_lineCount} : '${_decayLine}'"
	local _token1=$(echo -n "${_decayLine}" | cut -d':' -f1 | xargs)
	local _token2=$(echo -n "${_decayLine}" | cut -d':' -f2 | xargs -0)
	local _token3=$(echo -n "${_decayLine}" | cut -d':' -f3-6 | xargs)
	test ${debug} == true && echo >&2 "[debug]     token1='${_token1}'"
	test ${debug} == true && echo >&2 "[debug]     token2='${_token2}'"
	test ${debug} == true && echo >&2 "[debug]     token3='${_token3}'"
	local _decayGenName="${_token1}"
	local _decayGenDesc=$(echo -n "${_token2}"  | cut -d';' -f1 | xargs)
	local _decayGenGroup=$(echo -n "${_token2}" | cut -d';' -f2 | sed -e 's/group=//g' | xargs)
	local _decayGenClass="${_token3}"
	test ${debug} == true && echo >&2 "[debug]   decayGenName  = '${_decayGenName}'" 
	test ${debug} == true && echo >&2 "[debug]   decayGenDesc  = '${_decayGenDesc}'" 
	test ${debug} == true && echo >&2 "[debug]   decayGenGroup = '${_decayGenGroup}'" 
	test ${debug} == true && echo >&2 "[debug]   decayGenClass = '${_decayGenClass}'" 
	local _lenName=${#_decayGenName}
	local _lenDesc=${#_decayGenDesc}
	local _lenGroup=${#_decayGenGroup}
	local _lenClass=${#decayGenClass}
	test ${debug} == true && echo >&2 "[debug]   lenName  = ${_lenName}" 
	test ${debug} == true && echo >&2 "[debug]   lenDesc  = ${_lenDesc}" 
	test ${debug} == true && echo >&2 "[debug]   lenGroup = ${_lenGroup}" 
	test ${debug} == true && echo >&2 "[debug]   lenClass = ${_lenClass}" 
	if [ ${_lenName} -gt ${_lenNameMax} ]; then
	    _lenNameMax=${_lenName}
   	    test ${debug} == true && echo >&2 "[debug]   ==========> update lenNameMax=[${_lenNameMax}] from [${_lenName}]" 
	fi
	if [ ${_lenDesc} -gt ${_lenDescMax} ]; then
	    _lenDescMax=${_lenDesc}
   	    test ${debug} == true && echo >&2 "[debug]   ==========> update lenDescMax=[${_lenDescMax}] from [${_lenDesc}]" 
	fi
	if [ ${_lenGroup} -gt ${_lenGroupMax} ]; then
	    _lenGroupMax=${_lenGroup}
   	    test ${debug} == true && echo >&2 "[debug]   ==========> update lenGroupMax=[${_lenGroupMax}] from [${_lenGroup}]" 
	fi
	if [ ${_lenClass} -gt ${_lenClassMax} ]; then
	    _lenClassMax=${_lenClass}
   	    echo >&2 "[debug]   ==========> update lenClassMax=[${_lenClassMax}] from [${_lenClass}]" 
	fi
	
    done <${decayGenList}
    if [ ${_lenClassMax} -eq 0 ]; then
	_lenClassMax=4
    fi
    test ${debug} == true && echo >&2 "[debug] lenNameMax  = ${_lenNameMax}" 
    test ${debug} == true && echo >&2 "[debug] lenDescMax  = ${_lenDescMax}" 
    test ${debug} == true && echo >&2 "[debug] lenGroupMax = ${_lenGroupMax}" 
    test ${debug} == true && echo >&2 "[debug] lenClassMax = ${_lenClassMax}" 

    let _colNameWidth=_lenNameMax+2
    let _colDescWidth=_lenDescMax+2
    let _colGroupWidth=_lenGroupMax+2
    let _colClassWidth=_lenClassMax+2
    python3 -c "print('=' * ${_colNameWidth} , '', end='')" >> ${_decayGenRstFile}
    python3 -c "print('=' * ${_colDescWidth} , '', end='')" >> ${_decayGenRstFile}
    python3 -c "print('=' * ${_colGroupWidth}, '', end='')" >> ${_decayGenRstFile}
    python3 -c "print('=' * ${_colClassWidth}, '', end='')" >> ${_decayGenRstFile}
    python3 -c "print()" >> ${_decayGenRstFile}
    python3 -c "print('{:${_colNameWidth}s} {:${_colDescWidth}s} {:${_colGroupWidth}s} {:${_colClassWidth}s}'.format(' **Name**', ' **Description**', ' **Group**', ' **Class**'))" >> ${_decayGenRstFile} 
    python3 -c "print('=' * ${_colNameWidth} , '', end='')" >> ${_decayGenRstFile}
    python3 -c "print('=' * ${_colDescWidth} , '', end='')" >> ${_decayGenRstFile}
    python3 -c "print('=' * ${_colGroupWidth}, '', end='')" >> ${_decayGenRstFile}
    python3 -c "print('=' * ${_colClassWidth}, '', end='')" >> ${_decayGenRstFile}
    python3 -c "print()" >> ${_decayGenRstFile}
    while read -r _decayLine; do
	local _token1=$(echo -n "${_decayLine}" | cut -d':' -f1 | xargs)
	local _token2=$(echo -n "${_decayLine}" | cut -d':' -f2 | xargs -0)
	local _token3=$(echo -n "${_decayLine}" | cut -d':' -f3-6 | xargs)
	local _decayGenName="${_token1}"
	local _decayGenDesc=$(echo -n "${_token2}" | cut -d';' -f1  | xargs -0)
	local _decayGenGroup=$(echo -n "${_token2}" | cut -d';' -f2 | sed -e 's/group=//g' | xargs)
	local _decayGenClass="${_token3}"
	python3 -c "print('{:${_colNameWidth}s} {:${_colDescWidth}s} {:${_colGroupWidth}s} {:${_colClassWidth}s}'.format(' ``${_decayGenName}``', ' ${_decayGenDesc}', ' ``${_decayGenGroup}``', ' ``${_decayGenClass}``'))" >> ${_decayGenRstFile} 
    done <${decayGenList}
    python3 -c "print('=' * ${_colNameWidth} , '', end='')" >> ${_decayGenRstFile}
    python3 -c "print('=' * ${_colDescWidth} , '', end='')" >> ${_decayGenRstFile}
    python3 -c "print('=' * ${_colGroupWidth}, '', end='')" >> ${_decayGenRstFile}
    python3 -c "print('=' * ${_colClassWidth}, '', end='')" >> ${_decayGenRstFile}
    python3 -c "print()" >> ${_decayGenRstFile}
 
    test ${debug} == true && echo >&2 "[debug] Generated RST file: '${_decayGenRstFile}'" 
    cat >> ${_decayGenRstFile} <<EOF

.. end

EOF
    
    ls ${_decayGenRstFile} 
    rst2pdf -s ${falaiseResourcesDir}/${decaysResourcesSubdir}/testing/this_rst.style --repeat-table-rows ${_decayGenRstFile} ${_decayGenPdfFile}
    ls ${_decayGenPdfFile}
    xdg-open ${_decayGenPdfFile} & 
    rm -f ${decayGenList}
    return
}

gen_decay_list_rst

bxgenbb_inspector \
    --load-dll "Falaise@${falaiseLibDir}" \
    --datatools::resource-path "falaise@${falaiseResourcesDir}" \
    --variant-config "@falaise:${decaysResourcesSubdir}/variants/${decaysVariantVersion}/DecaysVariantRepository.conf" \
    --variant-load "${decaysProfile}" \
    --configuration "@falaise:${decaysResourcesSubdir}/${decaysVersion}/DecaysManager.conf" \
    --action "shoot"  \
    --generator "Co60" \
    --prng-seed 314159 \
    --number-of-events 100000 \
    --modulo 1000 \
    --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf" \
    --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf" \
    --prompt-time-limit 100 \
    --prompt \
    --title-prefix "Co60" \
    --output-file "histos_Co60.root"
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux genbb inspector failed !"
    exit 1
fi

root histos_Co60.root


exit 0
