#!/usr/bin/env bash

# Generate ITEP bulk and surface VG definition files from templates:

itep_strip_ids="3 8 9 14 15 20 21 22 23 24 25 26 27 28 31 32 33 34"

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
