#!/bin/bash
#
# Author: F. Mauger
# Date:   2025-01-13
#
# Test
# =========================
#
# This test must be run after Falaise installation and setup.
#
# Contents
# --------
#
#  * ``simu1.conf`` : main configuration file for flsimulate
#  * ``simu2.conf`` : main configuration file for flsimulate
#  * ``reco.conf ``: main configuration file for the flreconstruct program
#  * ``pipeline.conf ``: configuration file for flreconstruct pipeline
#  * ``run.sh`` : test script
#
# Run the test script
# -------------------
#
# flsimulate and flreconstruct must be in the PATH.
# cd in the directory containing the script:
#
#  $ ./run.sh [--work-dir /tmp/${USER}/tkrec_test1] [--visu]
#  $ echo $?
#

oldPwd=$(pwd)
echo >&2 "[debug] PATH = '${PATH}'"

label="flreconstruct-tkreconstruct-test1"
with_sim_visu=0
with_rec_visu=0
work_dir=""
cfg_dir=""
with_magfield=0
simuVer=1

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
	elif [ "${opt}" = "--simu1" ]; then
	    simuVer=1
	elif [ "${opt}" = "--simu2" ]; then
	    simuVer=2
	# elif [ "${opt}" = "-m" ]; then
	#     with_magfield=0
	# elif [ "${opt}" = "-M" ]; then
	#     with_magfield=1
	elif [ "${opt}" == "--sim-visu" ]; then
	    with_sim_visu=1
	elif [ "${opt}" == "--rec-visu" ]; then
	    with_rec_visu=1
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
    # if [ -d ${FLWORKDIR} ]; then
    # 	rm -fr ${FLWORKDIR}
    # fi
    cd ${oldPwd}
    exit ${error_code}
}

which flsimulate > /dev/null 2>&1
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate is not available! Abort!"
fi

if [ ! -d ${FLWORKDIR} ]; then
    mkdir -p ${FLWORKDIR}
fi
cp -f ${cfg_dir}/simu.conf ${FLWORKDIR}/simu.conf
cp -f ${cfg_dir}/simu${simuVer}.profile ${FLWORKDIR}/simu_base.profile
cp -f ${cfg_dir}/reco.conf ${FLWORKDIR}/reco.conf
cp -f ${cfg_dir}/pipeline.conf ${FLWORKDIR}/pipeline.conf

echo >&2 "[info] Preparing..."
echo >&2 "[info] FLWORKDIR='${FLWORKDIR}'"
if [ ! -d ${FLWORKDIR}/Events_visu/ ]; then
    mkdir -p  ${FLWORKDIR}/Events_visu/
fi
cd ${FLWORKDIR}
pwd

echo >&2 "[info] Running flsimulate-configure..."
# flsimulate-configure --no-gui \
#     -s "geometry:layout/if_basic/magnetic_field=${with_magfield}" \
#     -s "vertexes:generator=source_pads_bulk" \
#     -s "primary_events:generator=Se82.0nubb" \
#     -s "simulation:output_profile=all_details" \
#     -o "${FLWORKDIR}/simu.profile"
flsimulate-configure --no-gui \
		     -i "${FLWORKDIR}/simu_base.profile" \
		     -o "${FLWORKDIR}/simu.profile"
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate-configure failed! Abort!"
fi

echo >&2 "[info] Dump '${FLWORKDIR}/simu.profile' :"
cat ${FLWORKDIR}/simu.profile
echo >&2 ""

echo >&2 "[info] Running flsimulate..."
flsimulate -c ${FLWORKDIR}/simu.conf -o ${FLWORKDIR}/run_test1_sim.brio
if [ $? -ne 0 ]; then
    my_exit 1 "flsimulate failed! Abort!"
fi

if [ ${with_sim_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	--variant-profile "${FLWORKDIR}/simu.profile" \
	-i ${FLWORKDIR}/run_test1_sim.brio \
	--focus-on-roi \
	--show-simulated-vertex 1 \
	--show-simulated-tracks 1 \
	--show-simulated-hits 1
fi

echo >&2 "[info] Running flreconstruct..."
flreconstruct \
    -p ${FLWORKDIR}/reco.conf \
    -i ${FLWORKDIR}/run_test1_sim.brio \
    -o ${FLWORKDIR}/run_test1_rec.brio \
    > ${FLWORKDIR}/reco.log # 2>&1
echo >&2 ""

if [ ${with_rec_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	--variant-profile "${FLWORKDIR}/simu.profile" \
	-i ${FLWORKDIR}/run_test1_rec.brio \
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
