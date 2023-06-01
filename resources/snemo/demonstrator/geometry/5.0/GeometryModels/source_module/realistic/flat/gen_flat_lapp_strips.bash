#!/usr/bin/env bash

lapp_strip_ids="1 4 5 6 7 10 11 12 13 16 17 18 19 29 30"
# lapp_strip_ids="1 4"

lapp_strips_data="./lapp_strips.data"
cat > ${lapp_strips_data} <<EOF
1    31  LAPP-8
1.0  310.75 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.767 16.975  
1.1  298.5  133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.762 16.975  
1.2  322.5  133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.785 16.975  
1.3  331.25 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.789 16.975 
1.4  377.5  133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.723 16.975 
1.5  286.67 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.914 16.975 
1.6  276.67 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density2.024 16.975  
1.7  302.5  133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.865 16.975  

4    32  LAPP-9
4.0  288.83 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.890  16.975  
4.1  288.3  133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.911  16.975  
4.2  296.25 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.828  16.975  
4.3  271.67 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.974  16.975  
4.4  323.33 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.861  16.975  
4.5  308.89 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.762 16.975  
4.6  275.17 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.930 16.975   
4.7  286.67 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.850 16.975   

5    33  LAPP-20
5.0  309.17 133.0 335.5  snemo::se82_enriched97.9_pva0.1000_density1.975  16.975 
5.1  310.83 133.0 335.5  snemo::se82_enriched97.9_pva0.1000_density1.990  16.975 
5.2  304.17 133.0 335.5  snemo::se82_enriched97.9_pva0.1000_density1.964  16.975 
5.3  293.33 133.0 335.5  snemo::se82_enriched97.9_pva0.1000_density1.947  16.975 
5.4  309.17 133.0 335.5  snemo::se82_enriched97.9_pva0.1000_density1.991  16.975 
5.5  305.83 133.0 335.5  snemo::se82_enriched97.9_pva0.1000_density1.955  16.975 
5.6  285.0  133.0 335.5  snemo::se82_enriched97.9_pva0.1000_density2.098  16.975 
5.7  452.5  132.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.367 16.975  

6    26  LAPP-14
6.0  370.0  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.543  16.9
6.1  372.5  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.530  16.9
6.2  360.83 132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.521  16.9
6.3  372.5  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.537  16.9
6.4  386.67 132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.525  16.9
6.5  360.83 132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.566  16.9
6.6  337.5  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.599  16.9
6.7  364.17 132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.577  16.9

7    25  LAPP-13
7.0  366.67 133.0 335.5  snemo::se82_enriched96.65_pva0.1000_density1.584  16.975 
7.1  355.0  133.0 335.5  snemo::se82_enriched96.65_pva0.1000_density1.526  16.975 
7.2  353.33 133.0 335.5  snemo::se82_enriched96.65_pva0.1000_density1.545  16.975 
7.3  362.5  133.0 335.5  snemo::se82_enriched96.65_pva0.1000_density1.414  16.975 
7.4  360.83 133.0 335.5  snemo::se82_enriched96.65_pva0.1000_density1.448  16.975 
7.5  379.17 133.0 335.5  snemo::se82_enriched96.65_pva0.1000_density1.390  16.975 
7.6  383.33 133.0 335.5  snemo::se82_enriched96.65_pva0.1000_density1.300  16.975 
7.7  350.0  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.535  16.9

10   28  LAPP-16
10.0  347.0  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.597 16.9
10.1  390.0  133.0 335.0  snemo::se82_enriched96.65_pva0.1000_density1.444 16.95 
10.2  351.0  133.0 335.0  snemo::se82_enriched96.65_pva0.1000_density1.531 16.95 
10.3  352.0  133.0 335.0  snemo::se82_enriched96.65_pva0.1000_density1.578 16.95 
10.4  402.0  133.0 335.0  snemo::se82_enriched96.65_pva0.1000_density1.317 16.95 
10.5  411.0  133.0 335.0  snemo::se82_enriched96.65_pva0.1000_density1.257 16.95 
10.6  400.0  133.0 335.0  snemo::se82_enriched96.65_pva0.1000_density1.306 16.95 
10.7  471.25 133.0 335.0  snemo::se82_enriched99.88_pva0.1000_density1.408 16.95 

