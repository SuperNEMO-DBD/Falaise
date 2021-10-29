#!/usr/bin/env bash


csv_target="_test_vertexes_generators.csv"
if [ -f ${csv_target} ]; then
    rm -f  ${csv_target}
fi
touch ${csv_target}

do_batch_basic=true

if [ ${do_batch_basic} = true ]; then

    # base_vg_conf_files=`ls -1 ../../VertexModels/*.conf`
    # for bvcf in ${base_vg_conf_files} ; do
    # 	echo >&2 "[info] file='${bvcf}'"
    # 	bxextract_table_of_objects -i "${bvcf}" --group "SourceFoilRealistic" -G \
    # 				   >> ${csv_target}
    # done
    
    bxextract_table_of_objects -i ../../VertexModels/om_pmt_bulk_vg.conf  --group "OpticalModule" -G\
			       >> ${csv_target}

    bxextract_table_of_objects -i ../../VertexModels/om_vg.conf --group "OpticalModule" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../../VertexModels/tracker_vg.conf \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../../VertexModels/hall_vg.conf --group "Hall" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../../VertexModels/source_basic/source_vg.conf --group "SourceFoilBasic" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../../VertexModels/shielding_vg.conf --group "Shielding" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../../VertexModels/source_calibration_vg.conf --group "Calibration" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../../VertexModels/commissioning_vg.conf --group "HalfCommissioning" -G \
			       >> ${csv_target}

    bxextract_table_of_objects -i ../../VertexModels/misc_vg.conf -G \
			       >> ${csv_target}

fi

real_vg_conf_files=`ls -1 ../../VertexModels/source_realistic/*.conf | sort -V`
for rvcf in ${real_vg_conf_files} ; do
    echo >&2 "[info] file='${rvcf}'"
    bxextract_table_of_objects -i "${rvcf}" --group "SourceFoilRealistic" -G \
       >> ${csv_target}
done

real_flat_vg_conf_files=`ls -1 ../../VertexModels/source_realistic/flat/*.conf | sort -V`
for rfvcf in ${real_flat_vg_conf_files} ; do
    echo >&2 "[info] file='${rfvcf}'"
    # bxextract_table_of_objects -i "${rfvcf}" --group "SourceFoilRealistic/Flat" -G \
    bxextract_table_of_objects -i "${rfvcf}"  \
       >> ${csv_target}
done

real_snrs1_vg_conf_files=`ls -1 ../../VertexModels/source_realistic/snrs1/*.conf | sort -V`
for rsvcf in ${real_snrs1_vg_conf_files} ; do
    echo >&2 "[info] file='${rsvcf}'"
    # bxextract_table_of_objects -i "${rsvcf}" --group "SourceFoilRealistic/SNRS1" -G \
    bxextract_table_of_objects -i "${rsvcf}" \
       >> ${csv_target}
done

exit 0

# end
