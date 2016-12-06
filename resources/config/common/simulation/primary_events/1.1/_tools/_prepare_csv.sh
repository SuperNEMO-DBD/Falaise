#!/usr/bin/env bash

which bxextract_table_of_objects 1> /dev/null
if [ $? -ne 0 ]; then
    echo >&2 "ERROR: No 'bxextract_table_of_objects' utility is set!"
    exit 1
fi
#
csv_target="variants/models/peg_generators.csv"

echo >&2 "INFO: csv_target='${csv_target}'"

if [ -f  "${csv_target}" ]; then
    # Backup:
    cp -f ${csv_target} ${csv_target}.bak
fi

rm -f ${csv_target}

# Production:

bxextract_table_of_objects -i generators/misc.def --group "Miscellaneous" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/dbd.def --group "DBD" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/background.def --group "Background" -G \
		      >> ${csv_target}

bxextract_table_of_objects -i generators/calibration.def --group "Calibration" -G \
		      >> ${csv_target}

echo >&2 "INFO: File '${csv_target}' has been generated!"

exit 0

# end
