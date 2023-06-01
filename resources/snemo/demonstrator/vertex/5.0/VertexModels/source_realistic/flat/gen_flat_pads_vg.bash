#!/usr/bin/env bash

# Generate ITEP and LAPP bulk and surface VG definition files from templates:

lapp_strip_ids="1 4 5 6 7  10 11 12 13 16 17 18 19 29 30"
itep_strip_ids="2 3 8 9 14 15 20 21 22 23 24 25 26 27 28 31 32 33 34"

#####################
# LAPP-style strips #
#####################

lapp_strip_data="data/lapp_strip.data"
lapp_pads_surf_vg_template="template/strip_LAPP_pads_surface_vg.conf.template"
lapp_pads_bulk_vg_template="template/strip_LAPP_pads_bulk_vg.conf.template"
lapp_strip_bulk_vg_template="template/strip_LAPP_full_bulk_vg.conf.template"
lapp_strip_surface_vg_template="template/strip_LAPP_full_surface_vg.conf.template"

declare -i strip_counter=0
for lapp_strip_id in ${lapp_strip_ids}; do
    echo >&2 "[info] Strip counter = ${strip_counter}"
    echo >&2 "[info]   LAPP strip ID = ${lapp_strip_id}"

    sed -e "s/@STRIPID@/${lapp_strip_id}/g" ${lapp_pads_surf_vg_template} > strip_${lapp_strip_id}_pads_surface_vg.conf
    sed -e "s/@STRIPID@/${lapp_strip_id}/g" ${lapp_pads_bulk_vg_template} > strip_${lapp_strip_id}_pads_bulk_vg.conf

    se_masses=$(cat ${lapp_strip_data}    | grep "^${lapp_strip_id} " | grep "semass" | tr -s ' ' | cut -d' ' -f3-10)
    echo >&2 "[info]   Pads Se masses    = '${se_masses}'"
    
    total_masses=$(cat ${lapp_strip_data} | grep "^${lapp_strip_id} " | grep "totalmass" | tr -s ' ' | cut -d' ' -f3-10)
    echo >&2 "[info]   Pads total masses = '${total_masses}'"
    
    surfaces=$(cat ${lapp_strip_data}     | grep "^${lapp_strip_id} " | grep "surface" | tr -s ' ' | cut -d' ' -f3-10)
    echo >&2 "[info]   Pads surfaces     = '${surfaces}'"

    lapp_strip_se_mass0="`echo ${se_masses} | cut -d ' ' -f1`"
    lapp_strip_se_mass1="`echo ${se_masses} | cut -d ' ' -f2`" 
    lapp_strip_se_mass2="`echo ${se_masses} | cut -d ' ' -f3`" 
    lapp_strip_se_mass3="`echo ${se_masses} | cut -d ' ' -f4`"
    lapp_strip_se_mass4="`echo ${se_masses} | cut -d ' ' -f5`" 
    lapp_strip_se_mass5="`echo ${se_masses} | cut -d ' ' -f6`" 
    lapp_strip_se_mass6="`echo ${se_masses} | cut -d ' ' -f7`" 
    lapp_strip_se_mass7="`echo ${se_masses} | cut -d ' ' -f8`" 
    echo >&2 "[info]   Pad 0 Se mass    = '${lapp_strip_se_mass0}' g"
    echo >&2 "[info]   Pad 1 Se mass    = '${lapp_strip_se_mass1}' g"
    echo >&2 "[info]   Pad 2 Se mass    = '${lapp_strip_se_mass2}' g"
    echo >&2 "[info]   Pad 3 Se mass    = '${lapp_strip_se_mass3}' g"
    echo >&2 "[info]   Pad 4 Se mass    = '${lapp_strip_se_mass4}' g"
    echo >&2 "[info]   Pad 5 Se mass    = '${lapp_strip_se_mass5}' g"
    echo >&2 "[info]   Pad 6 Se mass    = '${lapp_strip_se_mass6}' g"
    echo >&2 "[info]   Pad 7 Se mass    = '${lapp_strip_se_mass7}' g"
    
    lapp_strip_total_mass0="`echo ${total_masses} | cut -d ' ' -f1`"
    lapp_strip_total_mass1="`echo ${total_masses} | cut -d ' ' -f2`" 
    lapp_strip_total_mass2="`echo ${total_masses} | cut -d ' ' -f3`" 
    lapp_strip_total_mass3="`echo ${total_masses} | cut -d ' ' -f4`"
    lapp_strip_total_mass4="`echo ${total_masses} | cut -d ' ' -f5`" 
    lapp_strip_total_mass5="`echo ${total_masses} | cut -d ' ' -f6`" 
    lapp_strip_total_mass6="`echo ${total_masses} | cut -d ' ' -f7`" 
    lapp_strip_total_mass7="`echo ${total_masses} | cut -d ' ' -f8`" 
    echo >&2 "[info]   Pad 0 total mass = '${lapp_strip_total_mass0}' g"
    echo >&2 "[info]   Pad 1 total mass = '${lapp_strip_total_mass1}' g"
    echo >&2 "[info]   Pad 2 total mass = '${lapp_strip_total_mass2}' g"
    echo >&2 "[info]   Pad 3 total mass = '${lapp_strip_total_mass3}' g"
    echo >&2 "[info]   Pad 4 total mass = '${lapp_strip_total_mass4}' g"
    echo >&2 "[info]   Pad 5 total mass = '${lapp_strip_total_mass5}' g"
    echo >&2 "[info]   Pad 6 total mass = '${lapp_strip_total_mass6}' g"
    echo >&2 "[info]   Pad 7 total mass = '${lapp_strip_total_mass7}' g"
    
    lapp_strip_surface0="`echo ${surfaces} | cut -d ' ' -f1`"
    lapp_strip_surface1="`echo ${surfaces} | cut -d ' ' -f2`" 
    lapp_strip_surface2="`echo ${surfaces} | cut -d ' ' -f3`" 
    lapp_strip_surface3="`echo ${surfaces} | cut -d ' ' -f4`"
    lapp_strip_surface4="`echo ${surfaces} | cut -d ' ' -f5`" 
    lapp_strip_surface5="`echo ${surfaces} | cut -d ' ' -f6`" 
    lapp_strip_surface6="`echo ${surfaces} | cut -d ' ' -f7`" 
    lapp_strip_surface7="`echo ${surfaces} | cut -d ' ' -f8`" 
    echo >&2 "[info]   Pad 0 surface    = '${lapp_strip_surface0}' cm2"
    echo >&2 "[info]   Pad 1 surface    = '${lapp_strip_surface1}' cm2"
    echo >&2 "[info]   Pad 2 surface    = '${lapp_strip_surface2}' cm2"
    echo >&2 "[info]   Pad 3 surface    = '${lapp_strip_surface3}' cm2"
    echo >&2 "[info]   Pad 4 surface    = '${lapp_strip_surface4}' cm2"
    echo >&2 "[info]   Pad 5 surface    = '${lapp_strip_surface5}' cm2"
    echo >&2 "[info]   Pad 6 surface    = '${lapp_strip_surface6}' cm2"
    echo >&2 "[info]   Pad 7 surface    = '${lapp_strip_surface7}' cm2"
   
    sed -e "s/@STRIPID@/${lapp_strip_id}/g" \
     	-e "s/@STRIPSEMASS0@/${lapp_strip_se_mass0}/g" \
     	-e "s/@STRIPSEMASS1@/${lapp_strip_se_mass1}/g" \
     	-e "s/@STRIPSEMASS2@/${lapp_strip_se_mass2}/g" \
     	-e "s/@STRIPSEMASS3@/${lapp_strip_se_mass3}/g" \
     	-e "s/@STRIPSEMASS4@/${lapp_strip_se_mass4}/g" \
     	-e "s/@STRIPSEMASS5@/${lapp_strip_se_mass5}/g" \
    	-e "s/@STRIPSEMASS6@/${lapp_strip_se_mass6}/g" \
    	-e "s/@STRIPSEMASS7@/${lapp_strip_se_mass7}/g" \
      	-e "s/@STRIPTOTALMASS0@/${lapp_strip_total_mass0}/g" \
      	-e "s/@STRIPTOTALMASS1@/${lapp_strip_total_mass1}/g" \
      	-e "s/@STRIPTOTALMASS2@/${lapp_strip_total_mass2}/g" \
      	-e "s/@STRIPTOTALMASS3@/${lapp_strip_total_mass3}/g" \
      	-e "s/@STRIPTOTALMASS4@/${lapp_strip_total_mass4}/g" \
      	-e "s/@STRIPTOTALMASS5@/${lapp_strip_total_mass5}/g" \
      	-e "s/@STRIPTOTALMASS6@/${lapp_strip_total_mass6}/g" \
      	-e "s/@STRIPTOTALMASS7@/${lapp_strip_total_mass7}/g" \
    	${lapp_strip_bulk_vg_template} > strip_${lapp_strip_id}_full_bulk_vg.conf
   
    sed -e "s/@STRIPID@/${lapp_strip_id}/g" \
     	-e "s/@STRIPSURFACE0@/${lapp_strip_surface0}/g" \
     	-e "s/@STRIPSURFACE1@/${lapp_strip_surface1}/g" \
     	-e "s/@STRIPSURFACE2@/${lapp_strip_surface2}/g" \
     	-e "s/@STRIPSURFACE3@/${lapp_strip_surface3}/g" \
     	-e "s/@STRIPSURFACE4@/${lapp_strip_surface4}/g" \
     	-e "s/@STRIPSURFACE5@/${lapp_strip_surface5}/g" \
    	-e "s/@STRIPSURFACE6@/${lapp_strip_surface6}/g" \
    	-e "s/@STRIPSURFACE7@/${lapp_strip_surface7}/g" \
   	${lapp_strip_surface_vg_template} > strip_${lapp_strip_id}_full_surface_vg.conf
    
    let strip_counter=strip_counter+1
