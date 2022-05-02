#!/usr/bin/env bash


csv_target="new_vertexes_generators.csv"

if [ -f ${csv_target} ]; then
    rm -f ${csv_target}
fi
touch ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/om_pmt_bulk_vg.conf  --group "OpticalModule" -G\
   >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/tracker_vg.conf \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/om_vg.conf --group "OpticalModule" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/hall_vg.conf --group "Hall" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/source_vg.conf --group "SourceFoilBasic" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/shielding_vg.conf --group "Shielding" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/source_calibration_vg.conf --group "Calibration" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/commissioning_vg.conf --group "HalfCommissioning" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/misc_vg.conf -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/realistic_flat_full_source_bulk_vg.conf --group "SourceFoilRealisticFlat" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/realistic_flat_full_source_surface_vg.conf --group "SourceFoilRealisticFlat" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/realistic_flat_source_strips_surface_uniform_vg.conf --group "SourceFoilRealisticFlat" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/realistic_flat_source_strips_bulk_uniform_mass_vg.conf --group "SourceFoilRealisticFlat" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/realistic_flat_source_strips_bulk_se82_vg.conf --group "SourceFoilRealisticFlat" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/realistic_flat_source_pads_surface_vg.conf --group "SourceFoilRealisticFlat" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/realistic_flat_source_pads_bulk_vg.conf --group "SourceFoilRealisticFlat" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/sds_bi207_calibration_source_spots_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/sds_bi207_calibration_source_track_0_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/sds_bi207_calibration_source_track_1_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/sds_bi207_calibration_source_track_2_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/sds_bi207_calibration_source_track_3_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/sds_bi207_calibration_source_track_4_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/sds_bi207_calibration_source_track_5_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}

bxextract_table_of_objects -i ../../VertexModels/sds_bi207_calibration_source_all_tracks_vg.conf --group "CalibrationSDS" -G \
       >> ${csv_target}
