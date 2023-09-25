#!/bin/bash
#
# Author: F. Mauger
# Date:   2023-09-19
#
# Test of the  feature
# ==========================
#
# This test must be run after Falaise installation and setup.
#
# Contents
# --------
#
#  * ``simu_X.conf`` : main configuration files for flsimulate
#  * ``run.sh`` : test script
#
# Run the test script
# -------------------
#
# flsimulate must be in the PATH.
# cd in the directory containing the script:
#
#  $ ./run.sh [--work-dir /tmp/${USER}/fltest-feature-aegir] [--visu]
#

# pythonExe=$(which python3)
# if [ -z "${pythonExe}" ]; then
#     echo >&2 "[error] Cannot find Python executable! Abort!"
#     exit 1    
# fi

echo >&2 "[debug] PATH = '${PATH}'"

label="run-feature-aegir"
with_visu=0
with_visu=1
work_dir=""
cfg_dir=""
step1=1
step2=1

function parse_cl_opts()
{
    while [ -n "$1" ]; do
	local opt="$1"
	if [ "${opt}" == "--work-dir" ]; then
	    shift 1
	    work_dir="$1"
	elif [ "${opt}" == "--cfg-dir" ]; then
	    shift 1
	    cfg_dir="$1"
	elif [ "${opt}" == "--visu" ]; then
	    with_visu=1
	else
	    echo >&2 "[error] Invalid command line option '${opt}'! Abort!"
	    return 1
	fi
	shift 1
    done
    return 0
}

parse_cl_opts $@
if [ $? -ne 0 ]; then
    exit 1
fi

if [ -z "${work_dir}" ]; then
    work_dir="_tmp_work.d"
fi
if [ -z "${cfg_dir}" ]; then
    cfg_dir="."
fi

#########################################
export FLWORKDIR="${work_dir}/${label}"

echo >&2 "[info] cfg_dir   = '${cfg_dir}'"
echo >&2 "[info] work_dir  = '${work_dir}'"
echo >&2 "[info] FLWORKDIR = '${FLWORKDIR}'"

function my_exit()
{
    local error_code="$1"
    shift 1
    local error_msg="$@"
    if [ -n "${error_msg}" ]; then
	echo >&2 "[error] $@"
    fi
    if [ -d ${FLWORKDIR} ]; then
	echo >&2 "[info] remove work dir '${FLWORKDIR}'"
	rm -fr ${FLWORKDIR}
    fi
    if [ -L _work.d ]; then
	echo >&2 "[info] remove symlink '_work.d'"
    	rm -f _work.d
    fi
    
    exit ${error_code}
}

which flsimulate > /dev/null 2>&1
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate is not available! Abort!"
fi

if [ ! -d ${FLWORKDIR} ]; then
    mkdir -p ${FLWORKDIR}
    ln -s ${FLWORKDIR} _work.d
fi

cp ${cfg_dir}/data/genCo60_1.d0t ${FLWORKDIR}/
cp ${cfg_dir}/data/genCo60_2.d0t ${FLWORKDIR}/

###############################################################################
if [ $step1 -eq 1 ]; then
    cat>&2<<EOF

    ========================================================
    STEP 1 - Basic source - AEGIR - 511 keV e+
    ========================================================