done

#####################
# ITEP-style strips #
#####################

itep_pad_surf_vg_template="template/strip_ITEP_pad_surface_vg.conf.template"
itep_pad_bulk_vg_template="template/strip_ITEP_pad_bulk_vg.conf.template"
itep_strip_bulk_vg_template="template/strip_ITEP_full_bulk_vg.conf.template"
itep_strip_surface_vg_template="template/strip_ITEP_full_surface_vg.conf.template"

declare -A itep_strip_se_masses
itep_strip_se_masses=(    [2]=185.175 [3]=204.066 [8]=213.641 [9]=173.502 [14]=192.6224 [15]=213.69277 [20]=224.59392 [21]=223.71096 [22]=197.0946 [23]=221.50456 [24]=130.69706 [25]=132.3477 [26]=132.8602 [27]=142.8015 [28]=196.254 [31]=185.193 [32]=203.31 [33]=202.113 [34]=212.112)

itep_strip_total_masses=( [2]=205.75  [3]=226.74  [8]=237.39  [9]=189.0   [14]=209.6    [15]=232.3     [20]=243.7     [21]=242.4     [22]=214.7     [23]=240.4    [24]=141.8     [25]=143.7    [26]=144.1    [27]=155.0    [28]=218.06  [31]=205.77  [32]=225.9  [33]=224.57  [34]=235.68)

