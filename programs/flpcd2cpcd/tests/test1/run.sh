#!/usr/bin/env bash
#
# Author: F. Mauger
# Date:   2024-03-14
#
#
# This test must be run after Falaise installation and setup.
#
# Contents
# --------
#
#  * ``udd2pcd.conf`` : main configuration file for flreconstruct UDD->pCD processing
#  * ``flpcd2cpcd-test1.conf`` : main configuration file for flpcd2cpcd
#  * ``variant.profile`` : variant profile
#  * ``run.sh`` : test script
#
# Run the test script
# -------------------
#
# cd in the directory containing the script:
#
#  $ ./run.sh [--work-dir /tmp/${USER}/flpcd2cpcd-test1] [--visu]
#  $ echo $?
#

echo >&2 "[debug] PATH = '${PATH}'"

label="run-flpcd2cpcd-test1"
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

which flreconstruct > /dev/null 2>&1
if [ $? -ne 0 ]; then
    my_exit 1 "flreconstruct is not available! Abort!"
fi

which flpcd2cpcd > /dev/null 2>&1
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate is not available! Abort!"
fi

if [ ! -d ${FLWORKDIR} ]; then
    mkdir -p ${FLWORKDIR}
fi

cp -f ${cfg_dir}/variant.profile ${FLWORKDIR}/variant.profile
cp -f ${cfg_dir}/data/snemo_run-724_udd-10events.brio ${FLWORKDIR}/snemo_run-724_udd-10events.brio

echo >&2 "[info] Running flreconstruct..."
# flreconstruct \
    #     -p ${cfg_dir}/udd2pcd.conf \
    #     -i ${FLWORKDIR}/snemo_run-724_udd-20-events.brio \
    #     -o ${FLWORKDIR}/pCD.brio
flreconstruct \
    -p ${cfg_dir}/udd2pcd.conf \
    -i ${FLWORKDIR}/snemo_run-724_udd-10events.brio \
    -o ${FLWORKDIR}/pCD.brio
if [ $? -ne 0 ]; then
    my_exit 1 "flreconstruct failed! Abort!"
fi
echo >&2 ""

echo >&2 "[info] Running flpcd2cpcd..."
flpcd2cpcd \
    -V "debug" \
    -p ${cfg_dir}/udd2pcd.conf \
    -i ${FLWORKDIR}/pCD.brio \
    -o ${FLWORKDIR}/epCD.brio
if [ $? -ne 0 ]; then
    my_exit 1 "flpcd2cpcd failed! Abort!"
fi
echo >&2 ""

if [ ${with_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	-i ${FLWORKDIR}/epCD.brio \
	--focus-on-roi \
	--show-calibrated-hits 1 \
	--show-calibrated-info 1
fi

my_exit 0

# end
