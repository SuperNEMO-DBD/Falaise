#!/usr/bin/env bash

itep_strip_ids="2 3 8 9 14 15 20 21 22 23 24 25 26 27 28 31 32 33 34"

itep_strips_data="./itep_strips.data"
# cat > ${itep_strips_data} <<EOF
# 2  4  250.0 135.0 2700.0 snemo::se82_enriched96.1_pva0.1000_density2.258   LAPP-ITEP-7
# 3  7  240.0 135.0 2700.0 snemo::se82_enriched96.9_pva0.1000_density2.592   LAPP-ITEP-3
# 8  6  220.0 135.0 2700.0 snemo::se82_enriched96.9_pva0.1000_density2.960   LAPP-ITEP-2
# 9  18 240.0 135.0 2700.0 snemo::se82_enriched98_pva0.0820_density2.160     ITEP-11
# 14 17 250.0 135.0 2700.0 snemo::se82_enriched97.5_pva0.0810_density2.300   ITEP-10
# 15 16 280.0 135.0 2700.0 snemo::se82_enriched97_pva0.0801_density2.276     ITEP-9
# 20 15 280.0 135.0 2700.0 snemo::se82_enriched99.6_pva0.0784_density2.388   ITEP-8
# 21 14 300.0 135.0 2700.0 snemo::se82_enriched99.92_pva0.0771_density2.217  ITEP-7  
# 22 13 280.0 135.0 2700.0 snemo::se82_enriched98.31_pva0.0820_density2.104  ITEP-6
# 23 12 300.0 135.0 2700.0 snemo::se82_enriched98.31_pva0.0786_density2.198  ITEP-5
# 24 11 200.0 135.0 2695.0 snemo::se82_enriched96.92_pva0.0783_density1.949  ITEP-4
# 25 10 200.0 135.0 2700.0 snemo::se82_enriched96.92_pva0.0790_density1.971  ITEP-3
# 26 9  190.0 135.0 2700.0 snemo::se82_enriched96.92_pva0.0780_density2.081  ITEP-2
# 27 8  200.0 135.0 2705.0 snemo::se82_enriched96.92_pva0.0787_density2.122  ITEP-1
# 28 34 280.0 135.0 2700.0 snemo::se82_enriched97.0_pva0.1000_density2.137   LAPP-ITEP-21
# 31 5  200.0 135.0 2700.0 snemo::se82_enriched96.9_pva0.1000_density2.823   LAPP-ITEP-1
# 32 2  210.0 135.0 2700.0 snemo::se82_enriched96.1_pva0.1000_density2.951   LAPP-ITEP-5
# 33 3  220.0 135.0 2700.0 snemo::se82_enriched96.1_pva0.1000_density2.800   LAPP-ITEP-6
# 34 1  230.0 135.0 2700.0 snemo::se82_enriched96.1_pva0.1000_density2.811   LAPP-ITEP-4
# EOF

if [ -f "source_itep_strip2_flat.geom" ]; then
    rm -f source_itep_strip2_flat.geom
fi

if [ -f "source_itep_strips_flat.geom" ]; then
    rm -f source_itep_strips_flat.geom
fi

cat > source_itep_strip2_flat.geom <<EOF
# -*- mode: conf-unix; -*-
#@description Geometry models for flat ITEP strip 2
#@key_label   "name"
#@meta_label  "type"

EOF

cat > source_itep_strips_flat.geom <<EOF
# -*- mode: conf-unix; -*-
#@description Geometry models for all flat ITEP strips (but 2)
#@key_label   "name"
#@meta_label  "type"

EOF

for itep_strip_id in ${itep_strip_ids} ; do
    source_itep_strips_file="source_itep_strips_flat.geom"
    if [ ${itep_strip_id} -eq 2 ]; then
	source_itep_strips_file="source_itep_strip2_flat.geom"
    fi
      
    echo >&2 "[info] ITEP strip ID = ${itep_strip_id}"
    itep_strip_t=`cat ${itep_strips_data} | grep "^${itep_strip_id} " | tr -s " " | cut -d' ' -f3`
    itep_strip_w=`cat ${itep_strips_data} | grep "^${itep_strip_id} " | tr -s " " | cut -d' ' -f4`
    itep_strip_h=`cat ${itep_strips_data} | grep "^${itep_strip_id} " | tr -s " " | cut -d' ' -f5`
    itep_strip_mat=`cat ${itep_strips_data} | grep "^${itep_strip_id} " | tr -s " " | cut -d' ' -f6`
    itep_strip_label=`cat ${itep_strips_data} | grep "^${itep_strip_id} " | tr -s " " | cut -d' ' -f7`
    itep_strip_legid=`cat ${itep_strips_data} | grep "^${itep_strip_id} " | tr -s " " | cut -d' ' -f2`
    echo >&2 "[info]   thickness = ${itep_strip_t} um"
    echo >&2 "[info]   width     = ${itep_strip_w} mm"
    echo >&2 "[info]   height    = ${itep_strip_h} mm"     
    echo >&2 "[info]   material  = ${itep_strip_mat}"
    echo >&2 "[info]   label     = ${itep_strip_label}"
    echo >&2 "[info]   legacy ID = ${itep_strip_legid}"
    cat template/itep_strip_flat.geom.template | \
	sed \
	    -e "s/@STRIP_ID@/${itep_strip_id}/g" \
	    -e "s/@STRIP_PAD0_THICKNESS@/${itep_strip_t}/g" \
	    -e "s/@STRIP_PAD0_WIDTH@/${itep_strip_w}/g" \
	    -e "s/@STRIP_PAD0_HEIGHT@/${itep_strip_h}/g" \
   	    -e "s/@STRIP_PAD0_MATERIAL@/${itep_strip_mat}/g" \
   	    -e "s/@STRIP_LABEL@/${itep_strip_label}/g" \
   	    -e "s/@STRIP_LEGACY_ID@/${itep_strip_legid}/g" \
	    >> ${source_itep_strips_file}

done

ls -l source_itep_strip2_flat.geom
ls -l source_itep_strips_flat.geom

exit 0

# end
