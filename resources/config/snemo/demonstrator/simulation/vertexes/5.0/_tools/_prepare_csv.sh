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

vg_dir_4_1=$(pwd)/../4.1/
vg_dir_4_2=$(pwd)/../4.2
vg_dir_5_0=$(pwd)/../5.0

# Production:

# bxextract_table_of_objects -i ${this_vg_dir}/generators/tracker_vg.def --group "Tracker" -G \
# 			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_5_0}/generators/realistic/realistic_source_pads_bulk_vg.def  --group "RealSource" -G\
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_5_0}/generators/realistic/realistic_source_pads_surface_vg.def  --group "RealSource" -G\
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_5_0}/generators/realistic/uniform_mass_realistic_source_foils_bulk_vg.def  --group "RealSource" -G\
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_5_0}/generators/realistic/uniform_realistic_source_foils_surface_vg.def  --group "RealSource" -G\
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_5_0}/generators/realistic/uniform_se82_realistic_source_foils_bulk_vg.def  --group "RealSource" -G\
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_5_0}/generators/realistic/uniform_full_realistic_source_bulk_vg.def  --group "RealSource" -G\
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_5_0}/generators/realistic/uniform_full_realistic_source_surface_vg.def  --group "RealSource" -G\
			   >> ${csv_target}


bxextract_table_of_objects -i ${vg_dir_4_2}/generators/om_pmt_bulk_vg.def  --group "OpticalModule" -G\
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_4_1}/generators/tracker_vg.def \
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_4_1}/generators/om_vg.def --group "OpticalModule" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_4_1}/generators/hall_vg.def --group "Hall" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_4_1}/generators/source_vg.def --group "Source" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_4_1}/generators/shielding_vg.def --group "Shielding" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_4_1}/generators/source_calibration_vg.def --group "Calibration" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_4_1}/generators/commissioning_vg.def --group "HalfCommissioning" -G \
			   >> ${csv_target}

bxextract_table_of_objects -i ${vg_dir_4_1}/generators/misc_vg.def -G \
			   >> ${csv_target}

echo >&2 "INFO: File '${csv_target}' has been generated!"

exit 0

# end
