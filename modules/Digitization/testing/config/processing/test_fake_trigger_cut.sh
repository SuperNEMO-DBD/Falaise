#!/usr/bin/env bash

echo "Starting..." >&2
PROCESSING_DIR="${FALAISE_DIGITIZATION_TESTING_DIR}/config/processing"

input_sd_data="${FALAISE_DIGITIZATION_TESTING_DIR}/data/BiPo214-source_strips_bulk_SD_100_events.brio"
output_sd_data="${FALAISE_DIGITIZATION_TESTING_DIR}/data/trigger_yes_cut.brio"

echo "Running bxdpp_processing..." >&2
bxdpp_processing  \
    -c "${PROCESSING_DIR}/module_manager.conf" \
    --load-dll "Falaise_Digitization@${FALAISE_BUILD_DIR}/BuildProducts/lib/x86_64-linux-gnu/Falaise/modules" \
    -P "trace" \
    -i "${input_sd_data}" \
    --modulo 1 \
    -m "faketriggercut" \
    --preserve-existing-files

 #    -o "${output_sd_data}" \
echo "Stopped." >&2

exit 0

# end
