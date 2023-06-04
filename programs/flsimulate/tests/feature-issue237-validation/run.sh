#!/bin/bash
#
# Author: F. Mauger
# Date:   2021-09-20
#
# Test of the issue #237 feature
# ===============================
#
# This test must be run after Falaise installation and setup.
#
# Contents
# --------
#
#  * ``simu.conf`` : main configuration file for flsimulate
#  * ``run.sh`` : test script
#
# Run the test script
# -------------------
#
# flsimulate must be in the PATH.
# cd in the directory containing the script:
#
#  $ ./run.sh [--work-dir /tmp/${USER}/fltest-feature-issue237] [--visu]
#

echo >&2 "[debug] PATH = '${PATH}'"

label="run-feature-issue237-validation"
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
    work_dir="_work.d"
fi
if [ -z "${cfg_dir}" ]; then
    cfg_dir="$(pwd)"
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
	echo >&2 "[info] Clear the work dir..."
    	# rm -fr ${FLWORKDIR}
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

step1=1

###############################################################################
if [ $step1 -eq 1 ]; then
    echo >&2 "[info] Running flsimulate-configure with HE(>2MeV) Se82 2nubb shot from the source bulk..."
    cp ${cfg_dir}/snemo_geom_test_sds.vprofile "${FLWORKDIR}/feature-issue237-validation.vprofile"
    
    echo >&2 "[info] Dump '${FLWORKDIR}/feature-pr75-validation.vprofile' :"
    cat ${FLWORKDIR}/feature-issue237-validation.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu.conf -o ${FLWORKDIR}/feature-issue237-validation.xml > ${FLWORKDIR}/flsim1.log \
	       2> ${FLWORKDIR}/flsimulate.err
    if [ $? -ne 0 ]; then
	cat ${FLWORKDIR}/flsimulate.err
	my_exit 1 "flsimulate failed! Abort!"
    fi

    # Extract the vertexes generated in the calibration sources:
    grep --after-context=3 "<vertex class_id=" ${FLWORKDIR}/feature-issue237-validation.xml | \
	tr -s "[[:space:]]" | grep -E "(<x>|<y>|<z>)"  | \
	sed -e 's@^\t@@g' -e "s@<x>@@g" -e "s@</x>@ @g" -e "s@<y>@@g" -e "s@</y>@ @g" -e "s@<z>@@g" | tr -d '\n' |  sed -e "s@</z>@\n@g" \
	> ${FLWORKDIR}/vertexes.data
    echo >&2 ""
fi

###############################################################################
tree ${FLWORKDIR}/
if [ ${with_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	-i ${FLWORKDIR}/feature-issue237-validation.xml \
	--focus-on-roi \
	--show-simulated-vertex 1 \
	--show-simulated-tracks 1 \
	--show-simulated-hits 1
    echo >&2 "[info] Plot generated vertexes..."
    cd ${FLWORKDIR}
    gnuplot ${cfg_dir}/plot.gp
fi

my_exit 0

# end
