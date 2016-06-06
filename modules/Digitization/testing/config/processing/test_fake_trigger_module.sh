#!/usr/bin/env bash

echo "Starting..." >&2
PROCESSING_DIR="${FALAISE_DIGITIZATION_TESTING_DIR}/config/processing"

input_sd_data="${FALAISE_DIGITIZATION_TESTING_DIR}/data/BiPo214-source_strips_bulk_SD_10_events.brio"
output_sd_data="${FALAISE_DIGITIZATION_TESTING_DIR}/data/out2.brio"

echo "Running bxdpp_processing..." >&2
bxdpp_processing  \
    -c "${PROCESSING_DIR}/module_manager.conf" \
    --load-dll "Falaise_Digitization@${FALAISE_BUILD_DIR}/BuildProducts/lib/x86_64-linux-gnu/Falaise/modules" \
    -P "information" \
    -i "${input_sd_data}" \
    --modulo 1 \
    -m "faketrig" \
    -o "${output_sd_data}" \
    --preserve-existing-files

echo "Stopped." >&2

exit 0

# end

#modules.conf
##################################################################
#[name="faketrig" type="snemo::digitization::fake_trigger_module"]
#@config A processor that simulate a fake trigger to make a selection on a simulated data

#@description The name of the simulated data bank
#SD_label : string = "SD"
