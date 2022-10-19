#!/usr/bin/env bash
geometryVersion="5.0"
geometryVariantVersion="2.0"
vertexVersion="5.0"
vertexVariantVersion="1.0"

geomSourceLayout="RealisticFlat"
geomSourceLayout="RealisticSNRS1"
if [ ${geomSourceLayout} = "RealisticFlat" ]; then
    vertexGenerator="real_flat_source_full_foils_surface"
elif [ ${geomSourceLayout} = "RealisticSNRS1" ] ; then
    vertexGenerator="real_snrs1_source_full_foils_surface"
else
    vertexGenerator="free_spot"
fi

vertexResourcesDir="$(pwd)"
falaiseResourcesDir="${vertexResourcesDir}/../../.."
vertexProfileLoad="$(pwd)/testing/vertexes-1.profile"
vertexProfile="$(pwd)/vertexes.profile"
vertexGenList="$(pwd)/vertex_generators.list"
origPwd="$(pwd)"
if [ ! -d ${falaiseResourcesDir} ]; then
    echo >&2 "[error] Directory does not exist '${falaiseResourcesDir}' !"
    cd ${origPwd}
    exit 1
fi
cd ${falaiseResourcesDir}
falaiseResourcesDir="$(pwd)"
cd ${origPwd}

vertexResourcesSubdir="snemo/demonstrator/vertex"
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
echo >&2 "[info] vertexVersion           = '${vertexVersion}'"
echo >&2 "[info] vertexResourcesSubdir   = '${vertexResourcesSubdir}'"
echo >&2 "[info] falaiseResourcesDir     = '${falaiseResourcesDir}'"
echo >&2 "[info] falaiseBuildDir         = '${falaiseBuildDir}'"
echo >&2 "[info] falaiseLibDir           = '${falaiseLibDir}'"
# exit 0

cat<<EOF

=================
Variant inspector
=================

EOF
bxvariant_inspector \
    --datatools::resource-path="falaise@${falaiseResourcesDir}" \
    --logging="debug" \
    --variant-config="@falaise:${vertexResourcesSubdir}/variants/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" \
    --variant-gui \
    --variant-load="${vertexProfileLoad}" \
    --variant-set="geometry:layout/if_basic/source_layout=${geomSourceLayout}" \
    --variant-set="vertexes:generator=${vertexGenerator}" \
    --variant-ui-writable-at-start \
    --variant-store="${vertexProfile}"
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux variant inspector failed !"
    exit 1
fi
reset

# exit 0
cat<<EOF

===================
Genvtx production
===================

EOF

bxgenvtx_production \
    --logging "debug" \
    --load-dll "Falaise@${falaiseLibDir}" \
    --datatools::logging "fatal" \
    --datatools::resource-path "falaise@${falaiseResourcesDir}" \
    --variant-config "@falaise:${vertexResourcesSubdir}/variants/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" \
    --variant-load "${vertexProfile}" \
    --geometry-manager "@falaise:${geometryResourcesSubdir}/${geometryVersion}/GeometryManager.conf" \
    --vertex-generator-manager "@falaise:${vertexResourcesSubdir}/${vertexVersion}/VertexGeneratorManager.conf" \
    --list \
    > ${vertexGenList}
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux genvtx production failed !"
    exit 1
fi

nbVertexGenerators=$(cat ${vertexGenList} | wc -l)
echo >&2 "[info] Found vertex generators = ${nbVertexGenerators}"
# if [ ${nbVertexGenerators} -ne 477 ]; then
#     echo >&2 "[error] Unexpected number of vertex generators for version ${vertexVersion} !"
#     exit 1
# fi

