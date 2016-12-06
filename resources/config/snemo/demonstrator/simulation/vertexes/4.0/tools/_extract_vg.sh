#!/usr/bin/env bash


function usage()
{
    cat<<EOF
extract_vg.sh - Extract a list of primary vertex generators with
                associated informations from definition files.

Usage: extract_vg.sh [OPTIONS]...[FILE]

Options:

   --help                  Print help then exit
   --debug                 Debug print
   -i, --input FILE        Generators definition input file
   -g, --group NAME        Group to be associated to the generators
                           unless already specified from the file
   -G, --ignore-file-group Ignore group specified from the file

EOF
    return 0
}

debug=0
generator_def_file=
requested_group=
ignore_file_group=0
ignore_file_variant=0

while [ -n "$1" ]; do
    token="$1"
    if [ "${token:0:1}" == "-" ]; then
	opt="${token}"
	if [ ${opt} == "--help" ]; then
	    usage
	    exit 0
	elif [ ${opt} == "--debug" ]; then
	    debug=1
	elif [ ${opt} == "--input" -o ${opt} == "-i" ]; then
	    shift 1
	    generator_def_file="$1"
	elif [ ${opt} == "--group" -o ${opt} == "-g" ]; then
	    shift 1
	    requested_group="$1"
	elif [ ${opt} == "--ignore-file-group" -o ${opt} == "-G" ]; then
	    ignore_file_group=1
	elif [ ${opt} == "--ignore-file-variant" -o ${opt} == "-V" ]; then
	    ignore_file_variant=1
	else
	    echo >&2 "ERROR: Unsupported option '${opt}'!"
	    exit 1
	fi
    else
	arg="${token}"
	if [ "x${generator_def_file}" == "x" ]; then
	    generator_def_file=${arg}
	else
	    echo >&2 "ERROR: Unsupported argument '${arg}'!"
	    exit 1
	fi
    fi
    shift 1
done

test ${debug} -ne 0 && echo >&2 "DEBUG: Find vertex generator names in file '${generator_def_file}'"
vg_names=$(cat ${generator_def_file} | grep -e "^\[name=" | grep -e "type=" | cut -d' ' -f1 | cut -d'=' -f2 | tr -d \")
for vg_name in ${vg_names}; do
    test ${debug} -ne 0 && echo >&2 "DEBUG: Name = '${vg_name}'"
    vg_config=$(grep -e "^\[name=\"${vg_name}\"" -A 5 ${generator_def_file} | grep ^#@config | cut -d' ' -f2-)
    test ${debug} -ne 0 && echo >&2 "DEBUG: Config = '${vg_config}'"
    vg_documentation=$(echo "${vg_config};" | cut -d';' -f1)
    test ${debug} -ne 0 && echo >&2 "DEBUG: Documentation = '${vg_documentation}'"
    vg_group=""
    if [ ${ignore_file_group} -eq 0 ]; then
	xg=$(echo "${vg_config};" | sed -e "s|;|\n|g" | grep ^group= | head -1)
	test ${debug} -ne 0 && echo >&2 "DEBUG: xg = '${xg}'"
	if [ "x${xg}" != "x" ]; then
	    vg_group=$(echo ${xg} | sed -e "s|^group=||g")
	fi
	test ${debug} -ne 0 && echo >&2 "DEBUG: File group = '${vg_group}'"
    elif [ "x${requested_group}" != "x" ]; then
	test ${debug} -ne 0 && echo >&2 "DEBUG: Requested group = '${requested_group}'"
	vg_group="${requested_group}"
    fi
    vg_variant=""
    if [ ${ignore_file_variant} -eq 0 ]; then
	xg=$(echo "${vg_config};" | sed -e "s|;|\n|g" | grep ^variant= | head -1)
	test ${debug} -ne 0 && echo >&2 "DEBUG: xg = '${xg}'"
	if [ "x${xg}" != "x" ]; then
	    vg_variant=$(echo ${xg} | sed -e "s|^variant=||g")
	fi
	test ${debug} -ne 0 && echo >&2 "DEBUG: File variant = '${vg_variant}'"
    fi
    test ${debug} -ne 0 && echo >&2 "DEBUG: Group = '${vg_group}'"
    echo "${vg_name} : ${vg_documentation} : ${vg_group} : ${vg_variant}"
done

exit 0

# end
