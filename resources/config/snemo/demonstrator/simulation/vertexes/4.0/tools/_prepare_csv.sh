#!/usr/bin/env bash

if [ -f  "variants/models/vertexes_generators.csv" ]; then
    # Backup:
    cp -f variants/models/vertexes_generators.csv variants/models/_vertexes_generators.csv.bak
fi

rm -f variants/models/vertexes_generators.csv

# Production:

./tools/_extract_vg.sh generators/tracker_vg.lis --group "Tracker" -G \
		      >> variants/models/vertexes_generators.csv

./tools/_extract_vg.sh generators/scin_vg.lis --group "Scintillator" -G \
		      >> variants/models/vertexes_generators.csv

./tools/_extract_vg.sh generators/hall_vg.lis --group "Hall" -G \
		      >> variants/models/vertexes_generators.csv

./tools/_extract_vg.sh --debug generators/source_vg.lis --group "Source" -G \
		      >> variants/models/vertexes_generators.csv

./tools/_extract_vg.sh generators/shield_vg.lis --group "Shielding" -G \
		      >> variants/models/vertexes_generators.csv

./tools/_extract_vg.sh generators/source_calibration_vg.lis --group "Calibration" -G \
		      >> variants/models/vertexes_generators.csv

./tools/_extract_vg.sh generators/commissioning_vg.lis --group "HalfCommissioning" -G \
		      >> variants/models/vertexes_generators.csv

exit 0
