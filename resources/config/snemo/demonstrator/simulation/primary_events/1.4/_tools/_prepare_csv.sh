#!/usr/bin/env bash
# to be executed from the base config directory where manager.conf is stored

which bxextract_table_of_objects 1> /dev/null
if [ $? -ne 0 ]; then
    echo >&2 "ERROR: No 'bxextract_table_of_objects' utility is set!"
    exit 1
fi
#
csv_target="variants/models/peg_generators.csv"
csv_target_bak="variants/models/_peg_generators.csv.bak"

echo >&2 "INFO: csv_target='${csv_target}'"

if [ -f "${csv_target}" ]; then
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

common_pe_dir_1_1=${resources_dir}/config/common/simulation/primary_events/1.1/
common_pe_dir_1_2=${resources_dir}/config/common/simulation/primary_events/1.2/
common_pe_dir_1_3=${resources_dir}/config/common/simulation/primary_events/1.3/
pe_dir_1_2=${opwd}/../1.2/
pe_dir_1_3=${opwd}/../1.3/


bxextract_table_of_objects -i ${common_pe_dir_1_1}/generators/dbd.def \
			   >> ${csv_target}

bxextract_table_of_objects -i ${common_pe_dir_1_1}/generators/background.def --group "Background" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${common_pe_dir_1_1}/generators/calibration.def --group "Calibration" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${common_pe_dir_1_2}/generators/dbd_more.def \
			   >> ${csv_target}

bxextract_table_of_objects -i ${common_pe_dir_1_3}/generators/more_background.def --group "Background" -G\
			   >> ${csv_target}

bxextract_table_of_objects -i ${common_pe_dir_1_3}/generators/dbd.def \
			   >> ${csv_target}

bxextract_table_of_objects -i ${pe_dir_1_2}/generators/misc.def --group "Miscellaneous" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${pe_dir_1_2}/generators/versatile.def --group "User" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${pe_dir_1_3}/generators/versatile_more.def --group "User" -G -l 10 \
			   >> ${csv_target}

echo >&2 "INFO: File '${csv_target}' has been generated!"

exit 0

# end
