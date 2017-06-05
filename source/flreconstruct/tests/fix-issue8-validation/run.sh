#!/bin/bash
#
# Author: F. Mauger
# Date:   2017-06-04
#
# Test of the issue #8 fix
# =========================
#
# This test must be run after Falaise installation and setup.
#
# Contents
# --------
#
#  * ``config/simu.conf`` : main configuration file for flsimulate
#  * ``config/reco.conf ``: main configuration file for flreconstruct
#  * ``run.sh`` : test script
#
# Run the test script
# -------------------
#
# flsimulate and flreconstruct must be in the PATH.
# cd in the directory containing the script:
#
#  $ ./run.sh [--work-dir /tmp/${USER}/fltest-issue8] [--visu]
#  $ echo $?
#

echo >&2 "[debug] PATH = '${PATH}'"

label="run-fix-issue8-validation"
with_visu=0
work_dir=""
cfg_dir=""
with_magfield=1

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
	elif [ "${arg}" = "-m" ]; then
	    with_magfield=0
	elif [ "${arg}" = "-M" ]; then
	    with_magfield=1
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
    work_dir="_work.d"
fi
if [ -z "${cfg_dir}" ]; then
    cfg_dir="."
fi

#########################################
export FLWORKDIR="${work_dir}/${label}"

echo >&2 "[info] cfg_dir   = '${cfg_dir}'"
echo >&2 "[info] work_dir  = '${work_dir}'"
echo >&2 "[info] FLWORKDIR = '${FLWORKDIR}'"
echo >&2 "[info] with_magfield = '${with_magfield}'"

function my_exit()
{
    local error_code="$1"
    shift 1
    local error_msg="$@"
    if [ -n "${error_msg}" ]; then
	echo >&2 "[error] $@"
    fi
    if [ -d ${FLWORKDIR} ]; then
	rm -fr ${FLWORKDIR}
    fi
    exit ${error_code}
}

which flsimulate > /dev/null 2>&1
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate is not available! Abort!"
fi

if [ ! -d ${FLWORKDIR} ]; then
    mkdir -p ${FLWORKDIR}
fi

echo >&2 "[info] Running flsimulate-configure..."
flsimulate-configure --no-gui \
    -t "urn:snemo:demonstrator:simulation:2.1" \
    -s "geometry:layout/if_basic/magnetic_field=${with_magfield}" \
    -s "vertexes:generator=source_pads_bulk" \
    -s "primary_events:generator=Se82.0nubb" \
    -s "simulation:output_profile=all_details" \
    -o "${FLWORKDIR}/fix-issue8-validation.vprofile"
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate-configure failed! Abort!"
fi

echo >&2 "[info] Dump '${FLWORKDIR}/fix-issue8-validation.vprofile' :"
cat ${FLWORKDIR}/fix-issue8-validation.vprofile
echo >&2 ""

echo >&2 "[info] Running flsimulate..."
flsimulate -c ${cfg_dir}/simu.conf -o ${FLWORKDIR}/fix-issue8-validation.xml
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate failed! Abort!"
fi

echo >&2 "[info] Running flreconstruct..."
flreconstruct \
    -p ${cfg_dir}/reco.conf \
    -i ${FLWORKDIR}/fix-issue8-validation.xml \
    -o ${FLWORKDIR}/fix-issue8-validation-rec.xml
echo >&2 ""

grep --max-count=1 "helix_trajectory_pattern" ${FLWORKDIR}/fix-issue8-validation-rec.xml
if [ $? -ne 0 ]; then
    echo >&2 "[info] flreconstruct output file contains no 'helix' tracks!"
    exit 1
else
    echo >&2 "[info] flreconstruct output file contains 'helix' tracks!"
fi
echo >&2 ""

if [ ${with_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	-i ${FLWORKDIR}/fix-issue8-validation-rec.xml \
	--focus-on-roi \
	--show-simulated-vertex 1 \
	--show-simulated-tracks 1 \
	--show-simulated-hits 1 \
	--show-calibrated-hits 1 \
	--show-calibrated-info 1 \
	--show-tracker-clustered-hits 1 \
	--show-tracker-trajectories 1 \
	--show-particle-tracks 1
fi

my_exit 0

# end