itep_strip_surfaces=( [2]=3645.0  [3]=3645.0  [8]=3645.0  [9]=3645.0   [14]=3645.0    [15]=3645.0     [20]=3645.0     [21]=3645.0     [22]=3645.0     [23]=3645.0    [24]=3638.25     [25]=3645.0    [26]=3644.0    [27]=3651.75    [28]=3645.0  [31]=3645.0  [32]=3645.0  [33]=3645.0  [34]=3645.0)

echo >&2 "[info] itep_strip_se_masses=${itep_strip_se_masses[@]}"
echo >&2 "[info]   itep_strip_se_masses[2]=${itep_strip_se_masses[2]}"
echo >&2 "[info]   itep_strip_se_masses[8]=${itep_strip_se_masses[8]}"
echo >&2 "[info] itep_strip_total_masses=${itep_strip_total_masses[@]}"
echo >&2 "[info] itep_strip_surfaces=${itep_strip_surfaces[@]}"

declare -i strip_counter=0
for itep_strip_id in ${itep_strip_ids}; do
    echo >&2 "[info] Strip counter = ${strip_counter}"
    echo >&2 "[info]   ITEP strip ID = ${itep_strip_id}"

    sed -e "s/@STRIPID@/${itep_strip_id}/g" ${itep_pad_surf_vg_template} > strip_${itep_strip_id}_pad_surface_vg.conf
    sed -e "s/@STRIPID@/${itep_strip_id}/g" ${itep_pad_bulk_vg_template} > strip_${itep_strip_id}_pad_bulk_vg.conf

    itep_strip_se_mass="${itep_strip_se_masses[${itep_strip_id}]}"
    echo >&2 "[info]   Se mass    = '${itep_strip_se_mass}' g"

    itep_strip_total_mass="${itep_strip_total_masses[${itep_strip_id}]}"
    echo >&2 "[info]   Total mass = '${itep_strip_total_mass}' g"

    itep_strip_surface="${itep_strip_surfaces[${itep_strip_id}]}"
    echo >&2 "[info]   Total surface = '${itep_strip_surface}' cm2"

    sed -e "s/@STRIPID@/${itep_strip_id}/g" \
     	-e "s/@STRIPSEMASS@/${itep_strip_se_mass}/g" \
      	-e "s/@STRIPTOTALMASS@/${itep_strip_total_mass}/g" \
	${itep_strip_bulk_vg_template} > strip_${itep_strip_id}_full_bulk_vg.conf
 
    sed -e "s/@STRIPID@/${itep_strip_id}/g" \
     	-e "s/@STRIPSURFACE@/${itep_strip_surface}/g" \
	${itep_strip_surface_vg_template} > strip_${itep_strip_id}_full_surface_vg.conf
     
    let strip_counter=strip_counter+1
    
done

exit 0

# end
