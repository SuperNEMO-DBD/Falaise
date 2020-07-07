#!/bin/bash
#
# Author: F. Mauger
# Date:   2018-03-15
#
# Test of the PR #75 feature
# ==========================
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
#  $ ./run.sh [--work-dir /tmp/${USER}/fltest-feature-pr65] [--visu]
#

echo >&2 "[debug] PATH = '${PATH}'"

label="run-feature-pr75-validation"
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

step1=1
step2=1
step3=1

###############################################################################
if [ $step1 -eq 1 ]; then
    echo >&2 "[info] Running flsimulate-configure with HE(>2MeV) Se82 2nubb shot from the source bulk..."
    flsimulate-configure --no-gui \
			 -t "urn:snemo:demonstrator:simulation:2.2" \
			 -s "vertexes:generator=source_pads_bulk" \
			 -s "primary_events:generator=Se82.2nubb_2MeV" \
			 -s "simulation:output_profile=none" \
			 -o "${FLWORKDIR}/feature-pr75-validation.vprofile"
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate-configure failed! Abort!"
    fi

    # -s "primary_events:generator=Se82.2nubb_2MeV" \
    # flat_versatile_generator

    echo >&2 "[info] Dump '${FLWORKDIR}/feature-pr75-validation.vprofile' :"
    cat ${FLWORKDIR}/feature-pr75-validation.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu.conf -o ${FLWORKDIR}/feature-pr75-validation.xml > ${FLWORKDIR}/flsim1.log 2>&1
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate failed! Abort!"
    fi

    # Extract the energy deposited in the calo hits:
    grep --after-context=42 "<first>calo</first>" ${FLWORKDIR}/feature-pr75-validation.xml | \
	tr -s "[[:space:]]" | grep -B 1 particle_name  | grep energy_deposit | sed -e 's@^\t@@g' -e "s@<energy_deposit>@@g" -e "s@</energy_deposit>@@g" > ${FLWORKDIR}/calo_de.data
    # Compute the mean deposited energy (in keV) and compare it to a given threshold:
    mean_energy_keV=$(python ${cfg_dir}/proc.py ${FLWORKDIR}/calo_de.data )
    if [ ${mean_energy_keV} -lt 900 ]; then
	my_exit 1 "flsimulate output file does not record enough energy in 'calo' hits!"
    else
	echo >&2 "[info] flsimulate output file records 'calo' hits with high mean energy = ${mean_energy_keV} keV!"
    fi
    echo >&2 ""
fi

###############################################################################
if [ $step2 -eq 1 ]; then
    echo >&2 "[info] Running flsimulate-configure with HE electron shot from the source bulk......"
    flsimulate-configure --no-gui \
			 -t "urn:snemo:demonstrator:simulation:2.2" \
			 -s "vertexes:generator=source_pads_bulk" \
			 -s "primary_events:generator=flat_versatile_generator" \
			 -s "primary_events:generator/if_flat_versatile/particle=electron" \
			 -s "primary_events:generator/if_flat_versatile/energy_min=4 MeV" \
			 -s "primary_events:generator/if_flat_versatile/energy_max=6 MeV" \
			 -s "simulation:output_profile=all_details" \
			 -o "${FLWORKDIR}/feature-pr75-validation.vprofile"
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate-configure failed! Abort!"
    fi

    echo >&2 "[info] Dump '${FLWORKDIR}/feature-pr75-validation.vprofile' :"
    cat ${FLWORKDIR}/feature-pr75-validation.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu.conf -o ${FLWORKDIR}/feature-pr75-validation.xml > ${FLWORKDIR}/flsim2.log 2>&1
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate failed! Abort!"
    fi

    # Extract the energy deposited in the calo hits:
    grep --after-context=42 "<first>calo</first>" ${FLWORKDIR}/feature-pr75-validation.xml | \
	tr -s "[[:space:]]" | grep -B 1 particle_name  | grep energy_deposit | sed -e 's@^\t@@g' -e "s@<energy_deposit>@@g" -e "s@</energy_deposit>@@g" > ${FLWORKDIR}/calo_de.data
    # Compute the mean deposited energy (in keV) and compare it to a given threshold:
    mean_energy_keV=$(python ${cfg_dir}/proc.py ${FLWORKDIR}/calo_de.data )
    if [ ${mean_energy_keV} -lt 3500 ]; then
	my_exit 1 "flsimulate output file does not record enough energy in 'calo' hits!"
    else
	echo >&2 "[info] flsimulate output file records 'calo' hits with high mean energy = ${mean_energy_keV} keV!"
    fi
    echo >&2 ""
