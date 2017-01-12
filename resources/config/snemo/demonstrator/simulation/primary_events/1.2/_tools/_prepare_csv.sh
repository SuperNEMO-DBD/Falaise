#!/usr/bin/env bash

which bxextract_table_of_objects 1> /dev/null
if [ $? -ne 0 ]; then
    echo >&2 "ERROR: No 'bxextract_table_of_objects' utility is set!"
    exit 1
fi
#
csv_target="variants/models/peg_generators.csv"
csv_target_bak="variants/models/_peg_generators.csv.bak"

echo >&2 "INFO: csv_target='${csv_target}'"

if [ -f  "${csv_target}" ]; then
    # Backup:
    cp -f ${csv_target} ${csv_target_bak}
fi

rm -f ${csv_target}

# Production:

opwd=$(pwd)
resources_dir=../../../../../..
cd ${resources_dir}
resources_dir=$(pwd)
echo >&2 "INFO: resources_dir='${resources_dir}'"
cd ${opwd}

common_pe_dir=${resources_dir}/config/common/simulation/primary_events/1.1/
this_pe_dir=${opwd}

bxextract_table_of_objects -i ${common_pe_dir}/generators/dbd.def --group "DBD" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${common_pe_dir}/generators/background.def --group "Background" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${common_pe_dir}/generators/calibration.def --group "Calibration" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${this_pe_dir}/generators/misc.def --group "Miscellaneous" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${this_pe_dir}/generators/versatile.def --group "Miscellaneous" -G \
			   >> ${csv_target}

echo >&2 "INFO: File '${csv_target}' has been generated!"

exit 0

# end
