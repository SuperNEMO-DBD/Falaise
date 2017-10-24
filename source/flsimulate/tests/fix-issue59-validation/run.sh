#!/bin/bash
#
# Author: F. Mauger
# Date:   2017-10-24
#
# Test of the issue #59 fix
# =========================
#
# This test must be run after Falaise installation and setup.
#
# Contents
# --------
#
#  * ``config/simu.conf`` : main configuration file for flsimulate
#  * ``run.sh`` : test script
#
# Run the test script
# -------------------
#
# flsimulate must be in the PATH.
# cd in the directory containing the script:
#
#  $ ./run.sh [--work-dir /tmp/${USER}/fltest-issue59] [--visu]
#

echo >&2 "[debug] PATH = '${PATH}'"

label="run-fix-issue49-validation"
with_visu=0
work_dir=""
cfg_dir=""

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
    work_dir="./_work.d"
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
    # if [ -d ${FLWORKDIR} ]; then
    # 	rm -fr ${FLWORKDIR}
    # fi
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
    -s "vertexes:generator=source_pads_surface" \
    -s "primary_events:generator=Se82.0nubb" \
    -s "simulation:output_profile=source_details" \
    -o "${FLWORKDIR}/fix-issue59-validation.vprofile"
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate-configure failed! Abort!"
fi

echo >&2 "[info] Dump '${FLWORKDIR}/fix-issue59-validation.vprofile' :"
cat ${FLWORKDIR}/fix-issue59-validation.vprofile
echo >&2 ""

echo >&2 "[info] Running flsimulate..."
flsimulate -c ${cfg_dir}/simu.conf -o ${FLWORKDIR}/fix-issue59-validation.xml
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate failed! Abort!"
fi
grep --max-count=1 "__source_SD" ${FLWORKDIR}/fix-issue59-validation.xml
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate output file contains no '__visu.track' hits from the source pads! Abort!"
else
    echo >&2 "[info] flsimulate output file contains '__visu.track' hits from the source pads!"
fi
echo >&2 ""

if [ ${with_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	-i ${FLWORKDIR}/fix-issue59-validation.xml \
	--focus-on-roi \
	--show-simulated-vertex 1 \
	--show-simulated-tracks 1 \
	--show-simulated-hits 1
fi

my_exit 0

# end