function gen_vertex_list_rst()
{
    echo >&2 "Entering 'gen_vertex_list_rst'..."
    local _vertexLine=""
    local _lineCount=0
    local _vertexGenRstFile="vertex_generators-${vertexVersion}.rst"
    local _vertexGenPdfFile="vertex_generators-${vertexVersion}.pdf"
    cat > ${_vertexGenRstFile} <<EOF
===========================================================================================================
Falaise - SuperNEMO Demonstrator - List of vertex generators (version ${vertexVersion})
===========================================================================================================

* Geometry version         : ${geometryVersion}
* Geometry variant version : ${geometryVariantVersion}
* Vertex version           : ${vertexVersion}
* Vertex variant version   : ${vertexVariantVersion}
* Selected variant profile:
EOF
    echo '  ' >> ${_vertexGenRstFile} 
    echo '  .. code:: shell' >> ${_vertexGenRstFile} 
    echo '  ' >> ${_vertexGenRstFile} 
    while read -r _vertexProfileLine; do
	echo "      ${_vertexProfileLine}" >> ${_vertexGenRstFile}
    done <${vertexProfile}
    echo '  ..' >> ${_vertexGenRstFile} 
    echo ''   >> ${_vertexGenRstFile} 
    echo '.. raw:: pdf'   >> ${_vertexGenRstFile} 
    echo ''   >> ${_vertexGenRstFile} 
    echo '   PageBreak'   >> ${_vertexGenRstFile} 
    echo '..'   >> ${_vertexGenRstFile} 
    echo ''   >> ${_vertexGenRstFile} 
    
    local _lenNameMax=0
    local _lenDescMax=0
    local _lenGroupMax=0
    local _lenRankMax=0
    local _lenVariantMax=0
    while read -r _vertexLine; do
	let _lineCount=_lineCount+1
	echo >&2 "Line #${_lineCount} : '${_vertexLine}'"
	local _token1=$(echo -n "${_vertexLine}" | cut -d':' -f1 | xargs)
	local _token2=$(echo -n "${_vertexLine}" | cut -d':' -f2 | xargs -0)
	echo >&2 "    token1='${_token1}'"
	echo >&2 "    token2='${_token2}'"
	local _vertexGenName="${_token1}"
	local _vertexGenDesc=$(echo -n "${_token2}"  | cut -d';' -f1 | sed -e 's/'\''//g' | xargs)
	local _vertexGenGroup=''
	local _vertexGenRank=''
	local _vertexGenVariant=''
	local _vertexDescAdd1=$(echo -n "${_token2};" | cut -d';' -f2  | xargs )
	local _vertexDescAdd2=$(echo -n "${_token2};" | cut -d';' -f3  | xargs )
	local _vertexDescAdd3=$(echo -n "${_token2};" | cut -d';' -f4  | xargs )
	echo >&2 "============== _vertexDescAdd1='${_vertexDescAdd1}'"	
	echo >&2 "============== _vertexDescAdd2='${_vertexDescAdd2}'"	
	echo >&2 "============== _vertexDescAdd3='${_vertexDescAdd3}'"	
	local _vertexDescAdds=()
	if [ -n "${_vertexDescAdd1}" ]; then
	    _vertexDescAdds+=("${_vertexDescAdd1}")
	fi
	if [ -n "${_vertexDescAdd2}" ]; then
	    _vertexDescAdds+=("${_vertexDescAdd2}")
	fi
	if [ -n "${_vertexDescAdd3}" ]; then
	    _vertexDescAdds+=("${_vertexDescAdd3}")
	fi
	echo >&2 "============== _vertexDescAdds='${_vertexDescAdds[@]}'"	
	for _vertexDescAdd in ${_vertexDescAdds[@]} ; do
   	    echo >&2 "============== processing _vertexDescAdd='${_vertexDescAdd}'"	
	    echo "${_vertexDescAdd}" | grep group= > /dev/null
	    if [ $? -eq 0 ]; then
 		_vertexGenGroup=$(echo "${_vertexDescAdd}" | sed -e 's/group=//g') 	
 		echo >&2 "============== Found group='${_vertexGenGroup}'!"	
	    else
		echo "${_vertexDescAdd}" | grep rank= > /dev/null
		if [ $? -eq 0 ]; then
		    _vertexGenRank=$(echo "${_vertexDescAdd}" | sed -e 's/rank=//g') 	
		    echo >&2 "============== Found rank='${_vertexGenRank}'!"	
		else	
		    echo "${_vertexDescAdd}" | grep variant= > /dev/null
		    if [ $? -eq 0 ]; then
			_vertexGenVariant=$(echo "${_vertexDescAdd}" | sed -e 's/variant=//g') 	
			echo >&2 "============== Found variant='${_vertexGenVariant}'!"	
		    fi
		fi
	    fi
	done
	echo >&2 "  vertexGenName    = '${_vertexGenName}'" 
	echo >&2 "  vertexGenDesc    = '${_vertexGenDesc}'" 
	echo >&2 "  vertexGenGroup   = '${_vertexGenGroup}'" 
	echo >&2 "  vertexGenVariant = '${_vertexGenVariant}'" 
	echo >&2 "  vertexGenRank    = '${_vertexGenRank}'" 
	local _lenName=${#_vertexGenName}
	local _lenDesc=${#_vertexGenDesc}
	local _lenGroup=${#_vertexGenGroup}
	local _lenVariant=${#_vertexGenVariant}
	local _lenRank=${#_vertexGenRank}
	echo >&2 "  lenName    = ${_lenName}" 
	echo >&2 "  lenDesc    = ${_lenDesc}" 
	echo >&2 "  lenGroup   = ${_lenGroup}" 
	echo >&2 "  lenVariant = ${_lenVariant}" 
	echo >&2 "  lenRank    = ${_lenRank}" 
	if [ ${_lenName} -gt ${_lenNameMax} ]; then
	    _lenNameMax=${_lenName}
   	    echo >&2 "  ==========> update lenNameMax=[${_lenNameMax}] from [${_lenName}]" 
	fi
	if [ ${_lenDesc} -gt ${_lenDescMax} ]; then
	    _lenDescMax=${_lenDesc}
   	    echo >&2 "  ==========> update lenDescMax=[${_lenDescMax}] from [${_lenDesc}]" 
	fi
	if [ ${_lenGroup} -gt ${_lenGroupMax} ]; then
	    _lenGroupMax=${_lenGroup}
   	    echo >&2 "  ==========> update lenGroupMax=[${_lenGroupMax}] from [${_lenGroup}]" 
	fi
	if [ ${_lenRank} -gt ${_lenRankMax} ]; then
	    _lenRankMax=${_lenRank}
   	    echo >&2 "  ==========> update lenRankMax=[${_lenRankMax}] from [${_lenRank}]" 
	fi
	if [ ${_lenVariant} -gt ${_lenVariantMax} ]; then
	    _lenVariantMax=${_lenVariant}
   	    echo >&2 "  ==========> update lenVariantMax=[${_lenVariantMax}] from [${_lenVariant}]" 
	fi
	# if [ ${_lineCount} -ge 40 ]; then
	#     break
	# fi
    done <${vertexGenList}
    if [ ${_lenRankMax} -eq 0 ]; then
	_lenRankMax=8
    fi
    if [ ${_lenVariantMax} -eq 0 ]; then
	_lenVariantMax=11
    fi
    echo >&2 "lenNameMax  = ${_lenNameMax}" 
    echo >&2 "lenDescMax  = ${_lenDescMax}" 
    echo >&2 "lenGroupMax = ${_lenGroupMax}" 
    echo >&2 "lenRankMax = ${_lenRankMax}" 
    echo >&2 "lenVariantMax = ${_lenVariantMax}" 
    
    let _colNameWidth=_lenNameMax+2
    let _colDescWidth=_lenDescMax+2
    let _colGroupWidth=_lenGroupMax+2
    let _colVariantWidth=_lenVariantMax+2
    let _colRankWidth=_lenRankMax+2
    python3 -c "print('=' * ${_colNameWidth} , '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colDescWidth} , '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colGroupWidth}, '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colVariantWidth}, '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colRankWidth}, '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print()" >> ${_vertexGenRstFile}
    python3 -c "print('{:${_colNameWidth}s} {:${_colDescWidth}s} {:${_colGroupWidth}s} {:${_colVariantWidth}s} {:${_colRankWidth}s}'.format(' **Name**', ' **Description**', ' **Group**', ' **Variant**', ' **Rank**'))" >> ${_vertexGenRstFile} 
    python3 -c "print('=' * ${_colNameWidth} , '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colDescWidth} , '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colGroupWidth}, '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colVariantWidth}, '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colRankWidth}, '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print()" >> ${_vertexGenRstFile}
    _lineCount=0
    while read -r _vertexLine; do
	let _lineCount=_lineCount+1
	echo >&2 "  ==========> second processing line #${_lineCount}" 
	local _token1=$(echo -n "${_vertexLine}" | cut -d':' -f1 | xargs)
	local _token2=$(echo -n "${_vertexLine}" | cut -d':' -f2 | xargs -0)
	local _vertexGenName="${_token1}"
	local _vertexGenDesc=$(echo -n "${_token2}"  | cut -d';' -f1 | sed -e 's/'\''//g' | xargs)
	local _vertexGenGroup=''
	local _vertexGenRank=''
	local _vertexGenVariant=''
	local _vertexDescAdd1=$(echo -n "${_token2};" | cut -d';' -f2  | xargs )
	local _vertexDescAdd2=$(echo -n "${_token2};" | cut -d';' -f3  | xargs )
	local _vertexDescAdd3=$(echo -n "${_token2};" | cut -d';' -f4  | xargs )
	local _vertexDescAdds=()
	if [ -n "${_vertexDescAdd1}" ]; then
	    _vertexDescAdds+=("${_vertexDescAdd1}")
	fi
	if [ -n "${_vertexDescAdd2}" ]; then
	    _vertexDescAdds+=("${_vertexDescAdd2}")
	fi
	if [ -n "${_vertexDescAdd3}" ]; then
	    _vertexDescAdds+=("${_vertexDescAdd3}")
	fi
	for _vertexDescAdd in ${_vertexDescAdds[@]} ; do
	    echo "${_vertexDescAdd}" | grep group= > /dev/null
	    if [ $? -eq 0 ]; then
 		_vertexGenGroup=$(echo "${_vertexDescAdd}" | sed -e 's/group=//g') 	
	    else
		echo "${_vertexDescAdd}" | grep rank= > /dev/null
		if [ $? -eq 0 ]; then
		    _vertexGenRank=$(echo "${_vertexDescAdd}" | sed -e 's/rank=//g') 	
		else	
		    echo "${_vertexDescAdd}" | grep variant= > /dev/null
		    if [ $? -eq 0 ]; then
			_vertexGenVariant=$(echo "${_vertexDescAdd}" | sed -e 's/variant=//g') 	
		    fi
		fi
	    fi
	done
	if [ "x${_vertexGenRank}" = "xhighlight" ]; then
	    _vertexGenRank='H'
	elif [ "x${_vertexGenRank}" = "xfirst" ]; then
	    _vertexGenRank='1'
	elif [ "x${_vertexGenRank}" = "xsecond" ]; then
	    _vertexGenRank='2'
	elif [ "x${_vertexGenRank}" = "xthird" ]; then
	    _vertexGenRank='3'
	elif [ "x${_vertexGenRank}" = "xlast" ]; then
	    _vertexGenRank='L'
	else
	    _vertexGenRank=' '
	fi
	python3 -c "print('{:${_colNameWidth}s} {:${_colDescWidth}s} {:${_colGroupWidth}s} {:${_colVariantWidth}s} {:${_colRankWidth}s}'.format(' ``${_vertexGenName}``', ' ${_vertexGenDesc}', ' ``${_vertexGenGroup}``', ' ``${_vertexGenVariant}``', ' ``${_vertexGenRank}``'))" >> ${_vertexGenRstFile} 
    done <${vertexGenList}
    
    python3 -c "print('=' * ${_colNameWidth}    , '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colDescWidth}    , '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colGroupWidth}   , '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colVariantWidth} , '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print('=' * ${_colRankWidth}    , '', end='')" >> ${_vertexGenRstFile}
    python3 -c "print()" >> ${_vertexGenRstFile}
    
    echo >&2 "Generated RST file: '${_vertexGenRstFile}'"
    echo '' >> ${_vertexGenRstFile} 
#     cat >> ${_vertexGenRstFile} <<EOF
    
# .. end
# EOF					
    ls ${_vertexGenRstFile} 
    rst2pdf -s ${falaiseResourcesDir}/${vertexResourcesSubdir}/testing/this_rst.style --repeat-table-rows ${_vertexGenRstFile} ${_vertexGenPdfFile}
    ls ${_vertexGenPdfFile}
    xdg-open ${_vertexGenPdfFile} & 
    rm -f ${vertexGenList}
    return
}

gen_vertex_list_rst

# exit 0

#######################
bxgenvtx_production \
    --logging "fatal" \
    --load-dll "Falaise@${falaiseLibDir}" \
    --datatools::logging "fatal" \
    --datatools::resource-path "falaise@${falaiseResourcesDir}" \
    --variant-config "@falaise:${vertexResourcesSubdir}/variants/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" \
    --variant-load "${vertexProfile}" \
    --geometry-manager "@falaise:${geometryResourcesSubdir}/${geometryVersion}/GeometryManager.conf" \
    --vertex-generator-manager "@falaise:${vertexResourcesSubdir}/${vertexVersion}/VertexGeneratorManager.conf" \
    --shoot \
    --vertex-generator "${vertexGenerator}" \
    --number-of-vertices 10000 \
    --prng-seed 314159 \
    --vertex-modulo 1000 \
    --output-file "vertexes.data" \
    --visu \
    --visu-spot-size "2.0 mm" \
    --visu-spot-color "green" \
    --visu-max-counts 10000 \
    --visu-view "yz" \
    --visu-object "[1100:0]" \
    --visu-output \
    --visu-output-file "vertexes-dd.data" \    
    > ${vertexGenList}
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux genvtx production failed !"
    exit 1
fi

cat >&2 <<EOF

===================
Geomtools inspector
===================

EOF

# geomVtxScript="geom.macro"
# cat > ${geomVtxScript} <<EOF
# load_display_data vertexes vertexes-dd.data
# display -xy [1100:0]
# display -3d --with-display-data --output "vertexes_on_foils.pdf" [1100:0]
# EOF
# cat ${geomVtxScript}

bxgeomtools_inspector \
    --logging "fatal" \
    --datatools::logging "fatal" \
    --datatools::resource-path "falaise@${falaiseResourcesDir}" \
    --load-dll "Falaise@${falaiseLibDir}" \
    --interactive \
    --variant-config "@falaise:${vertexResourcesSubdir}/variants/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" \
    --variant-load "${vertexProfile}" \
    --manager-config "@falaise:${geometryResourcesSubdir}/${geometryVersion}/GeometryManager.conf" 
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux geomtools inspector failed !"
    exit 1
fi

# ls -l vertexes_on_foils.pdf
# --variant-config "@falaise:${geometryResourcesSubdir}/variants/${geometryVariantVersion}/GeometryVariantRepository.conf" 
# --variant-config "@falaise:${vertexResourcesSubdir}/variants/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" 
# --variant-load "${vertexProfile}" 


exit 0