EOF
    # -t "urn:snemo:demonstrator:simulation:2.4" 
    echo >&2 "[info] Running flsimulate-configure..."
    flsimulate-configure --no-gui \
			 -s "geometry:layout/if_basic/source_layout=RealisticFlat" \
			 -s "vertexes:generator=real_flat_source_strip_3_bulk" \
			 -s "primary_events:generator=aegir" \
			 -s "primary_events:generator/if_aegir/generators_file=${FLWORKDIR}/generators.conf" \
			 -s "primary_events:generator/if_aegir/selected=positron.511keV" \
			 -s "simulation:output_profile=all_details" \
			 -o "${FLWORKDIR}/simu-1.vprofile"
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate-configure failed! Abort!"
    fi
    cp ${cfg_dir}/generators.conf ${FLWORKDIR}/generators.conf
    if [ ! -f ${FLWORKDIR}/generators.conf ]; then
	my_exit 1 "Missing generators config file! Abort!"
    fi
    # cp ${cfg_dir}/simu-1.vprofile ${FLWORKDIR}/simu-1.vprofile
    # if [ ! -f ${FLWORKDIR}/simu-1.vprofile ]; then
    # 	my_exit 1 "Missing vprofile file! Abort!"
    # fi

    echo >&2 "[info] Dump '${FLWORKDIR}/generators.conf' :"
    cat ${FLWORKDIR}/generators.conf
    echo >&2 ""

    echo >&2 "[info] Dump '${FLWORKDIR}/simu-1.vprofile' :"
    cat ${FLWORKDIR}/simu-1.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu_1.conf -o ${FLWORKDIR}/feature-aegir-1.brio > ${FLWORKDIR}/flsim1.log 2>&1
    if [ $? -ne 0 ]; then
	cat ${FLWORKDIR}/flsim1.log
	my_exit 1 "flsimulate failed! Abort!"	
    fi

    if [ ${with_visu} -eq 1 ]; then
	echo >&2 "[info] Running flvisualize..."
	flvisualize \
	    --variant-profile ${FLWORKDIR}/simu-1.vprofile \
	    --input-file ${FLWORKDIR}/feature-aegir-1.brio \
	    --focus-on-roi \
	    --show-simulated-vertex 1 \
	    --show-simulated-tracks 1 \
	    --show-simulated-hits 1
    fi
fi

###############################################################################
if [ $step2 -eq 1 ]; then
    cat>&2<<EOF

    ========================================================
    STEP 2 - Basic source - AEGIR - 511 keV e+
    ========================================================
EOF
    # -t "urn:snemo:demonstrator:simulation:2.4" 
    echo >&2 "[info] Running flsimulate-configure..."
    flsimulate-configure --no-gui \
			 -s "geometry:layout/if_basic/source_layout=RealisticFlat" \
			 -s "vertexes:generator=real_flat_source_full_foils_mass_bulk" \
			 -s "primary_events:generator=aegir" \
			 -s "primary_events:generator/if_aegir/generators_file=${FLWORKDIR}/generators.conf" \
			 -s "primary_events:generator/if_aegir/selected=precomputed" \
			 -s "simulation:output_profile=all_details" \
			 -o "${FLWORKDIR}/simu-2.vprofile"
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate-configure failed! Abort!"
    fi
    cp ${cfg_dir}/generators.conf ${FLWORKDIR}/generators.conf
    if [ ! -f ${FLWORKDIR}/generators.conf ]; then
	my_exit 1 "Missing generators config file! Abort!"
    fi

    echo >&2 "[info] Dump '${FLWORKDIR}/generators.conf' :"
    cat ${FLWORKDIR}/generators.conf
    echo >&2 ""

    echo >&2 "[info] Dump '${FLWORKDIR}/simu-2.vprofile' :"
    cat ${FLWORKDIR}/simu-2.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu_2.conf -o ${FLWORKDIR}/feature-aegir-2.brio > ${FLWORKDIR}/flsim2.log 2>&1
    if [ $? -ne 0 ]; then
	cat ${FLWORKDIR}/flsim2.log
	my_exit 1 "flsimulate failed! Abort!"	
    fi

    if [ ${with_visu} -eq 1 ]; then
	echo >&2 "[info] Running flvisualize..."
	flvisualize \
	    --variant-profile ${FLWORKDIR}/simu-2.vprofile \
	    --input-file ${FLWORKDIR}/feature-aegir-2.brio \
	    --focus-on-roi \
	    --show-simulated-vertex 1 \
	    --show-simulated-tracks 1 \
	    --show-simulated-hits 1
    fi
fi

###############################################################################
tree ${FLWORKDIR}/

my_exit 0

# end
