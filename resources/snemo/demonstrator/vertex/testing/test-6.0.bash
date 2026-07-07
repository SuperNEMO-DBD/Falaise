#!/usr/bin/env bash

# vertexGenerator="calo_curtain_bulk"
# vertexGenerator="calo_curtain_surface_front"
vertexGenerator="real_snrs1_source_full_foils_surface"

#vertexGenerator="real_snrs1_source_strip_3_pad_0_bulk_test"
#vertexGenerator="real_snrs1_source_strip_3_pad_0_surface_back_test"
 
if [ "x$1" != "x" ]; then
    vertexGenerator="$1"
    shift 1
fi
nb_vertexes=10000
nb_vertexes_max_counts=10000

nb_vertexes=10000
nb_vertexes_max_counts=10000
visuSpotSize="0.005 mm"
visuSpotSize="0.020 mm"
# visuSpotSize="1.0 mm"
vertexColor="red"
if [ "x$1" != "x" ]; then
    vertexColor="$1"
    shift 1
fi

vertexLabel=""

if [ "x$1" != "x" ]; then
    vertexLabel="$1"
    shift 1
else
    vertexLabel="test"
fi

vertexData="vertexes-${vertexLabel}.data"
vertexDdData="vertexes-dd-${vertexLabel}.data"
vertexLog="gen_vertex_prod-${vertexLabel}.log"
do_visu=1

if [ "x$1" != "x" ]; then
    if [ "$1" == "novisu" ]; then
	do_visu=0
    fi
fi
echo >&2 "[info] vertexGenerator = '${vertexGenerator}'"
echo >&2 "[info] vertexColor = '${vertexColor}'"
echo >&2 "[info] vertexData = '${vertexData}'"
echo >&2 "[info] vertexDdData = '${vertexDdData}'"

### exit 0


geometryVersion="6.0"
geometryVariantVersion="4.0"
vertexVersion="6.0"
vertexVariantVersion="3.0"
falaiseResourcesDir="${geometryResourcesDir}/../../.."
if [ ! -d "/tmp/${USER}/flgeometry/" ]; then
    mkdir "/tmp/${USER}/flgeometry/"
fi
geometryProfile="/tmp/${USER}/flgeometry/geometry.profile"
origPwd="$(pwd)"
if [ ! -d ${falaiseResourcesDir} ]; then
    echo >&2 "[error] Directory does not exist '${falaiseResourcesDir}' !"
    cd ${origPwd}
    exit 1
fi
cd ${falaiseResourcesDir}
falaiseResourcesDir="$(pwd)"
cd ${origPwd}

geomSourceLayout="RealisticFlat"
geomSourceLayout="RealisticSNRS1"
if [ "x${vertexGenerator}" = "x" ]; then
    if [ ${geomSourceLayout} = "RealisticFlat" ]; then
	vertexGenerator="real_flat_source_full_foils_surface"
    elif [ ${geomSourceLayout} = "RealisticSNRS1" ] ; then
	vertexGenerator="real_snrs1_source_full_foils_surface"
	vertexGenerator="real_snrs1_source_strip_3_pad_0_bulk"
   else
	vertexGenerator="free_spot"
    fi
fi

vertexResourcesDir="$(pwd)"
falaiseResourcesDir="${vertexResourcesDir}/../../.."
vertexProfileLoad="$(pwd)/variants/${vertexVariantVersion}/testing/vertexes-1.profile"
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

libLabel="lib"
libLabelAlt="lib64"
if [ -d "/sps/nemo/sw/redhat-9-x86_64/snsw/var/BxWork/falaise-test/build.d/BuildProducts" ]; then
    echo >&2 "[info] Loading special test development stuff at CC..."
    falaiseBuildDir="/sps/nemo/sw/redhat-9-x86_64/snsw/var/BxWork/falaise-test/build.d/BuildProducts"
    libLabel="lib64"
    libLabelAlt="lib"