fi

###############################################################################
if [ $step3 -eq 1 ]; then
    echo >&2 "[info] Running flsimulate-configure with HE alpha shot towards calo blocks..."
    output_profile="none"
    if [ ${with_visu} -eq 1 ]; then
    	output_profile="all_details"
    fi
    flsimulate-configure --no-gui \
			 -t "urn:snemo:demonstrator:simulation:2.2" \
			 -s "vertexes:generator=free_spot" \
			 -s "vertexes:generator/if_free_spot/x=20 cm" \
			 -s "vertexes:generator/if_free_spot/y=10 cm" \
			 -s "vertexes:generator/if_free_spot/z=10 cm" \
			 -s "primary_events:generator=tweakable_generator" \
			 -s "primary_events:generator/if_tweakable/particle=alpha" \
			 -s "primary_events:generator/if_tweakable/energy_mode=gaussian_energy" \
			 -s "primary_events:generator/if_tweakable/energy_mode/if_gaussian/energy_mean=15000 keV" \
			 -s "primary_events:generator/if_tweakable/energy_mode/if_gaussian/energy_sigma=50 keV" \
			 -s "primary_events:generator/if_tweakable/direction_mode=cone" \
			 -s "primary_events:generator/if_tweakable/direction_mode/if_cone/axis_direction=+x" \
			 -s "primary_events:generator/if_tweakable/direction_mode/if_cone/angle_max=10 degree" \
			 -s "primary_events:generator/if_tweakable/direction_mode/if_cone/angle_min=0 degree" \
			 -s "simulation:output_profile=${output_profile}" \
			 -o "${FLWORKDIR}/feature-pr75-validation.vprofile"
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate-configure failed! Abort!"
    fi

    echo >&2 "[info] Dump '${FLWORKDIR}/feature-pr75-validation.vprofile' :"
    cat ${FLWORKDIR}/feature-pr75-validation.vprofile
    echo >&2 ""

    echo >&2 "[info] Running flsimulate..."
    flsimulate -c ${cfg_dir}/simu.conf -o ${FLWORKDIR}/feature-pr75-validation.xml > ${FLWORKDIR}/flsim3.log 2>&1
    if [ $? -ne 0 ]; then
	my_exit 1 "flsimulate failed! Abort!"
    fi

    # Extract the energy deposited in the calo hits:
    grep --after-context=42 "<first>calo</first>" ${FLWORKDIR}/feature-pr75-validation.xml | \
	tr -s "[[:space:]]" | grep -B 1 particle_name  | grep energy_deposit | sed -e 's@^\t@@g' -e "s@<energy_deposit>@@g" -e "s@</energy_deposit>@@g" > ${FLWORKDIR}/calo_de.data
    # Compute the mean deposited energy (in keV) and compare it to a given threshold:
    mean_energy_keV=$(python ${cfg_dir}/proc.py ${FLWORKDIR}/calo_de.data )
    echo >&2 "[info] 'calo' mean energy = ${mean_energy_keV} keV"
    if [ ${mean_energy_keV} -lt 9000 ]; then
	echo >&2 "[warning] flsimulate output file does not record enough energy in 'calo' hits!"
    else
	echo >&2 "[info] flsimulate output file records 'calo' hits with high mean energy = ${mean_energy_keV} keV!"
    fi
    echo >&2 ""
fi

###############################################################################
tree ${FLWORKDIR}/
if [ ${with_visu} -eq 1 ]; then
    echo >&2 "[info] Running flvisualize..."
    flvisualize \
	-i ${FLWORKDIR}/feature-pr75-validation.xml \
	--focus-on-roi \
	--show-simulated-vertex 1 \
	--show-simulated-tracks 1 \
	--show-simulated-hits 1
fi

my_exit 0

# end
