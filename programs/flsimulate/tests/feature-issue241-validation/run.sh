#!/bin/bash
#
# Author: F. Mauger
# Date:   2022-05-04
#
# Test of the issue #241 feature
# ==========================
#
# This test must be run after Falaise installation and setup.
#
# Contents
# --------
#
#  * ``simu_X.conf`` : main configuration files for flsimulate
#  * ``reco_Y.conf`` : main configuration files for flreconstruct
#  * ``run.sh`` : test script
#
# Run the test script
# -------------------
#
# flsimulate must be in the PATH.
# cd in the directory containing the script:
#
#  $ ./run.sh [--work-dir /tmp/${USER}/fltest-feature-241] [--visu]
#

pythonExe=$(which python3)
if [ -z "${pythonExe}" ]; then
    echo >&2 "[error] Cannot find Python executable! Abort!"
    exit 1    
fi

echo >&2 "[debug] PATH = '${PATH}'"

label="run-feature-issue241-validation"
with_visu=0
work_dir=""
cfg_dir=""
step1=1
step2=1
step3=1

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

###############################################################################
if [ $step1 -eq 1 ]; then
    cat>&2<<EOF

    ========================================================
    STEP 1 - RealisticFlat - LAPP-ITEP-3
    ========================================================
EOF
    echo >&2 "[info] Running flsimulate-configure..."
    flsimulate-configure --no-gui \
			 -t "urn:snemo:demonstrator:simulation:2.2" \
			 -s "geometry:layout/if_basic/source_layout=RealisticFlat" \
			 -s "vertexes:generator=real_flat_source_strip_3_bulk" \
			 -s "primary_events:generator=electron.1MeV" \
			 -s "simulation:output_profile=none" \
			 -o "${FLWORKDIR}/feature-issue241-validation-a.vprofile"
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate-configure failed! Abort!"
    fi

    echo >&2 "[info] Dump '${FLWORKDIR}/feature-issue241-validation-a.vprofile' :"
    cat ${FLWORKDIR}/feature-issue241-validation-a.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu_a.conf -o ${FLWORKDIR}/feature-issue241-validation-a.xml > ${FLWORKDIR}/flsim1.log 2>&1
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate failed! Abort!"
    fi

    if [ ${with_visu} -eq 1 ]; then
	echo >&2 "[info] Running flvisualize..."
	flvisualize \
	    --variant-profile ${FLWORKDIR}/feature-issue241-validation-a.vprofile \
	    --input-file ${FLWORKDIR}/feature-issue241-validation-a.xml \
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
    STEP 2 - RealisticSNRS1 - LAPP-ITEP-3
    ========================================================
EOF
    echo >&2 "[info] Running flsimulate-configure..."
    flsimulate-configure --no-gui \
			 -t "urn:snemo:demonstrator:simulation:2.2" \
			 -s "geometry:layout/if_basic/source_layout=RealisticSNRS1" \
			 -s "vertexes:generator=real_snrs1_source_strip_3_bulk" \
			 -s "primary_events:generator=electron.1MeV" \
			 -s "simulation:output_profile=none" \
			 -o "${FLWORKDIR}/feature-issue241-validation-b.vprofile"
   if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate-configure failed! Abort!"
    fi

    echo >&2 "[info] Dump '${FLWORKDIR}/feature-issue241-validation-b.vprofile' :"
    cat ${FLWORKDIR}/feature-issue241-validation-b.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu_b.conf -o ${FLWORKDIR}/feature-issue241-validation-b.xml > ${FLWORKDIR}/flsim2.log 2>&1
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate failed! Abort!"
    fi

    echo >&2 "[info] Running flreconstruct..."
    flreconstruct -p ${cfg_dir}/reco_b.conf -i ${FLWORKDIR}/feature-issue241-validation-b.xml -o ${FLWORKDIR}/feature-issue241-validation-reco-b.xml
    if [ $? -ne 0 ]; then
	my_exit 1 "flreconstruct failed! Abort!"
    fi

    if [ ${with_visu} -eq 1 ]; then
	echo >&2 "[info] Running flvisualize..."
	flvisualize \
	    --variant-profile ${FLWORKDIR}/feature-issue241-validation-b.vprofile \
	    --input-file ${FLWORKDIR}/feature-issue241-validation-reco-b.xml \
	    --focus-on-roi \
	    --show-simulated-vertex 1 \
	    --show-simulated-tracks 1 \
	    --show-simulated-hits 1
    fi
fi

###############################################################################
if [ $step3 -eq 1 ]; then
    cat>&2<<EOF

    ========================================================
    STEP 3 - RealisticSNRS1 - LAPP-ITEP-3 - 
    ========================================================
EOF
    echo >&2 "[info] Running flsimulate-configure..."
    echo >&2 "[info] Running flsimulate-configure with HE alpha shot towards calo blocks..."
    output_profile="none"
    if [ ${with_visu} -eq 1 ]; then
    	output_profile="all_details"
    fi

    echo >&2 "[info] Running flsimulate-configure..."
    flsimulate-configure --no-gui \
			 -t "urn:snemo:demonstrator:simulation:2.2" \
			 -s "geometry:layout/if_basic/magnetic_field=false" \
			 -s "geometry:layout/if_basic/source_layout=RealisticSNRS1" \
			 -s "vertexes:generator=real_snrs1_source_strip_3_bulk" \
			 -s "primary_events:generator=electron.1MeV" \
			 -s "simulation:output_profile=none" \
			 -o "${FLWORKDIR}/feature-issue241-validation-c.vprofile"
   if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate-configure failed! Abort!"
    fi

    echo >&2 "[info] Dump '${FLWORKDIR}/feature-issue241-validation-c.vprofile' :"
    cat ${FLWORKDIR}/feature-issue241-validation-c.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu_c.conf -o ${FLWORKDIR}/feature-issue241-validation-c.xml > ${FLWORKDIR}/flsim3.log 2>&1
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate failed! Abort!"
    fi

    echo >&2 "[info] Running flreconstruct..."
    flreconstruct -p ${cfg_dir}/reco_c.conf -i ${FLWORKDIR}/feature-issue241-validation-c.xml -o ${FLWORKDIR}/feature-issue241-validation-reco-c.xml
    if [ $? -ne 0 ]; then
	my_exit 1 "flreconstruct failed! Abort!"
    fi

    if [ ${with_visu} -eq 1 ]; then
	echo >&2 "[info] Running flvisualize..."
	flvisualize \
	    --variant-profile ${FLWORKDIR}/feature-issue241-validation-c.vprofile \
	    --input-file ${FLWORKDIR}/feature-issue241-validation-reco-c.xml \
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