else if [ -d "/opt/SW/SuperNEMO-DBD/Falaise/_build-dev.d/BuildProducts" ]; then
	 echo >&2 "[info] Loading frc's special development stuff..."
	 falaiseBuildDir="/opt/SW/SuperNEMO-DBD/Falaise/_build-dev.d/BuildProducts"
     fi
fi
if [ ! -d ${falaiseBuildDir} ]; then
    echo >&2 "[warning] Falaise build directory does not exist '${falaiseBuildDir}' ! Ask for user input..."
    read -p "Enter Falaise build directory: "
    falaiseBuildDir="${REPLY}"

    # Example at CC: /sps/nemo/sw/redhat-9-x86_64/snsw/var/BxWork/falaise-5.1.13b-test/build.d/BuildProducts

fi
if [ ! -d ${falaiseBuildDir} ]; then
    echo >&2 "[error] Falaise build directory does not exist '${falaiseBuildDir}' !"
    exit 1
fi
cd ${falaiseBuildDir}
falaiseBuildDir="$(pwd)"
cd ${origPwd}

falaiseLibDir="${falaiseBuildDir}/${libLabel}"
if [ ! -d ${falaiseLibDir} ]; then
    echo >&2 "[warning] Falaise lib directory does not exist '${falaiseLibDir}' !"
    falaiseLibDir="${falaiseBuildDir}/${libLabelAlt}"
    if [ ! -d ${falaiseLibDir} ]; then
	echo >&2 "[error] Falaise lib directory does not exist '${falaiseLibDir}' !"
	exit 1
    fi
fi
cd ${falaiseLibDir}
falaiseLibDir="$(pwd)"
cd ${origPwd}

echo >&2 "[info] geometryVersion         = '${geometryVersion}'"
echo >&2 "[info] geometryVariantVersion  = '${geometryVariantVersion}'"
echo >&2 "[info] vertexVersion           = '${vertexVersion}'"
echo >&2 "[info] vertexResourcesSubdir   = '${vertexResourcesSubdir}'"
echo >&2 "[info] falaiseResourcesDir     = '${falaiseResourcesDir}'"
echo >&2 "[info] falaiseBuildDir         = '${falaiseBuildDir}'"
echo >&2 "[info] falaiseLibDir           = '${falaiseLibDir}'"
# exit 0

###--variant-load="${vertexProfileLoad}" 

if [ -f ${vertexProfile} ]; then
    rm -f ${vertexProfile}
fi

cat<<EOF

=================
Variant inspector
=================

EOF
bxvariant_inspector \
    --datatools::resource-path="falaise@${falaiseResourcesDir}" \
    --logging="debug" \
    --variant-config="@falaise:${vertexResourcesSubdir}/variants/service/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" \
    --variant-gui \
    --variant-set="geometry:layout/if_basic/source_layout=${geomSourceLayout}" \
    --variant-set="vertexes:generator=${vertexGenerator}" \
    --variant-ui-writable-at-start \
    --variant-store="${vertexProfile}"
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux variant inspector failed !"
    exit 1
fi

echo >&2 "[info] Variant file:"
cat ${vertexProfile}

# exit 0
cat<<EOF

===================
Genvtx production
===================

EOF

function gen_vertex_list()
{
    bxgenvtx_production \
	--logging "debug" \
	--load-dll "Falaise@${falaiseLibDir}" \
	--datatools::logging "fatal" \
	--datatools::resource-path "falaise@${falaiseResourcesDir}" \
	--variant-config "@falaise:${vertexResourcesSubdir}/variants/service/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" \
	--variant-load "${vertexProfile}" \
	--geometry-manager "@falaise:${geometryResourcesSubdir}/${geometryVersion}/GeometryManager.conf" \
	--vertex-generator-manager "@falaise:${vertexResourcesSubdir}/${vertexVersion}/VertexGeneratorManager.conf" \
	--list \
	> ${vertexGenList} 2> "list-${vertexLog}"
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux genvtx production failed !"
	return 1
    fi
    return 0
}
gen_vertex_list
if [ $? -ne 0 ]; then
    echo >&2 "[error] gen_vertex_list failed !"
    exit 1
