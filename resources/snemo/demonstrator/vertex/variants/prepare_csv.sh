#!/usr/bin/env bash

vertexVersion="5.0"

do_batch_basic=true
csv_target="new_vertexes_generators.csv"
csv_target="_test_vertexes_generators.csv"
if [ -f ${csv_target} ]; then
    rm -f  ${csv_target}
fi
touch ${csv_target}


if [ ${do_batch_basic} = true ]; then

    # base_vg_conf_files=`ls -1 ../${vertexVersion}/VertexModels/*.conf`
    # for bvcf in ${base_vg_conf_files} ; do
    # 	echo >&2 "[info] file='${bvcf}'"
    # 	bxextract_table_of_objects -i "${bvcf}" --group "SourceFoilRealistic" -G \
    # 				   >> ${csv_target}
    # done
    
    bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/om_pmt_bulk_vg.conf  --group "OpticalModule" -G\
			       >> ${csv_target}

    bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/om_vg.conf --group "OpticalModule" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/tracker_vg.conf \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/hall_vg.conf --group "Hall" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/source_basic/source_vg.conf --group "SourceFoilBasic" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/shielding_vg.conf --group "Shielding" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/source_calibration_vg.conf --group "Calibration" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/commissioning_vg.conf --group "HalfCommissioning" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/misc_vg.conf -G \
			       >> ${csv_target}

fi

real_vg_conf_files=`ls -1 ../${vertexVersion}/VertexModels/source_realistic/*.conf | sort -V`
for rvcf in ${real_vg_conf_files} ; do
    echo >&2 "[info] file='${rvcf}'"
    bxextract_table_of_objects -i "${rvcf}" --group "SourceFoilRealistic" -G \
       >> ${csv_target}
done

real_flat_vg_conf_files=`ls -1 ../${vertexVersion}/VertexModels/source_realistic/flat/*.conf | sort -V`
for rfvcf in ${real_flat_vg_conf_files} ; do
    echo >&2 "[info] file='${rfvcf}'"
    # bxextract_table_of_objects -i "${rfvcf}" --group "SourceFoilRealistic/Flat" -G \
    bxextract_table_of_objects -i "${rfvcf}"  \
       >> ${csv_target}
done

real_snrs1_vg_conf_files=`ls -1 ../${vertexVersion}/VertexModels/source_realistic/snrs1/*.conf | sort -V`
for rsvcf in ${real_snrs1_vg_conf_files} ; do
    echo >&2 "[info] file='${rsvcf}'"
    # bxextract_table_of_objects -i "${rsvcf}" --group "SourceFoilRealistic/SNRS1" -G \
    bxextract_table_of_objects -i "${rsvcf}" \
       >> ${csv_target}
done

bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/sds_bi207_calibration_source_spots_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/sds_bi207_calibration_source_track_0_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/sds_bi207_calibration_source_track_1_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/sds_bi207_calibration_source_track_2_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/sds_bi207_calibration_source_track_3_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/sds_bi207_calibration_source_track_4_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/sds_bi207_calibration_source_track_5_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../${vertexVersion}/VertexModels/sds_bi207_calibration_source_all_tracks_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

exit 0

# end
