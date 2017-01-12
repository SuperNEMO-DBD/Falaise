#!/usr/bin/env bash

which bxextract_table_of_objects 1> /dev/null
if [ $? -ne 0 ]; then
    echo >&2 "ERROR: No 'bxextract_table_of_objects' utility is set!"
    exit 1
fi
#
csv_target="variants/models/vertexes_generators.csv"
csv_target_bak="variants/models/_vertexes_generators.csv.bak"

if [ -f "${csv_target}" ]; then
    # Backup:
    cp -f ${csv_target} ${csv_target_bak}
fi

rm -f ${csv_target}

this_vg_dir=$(pwd)

# Production:

# bxextract_table_of_objects -i ${this_vg_dir}/generators/tracker_vg.def --group "Tracker" -G \
# 			   >> ${csv_target}

bxextract_table_of_objects -i ${this_vg_dir}/generators/tracker_vg.def \
			   >> ${csv_target}

bxextract_table_of_objects -i ${this_vg_dir}/generators/om_vg.def --group "OpticalModule" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${this_vg_dir}/generators/hall_vg.def --group "Hall" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${this_vg_dir}/generators/source_vg.def --group "Source" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${this_vg_dir}/generators/shielding_vg.def --group "Shielding" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${this_vg_dir}/generators/source_calibration_vg.def --group "Calibration" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${this_vg_dir}/generators/commissioning_vg.def --group "HalfCommissioning" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${this_vg_dir}/generators/misc_vg.def -G \
			   >> ${csv_target}

echo >&2 "INFO: File '${csv_target}' has been generated!"

exit 0

# end