fi

nbVertexGenerators=$(cat ${vertexGenList} | wc -l)
echo >&2 "[info] Found vertex generators = ${nbVertexGenerators}"
# if [ ${nbVertexGenerators} -ne 477 ]; then
#     echo >&2 "[error] Unexpected number of vertex generators for version ${vertexVersion} !"
#     exit 1
# fi

### exit 0

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
	# echo >&2 "============== _vertexDescAdd1='${_vertexDescAdd1}'"	
	# echo >&2 "============== _vertexDescAdd2='${_vertexDescAdd2}'"	
	# echo >&2 "============== _vertexDescAdd3='${_vertexDescAdd3}'"	
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
	# echo >&2 "============== _vertexDescAdds='${_vertexDescAdds[@]}'"	
	for _vertexDescAdd in ${_vertexDescAdds[@]} ; do
   	    # echo >&2 "============== processing _vertexDescAdd='${_vertexDescAdd}'"	
	    echo "${_vertexDescAdd}" | grep group= > /dev/null
	    if [ $? -eq 0 ]; then
 		_vertexGenGroup=$(echo "${_vertexDescAdd}" | sed -e 's/group=//g') 	
 		# echo >&2 "============== Found group='${_vertexGenGroup}'!"	
	    else
		echo "${_vertexDescAdd}" | grep rank= > /dev/null
		if [ $? -eq 0 ]; then
		    _vertexGenRank=$(echo "${_vertexDescAdd}" | sed -e 's/rank=//g') 	
		    # echo >&2 "============== Found rank='${_vertexGenRank}'!"	
		else	
		    echo "${_vertexDescAdd}" | grep variant= > /dev/null
		    if [ $? -eq 0 ]; then
			_vertexGenVariant=$(echo "${_vertexDescAdd}" | sed -e 's/variant=//g') 	
			# echo >&2 "============== Found variant='${_vertexGenVariant}'!"	
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
   	    # echo >&2 "  ==========> update lenNameMax=[${_lenNameMax}] from [${_lenName}]" 
	fi
	if [ ${_lenDesc} -gt ${_lenDescMax} ]; then
	    _lenDescMax=${_lenDesc}
   	    # echo >&2 "  ==========> update lenDescMax=[${_lenDescMax}] from [${_lenDesc}]" 
	fi
	if [ ${_lenGroup} -gt ${_lenGroupMax} ]; then
	    _lenGroupMax=${_lenGroup}
   	    # echo >&2 "  ==========> update lenGroupMax=[${_lenGroupMax}] from [${_lenGroup}]" 
	fi
	if [ ${_lenRank} -gt ${_lenRankMax} ]; then
	    _lenRankMax=${_lenRank}
   	    # echo >&2 "  ==========> update lenRankMax=[${_lenRankMax}] from [${_lenRank}]" 
	fi
	if [ ${_lenVariant} -gt ${_lenVariantMax} ]; then
	    _lenVariantMax=${_lenVariant}
   	    # echo >&2 "  ==========> update lenVariantMax=[${_lenVariantMax}] from [${_lenVariant}]" 
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
	# echo >&2 "  ==========> second processing line #${_lineCount}" 
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

# gen_vertex_list_rst

# exit 0

#######################

