#!/usr/bin/env bash

label="exaegir_test1"
step1=1
work_dir=""
cfg_dir=""
with_visu=1

function my_exit()
{
    local error_code="$1"
    shift 1
    local error_msg="$@"
    if [ -n "${error_msg}" ]; then
	echo >&2 "[error] $@"
    fi
    if [ -L"${FLWORKDIR}/_exaegir_plugin.d" ]; then
	echo >&2 "[info] remove symlink '_exaegir_plugin.d'"
    	rm -f "${FLWORKDIR}/_exaegir_plugin.d" 
    fi
     if [ -L _work.d ]; then
	echo >&2 "[info] remove symlink '_work.d'"
    	rm -f _work.d
    fi
    if [ -d ${FLWORKDIR} ]; then
	echo >&2 "[info] remove work dir '${FLWORKDIR}'"
	rm -fr ${FLWORKDIR}
    fi
    exit ${error_code}
}

#########################################
work_dir="_tmp_work.d"
cfg_dir="."
export FLWORKDIR="${work_dir}/${label}"

if [ -L _work.d ]; then
    echo >&2 "[info] Removing symlink '_work.d'"
    rm -f _work.d
fi

if [ ! -d ${FLWORKDIR} ]; then
    echo >&2 "[info] Creating work dir '${FLWORKDIR}'"
    mkdir -p ${FLWORKDIR}
    ln -s ${FLWORKDIR} _work.d
fi

echo >&2 "[info] cfg_dir   = '${cfg_dir}'"
echo >&2 "[info] work_dir  = '${work_dir}'"
echo >&2 "[info] FLWORKDIR = '${FLWORKDIR}'"

which flsimulate > /dev/null 2>&1
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate is not available! Abort!"
fi

which exaegir-config > /dev/null 2>&1
if [ $? -ne 0 ]; then
    my_exit 1 "exaegir is not available! Abort!"
fi

exaegirLibDir=$(exaegir-config --libdir)

cp ${cfg_dir}/simu_1.conf ${FLWORKDIR}/
cp ${cfg_dir}/generators.conf ${FLWORKDIR}/generators.conf

if [ ! -f ${FLWORKDIR}/generators.conf ]; then
    my_exit 1 "[error] Missing generators config file! Abort!"
fi

ln -s ${exaegirLibDir} "${FLWORKDIR}/_exaegir_plugin.d"

echo >&2 "[info] Running flsimulate-configure..."
flsimulate-configure --no-gui \
		     -M  \
		     -H  \
		     -L "exaegir@$(exaegir-config --libdir)" \
		     -s "geometry:layout/if_basic/source_layout=RealisticFlat" \
		     -s "vertexes:generator=real_flat_source_strip_3_bulk" \
		     -s "primary_events:generator=aegir" \
		     -s "primary_events:generator/if_aegir/setup_file=${FLWORKDIR}/generators.conf" \
		     -s "primary_events:generator/if_aegir/generator=dummy2" \
		     -s "simulation:output_profile=all_details" \
		     -o "${FLWORKDIR}/simu-1.vprofile"
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate-configure failed! Abort!"
fi

cat "${FLWORKDIR}/simu-1.vprofile"
 
echo >&2 "[info] Running flsimulate..."
flsimulate -c ${cfg_dir}/simu_1.conf \
	   -o ${FLWORKDIR}/test1-exaegir.brio \
	   > ${FLWORKDIR}/flsim1.log 2>&1
if [ $? -ne 0 ]; then
    cat ${FLWORKDIR}/flsim1.log
    my_exit 1 "flsimulate failed! Abort!"	
fi

if [ ${with_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	--variant-profile ${FLWORKDIR}/simu-1.vprofile \
	--input-file ${FLWORKDIR}/test1-exaegir.brio \
	--focus-on-roi \
	--show-simulated-vertex 1 \
	--show-simulated-tracks 1 \
	--show-simulated-hits 1
fi

my_exit 0
