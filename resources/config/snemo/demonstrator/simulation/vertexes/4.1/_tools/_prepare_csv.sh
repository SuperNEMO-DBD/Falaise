#!/usr/bin/env bash

which bxextract_table_of_objects 1> /dev/null
if [ $? -ne 0 ]; then
    echo >&2 "ERROR: No 'bxextract_table_of_objects' utility is set!"
    exit 1
fi
#
csv_target="variants/models/vertexes_generators.csv"

if [ -f  "${csv_target}" ]; then
    # Backup:
    cp -f ${csv_target} _${csv_target}.bak
fi

rm -f ${csv_target}

# Production:

bxextract_table_of_objects -i generators/tracker_vg.def --group "Tracker" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/om_vg.def --group "OpticalModule" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/hall_vg.def --group "Hall" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/source_vg.def --group "Source" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/shielding_vg.def --group "Shielding" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/source_calibration_vg.def --group "Calibration" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/commissioning_vg.def --group "HalfCommissioning" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/misc_vg.def -G \
		      >> ${csv_target}

exit 0

# end