function gen_vertex_prod()
{
    
    bxgenvtx_production \
	--logging "fatal" \
	--load-dll "Falaise@${falaiseLibDir}" \
	--datatools::logging "fatal" \
	--datatools::resource-path "falaise@${falaiseResourcesDir}" \
	--variant-config "@falaise:${vertexResourcesSubdir}/variants/service/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" \
	--variant-load "${vertexProfile}" \
	--geometry-manager "@falaise:${geometryResourcesSubdir}/${geometryVersion}/GeometryManager.conf" \
	--vertex-generator-manager "@falaise:${vertexResourcesSubdir}/${vertexVersion}/VertexGeneratorManager.conf" \
	--shoot \
	--vertex-generator "${vertexGenerator}" \
	--number-of-vertices ${nb_vertexes} \
	--prng-seed 314159 \
	--vertex-modulo 1000 \
	--output-file "${vertexData}" \
	--visu \
	--visu-spot-size "${visuSpotSize}" \
	--visu-spot-color "${vertexColor}" \
	--visu-max-counts  ${nb_vertexes_max_counts} \
	--visu-view "yz" \
	--visu-object "[1000:0]" \
	--visu-output \
	--visu-output-file "${vertexDdData}" \
	> ${vertexGenList} 2> "${vertexLog}"
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux genvtx production failed !"
	return 1
    fi
    return 0
}

do_prod=0
if [ ! -f ${vertexData} -o ! -f ${vertexDdData} ]; then
    do_prod=1
fi

if [ ${do_prod} -eq 1 ]; then
    gen_vertex_prod
    if [ $? -ne 0 ]; then
	echo >&2 "[error] prod failed !"
	exit 1
    fi
fi

### exit 0


#######################
function prod_visu()
{
    bxgenvtx_production \
	--logging "fatal" \
	--load-dll "Falaise@${falaiseLibDir}" \
	--datatools::logging "fatal" \
	--datatools::resource-path "falaise@${falaiseResourcesDir}" \
	--variant-config "@falaise:${vertexResourcesSubdir}/variants/service/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" \
	--variant-load "${vertexProfile}" \
	--geometry-manager "@falaise:${geometryResourcesSubdir}/${geometryVersion}/GeometryManager.conf" \
	--vertex-generator-manager "@falaise:${vertexResourcesSubdir}/${vertexVersion}/VertexGeneratorManager.conf" \
	--shoot \
	--vertex-generator "${vertexGenerator}" \
	--number-of-vertices 1000000 \
	--prng-seed 314159 \
	--vertex-modulo 10000 \
	--output-file "${vertexData}" \
	--visu \
	--visu-spot-size "0.025 mm" \
	--visu-spot-color "${vertexColor}" \
	--visu-max-counts 1000000 \
	--visu-view "yz" \
	--visu-object "[1000:0]" \
	--visu-output \
	--visu-output-file "${vertexDdData}" \    
    > ${vertexGenList}
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux genvtx production failed !"
	return 1
    fi
    return 0
}


cat >&2 <<EOF

===================
Geomtools inspector
===================

EOF

# geomVtxScript="geom.macro"
# cat > ${geomVtxScript} <<EOF
# load_display_data vertexes vertexes-dd.data
# display -xy [1100:0]
# display -xy [1000:0]
# display -3d --with-display-data --output "vertexes_on_foils.pdf" [1100:0]
# EOF
# cat ${geomVtxScript}

function gen_vertex_visu()
{
    bxgeomtools_inspector \
	--logging "fatal" \
	--datatools::logging "fatal" \
	--datatools::resource-path "falaise@${falaiseResourcesDir}" \
	--load-dll "Falaise@${falaiseLibDir}" \
	--interactive \
	--variant-config "@falaise:${vertexResourcesSubdir}/variants/service/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" \
	--variant-load "${vertexProfile}" \
	--manager-config "@falaise:${geometryResourcesSubdir}/${geometryVersion}/GeometryManager.conf" 
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux geomtools inspector failed !"
	return 1
    fi
    return 0
}

if [ ${do_visu} -eq 1 ]; then
    gen_vertex_visu
    if [ $? -ne 0 ]; then
	echo >&2 "[error] gen_vertex_visu failed !"
	exit 1
    fi
fi

# ls -l vertexes_on_foils.pdf
# --variant-config "@falaise:${geometryResourcesSubdir}/variants/${geometryVariantVersion}/GeometryVariantRepository.conf" 
# --variant-config "@falaise:${vertexResourcesSubdir}/variants/service/${vertexVariantVersion}/VertexGeneratorVariantRepository.conf" 
# --variant-load "${vertexProfile}" 


exit 0