11   23  LAPP-17
11.0  316.17 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.749 16.975 
11.1  334.17 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.664 16.975 
11.2  343.33 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.654 16.975 
11.3  326.67 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.708 16.975 
11.4  330.83 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.721 16.975 
11.5  340.0  133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.730 16.975 
11.6  342.5  133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.758 16.975 
11.7  345.83 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.591 16.975 

12   24  LAPP-19
12.0  360.83 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.547 16.975
12.1  372.53 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.552 16.975
12.2  365.0  133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.517 16.975
12.3  363.33 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.524 16.975
12.4  365.83 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.580 16.975
12.5  328.33 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.714 16.975
12.6  370.83 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.480 16.975
12.7  472.5  132.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.363 16.975

13   21  LAPP-11
13.0  495.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.060 16.975
13.1  472.5  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.030 16.975
13.2  424.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.088 16.975
13.3  427.5  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.333 16.975
13.4  420.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.300 16.975
13.5  420.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.296 16.975
13.6  322.5  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.648 16.975
13.7  320.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.708 16.975

16   20  LAPP-10
16.0  266.67 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density2.068 16.975
16.1  279.17 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density2.093 16.975
16.2  263.33 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density2.096 16.975
16.3  289.17 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.972 16.975
16.4  365.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.288 16.975
16.5  377.5  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.268 16.975
16.6  410.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.140 16.975
16.7  460.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.236 16.975

17   29  LAPP-18
17.0  282.17 133.0 333.0  snemo::se82_enriched96.65_pva0.1000_density1.596 16.85
17.1  290.83 133.0 334.0  snemo::se82_enriched96.65_pva0.1000_density1.449 16.9
17.2  304.17 133.0 334.0  snemo::se82_enriched96.65_pva0.1000_density1.444 16.9
17.3  287.5  133.0 334.0  snemo::se82_enriched96.65_pva0.1000_density1.509 16.9
17.4  311.67 133.0 334.0  snemo::se82_enriched96.65_pva0.1000_density1.486 16.9
17.5  302.5  133.0 335.5  snemo::se82_enriched96.65_pva0.1000_density1.570 16.875
17.6  275.0  133.0 334.0  snemo::se82_enriched96.65_pva0.1000_density1.646 16.9
17.7  259.17 133.0 334.0  snemo::se82_enriched96.65_pva0.1000_density1.593 16.9

18   22  LAPP-12
18.0  330.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.678 16.975
18.1  320.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.717 16.975
18.2  342.5  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.576 16.975
18.3  315.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.670 16.975
18.4  312.5  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.586 16.975
18.5  480.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.347 16.975
18.6  467.5  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.362 16.975
18.7  465.0  132.5 335.5  snemo::se82_enriched99.88_pva0.1000_density1.372 16.975

19   19  LAPP-7
19.0  285.0  133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.919 16.975
19.1  273.3  133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.959 16.975
19.2  292.5  133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.899 16.975
19.3  304.17 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.795 16.975
19.4  304.17 133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.886 16.975
19.5  283.3  133.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.952 16.975
19.6  397.5  132.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.299 16.975
19.7  390.0  132.0 335.5  snemo::se82_enriched99.88_pva0.1000_density1.227 16.975

29   27  LAPP-15
29.0  357.5  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.590 16.9
29.1  365.0  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.562 16.9
29.2  349.17 132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.596 16.9
29.3  355.0  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.597 16.9
29.4  368.18 132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.542 16.9
29.5  339.0  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.626 16.9
29.6  336.0  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.617 16.9
29.7  361.0  132.5 334.0  snemo::se82_enriched96.65_pva0.1000_density1.487 16.9

30   30  LAPP-6
30.0  346.66 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.725  16.975
30.1  277.08 132.5 335.5  snemo::se82_enriched96.9_pva0.1000_density2.321  16.975
30.2  310.0  132.5 335.5  snemo::se82_enriched96.9_pva0.1000_density2.107  16.975
30.3  309.17 132.5 335.5  snemo::se82_enriched96.9_pva0.1000_density2.208  16.975
30.4  291.67 132.5 335.5  snemo::se82_enriched96.9_pva0.1000_density2.176  16.975
30.5  280.83 132.5 335.5  snemo::se82_enriched96.9_pva0.1000_density2.200  16.975
30.6  308.3  132.5 335.5  snemo::se82_enriched96.9_pva0.1000_density2.148  16.975
30.7  348.75 133.0 335.5  snemo::se82_enriched96.1_pva0.1000_density1.737  16.975

EOF


if [ -f "source_lapp_strips_flat.geom" ]; then
    rm -f source_lapp_strips_flat.geom
fi


cat > source_lapp_strips_flat.geom <<EOF
# -*- mode: conf-unix; -*-
#@description Geometry models for all flat LAPP strips
#@key_label   "name"
#@meta_label  "type"

EOF

lapp_pad_ids="0 1 2 3 4 5 6 7"

for lapp_strip_id in ${lapp_strip_ids} ; do
    echo >&2 "[info] LAPP strip ID = ${lapp_strip_id}"
    source_lapp_strips_file="source_lapp_strips_flat.geom"
    lapp_strip_legid=`cat ${lapp_strips_data} | grep "^${lapp_strip_id} " | tr -s " " | cut -d' ' -f2`    
    lapp_strip_label=`cat ${lapp_strips_data} | grep "^${lapp_strip_id} " | tr -s " " | cut -d' ' -f3`
    echo >&2 "[info]   label     = ${lapp_strip_label}"
    echo >&2 "[info]   legacy ID = ${lapp_strip_legid}"
    declare -a lapp_pads_strip_t
    declare -a lapp_pads_strip_w
    declare -a lapp_pads_strip_h
    declare -a lapp_pads_strip_mat
    declare -a lapp_pads_strip_lim_max
    for pad_id in ${lapp_pad_ids} ; do
	echo >&2 "[info]   LAPP pad = ${lapp_strip_id}.${pad_id}"
    	lapp_pads_strip_t[${pad_id}]=`cat ${lapp_strips_data}   | grep "^${lapp_strip_id}.${pad_id} " | tr -s " " | cut -d' ' -f2`
	lapp_pads_strip_w[${pad_id}]=`cat ${lapp_strips_data}   | grep "^${lapp_strip_id}.${pad_id} " | tr -s " " | cut -d' ' -f3`
	lapp_pads_strip_h[${pad_id}]=`cat ${lapp_strips_data}   | grep "^${lapp_strip_id}.${pad_id} " | tr -s " " | cut -d' ' -f4`
	lapp_pads_strip_mat[${pad_id}]=`cat ${lapp_strips_data} | grep "^${lapp_strip_id}.${pad_id} " | tr -s " " | cut -d' ' -f5`
	lapp_pads_strip_lim_max[${pad_id}]=`cat ${lapp_strips_data} | grep "^${lapp_strip_id}.${pad_id} " | tr -s " " | cut -d' ' -f6`
	echo >&2 "[info]     thickness = ${lapp_pads_strip_t[${pad_id}]} um"
	echo >&2 "[info]     width     = ${lapp_pads_strip_w[${pad_id}]} mm"
	echo >&2 "[info]     height    = ${lapp_pads_strip_h[${pad_id}]} mm"     
	echo >&2 "[info]     material  = ${lapp_pads_strip_mat[${pad_id}]}"
	echo >&2 "[info]     lim.max.  = ${lapp_pads_lim_max[${pad_id}]} cm"
     done
    cat template/lapp_strip_flat.geom.template | \
	sed \
	    -e "s/@STRIP_ID@/${lapp_strip_id}/g" \
     	    -e "s/@STRIP_LABEL@/${lapp_strip_label}/g" \
   	    -e "s/@STRIP_LEGACY_ID@/${lapp_strip_legid}/g" \
	    -e "s/@STRIP_PAD0_THICKNESS@/${lapp_pads_strip_t[0]}/g" \
	    -e "s/@STRIP_PAD0_WIDTH@/${lapp_pads_strip_w[0]}/g" \
	    -e "s/@STRIP_PAD0_HEIGHT@/${lapp_pads_strip_h[0]}/g" \
   	    -e "s/@STRIP_PAD0_MATERIAL@/${lapp_pads_strip_mat[0]}/g" \
   	    -e "s/@STRIP_PAD0_LIMMAX@/${lapp_pads_strip_lim_max[0]}/g" \
	    -e "s/@STRIP_PAD1_THICKNESS@/${lapp_pads_strip_t[1]}/g" \
	    -e "s/@STRIP_PAD1_WIDTH@/${lapp_pads_strip_w[1]}/g" \
	    -e "s/@STRIP_PAD1_HEIGHT@/${lapp_pads_strip_h[1]}/g" \
   	    -e "s/@STRIP_PAD1_MATERIAL@/${lapp_pads_strip_mat[1]}/g" \
   	    -e "s/@STRIP_PAD1_LIMMAX@/${lapp_pads_strip_lim_max[1]}/g" \
	    -e "s/@STRIP_PAD2_THICKNESS@/${lapp_pads_strip_t[2]}/g" \
	    -e "s/@STRIP_PAD2_WIDTH@/${lapp_pads_strip_w[2]}/g" \
	    -e "s/@STRIP_PAD2_HEIGHT@/${lapp_pads_strip_h[2]}/g" \
   	    -e "s/@STRIP_PAD2_MATERIAL@/${lapp_pads_strip_mat[2]}/g" \
   	    -e "s/@STRIP_PAD2_LIMMAX@/${lapp_pads_strip_lim_max[2]}/g" \
	    -e "s/@STRIP_PAD3_THICKNESS@/${lapp_pads_strip_t[3]}/g" \
	    -e "s/@STRIP_PAD3_WIDTH@/${lapp_pads_strip_w[3]}/g" \
	    -e "s/@STRIP_PAD3_HEIGHT@/${lapp_pads_strip_h[3]}/g" \
   	    -e "s/@STRIP_PAD3_MATERIAL@/${lapp_pads_strip_mat[3]}/g" \
   	    -e "s/@STRIP_PAD3_LIMMAX@/${lapp_pads_strip_lim_max[3]}/g" \
	    -e "s/@STRIP_PAD4_THICKNESS@/${lapp_pads_strip_t[4]}/g" \
	    -e "s/@STRIP_PAD4_WIDTH@/${lapp_pads_strip_w[4]}/g" \
	    -e "s/@STRIP_PAD4_HEIGHT@/${lapp_pads_strip_h[4]}/g" \
   	    -e "s/@STRIP_PAD4_MATERIAL@/${lapp_pads_strip_mat[4]}/g" \
   	    -e "s/@STRIP_PAD4_LIMMAX@/${lapp_pads_strip_lim_max[4]}/g" \
	    -e "s/@STRIP_PAD5_THICKNESS@/${lapp_pads_strip_t[5]}/g" \
	    -e "s/@STRIP_PAD5_WIDTH@/${lapp_pads_strip_w[5]}/g" \
	    -e "s/@STRIP_PAD5_HEIGHT@/${lapp_pads_strip_h[5]}/g" \
   	    -e "s/@STRIP_PAD5_MATERIAL@/${lapp_pads_strip_mat[5]}/g" \
   	    -e "s/@STRIP_PAD5_LIMMAX@/${lapp_pads_strip_lim_max[5]}/g" \
	    -e "s/@STRIP_PAD6_THICKNESS@/${lapp_pads_strip_t[6]}/g" \
	    -e "s/@STRIP_PAD6_WIDTH@/${lapp_pads_strip_w[6]}/g" \
	    -e "s/@STRIP_PAD6_HEIGHT@/${lapp_pads_strip_h[6]}/g" \
   	    -e "s/@STRIP_PAD6_MATERIAL@/${lapp_pads_strip_mat[6]}/g" \
   	    -e "s/@STRIP_PAD6_LIMMAX@/${lapp_pads_strip_lim_max[6]}/g" \
	    -e "s/@STRIP_PAD7_THICKNESS@/${lapp_pads_strip_t[7]}/g" \
	    -e "s/@STRIP_PAD7_WIDTH@/${lapp_pads_strip_w[7]}/g" \
	    -e "s/@STRIP_PAD7_HEIGHT@/${lapp_pads_strip_h[7]}/g" \
   	    -e "s/@STRIP_PAD7_MATERIAL@/${lapp_pads_strip_mat[7]}/g" \
   	    -e "s/@STRIP_PAD7_LIMMAX@/${lapp_pads_strip_lim_max[7]}/g" \
	    >> ${source_lapp_strips_file}


done

ls -l source_lapp_strips_flat.geom

exit 0

# end
