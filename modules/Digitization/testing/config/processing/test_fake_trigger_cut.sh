#!/usr/bin/env bash

APP_NAME="test_fake_trigger_cut.sh"

echo "Starting..." >&2
PROCESSING_DIR="${FALAISE_DIGITIZATION_TESTING_DIR}/config/processing"

INPUT_DIR="/data/nemo/group/Private/goliviero//raw_simulated_data_brio"
OUTPUT_DIR="/data/nemo/group/Private/goliviero//fake_trigger_simulated_data_brio"

input_module_file="modules.conf"
input_module_mgr_file="module_manager.conf"



input_file_list=`ls $INPUT_DIR/*SD.brio`

for file in $input_file_list
do
    #echo $file
    input_sd_data=$file
    full_dir_name=`echo "$input_sd_data"`
    base_name=`basename $full_dir_name`
    base_dir_name=`basename $full_dir_name | cut --d="." --f=1`
    output_ft_yes_data=`echo "$base_name"  |  sed 's/SD/FT_YES/g'`
    output_ft_no_data=`echo "$base_name"  |  sed 's/SD/FT_NO/g'`
    echo "$APP_NAME : INFO : " $output_ft_no_data $output_ft_yes_data
    log_file="${base_dir_name}.log"
    

    mkdir -p $OUTPUT_DIR/$base_dir_name
    
    if [ $? -ne 0 ];
    then 
	echo "$APP_NAME : ERROR : Can not create the $dir_name repository"
	exit 1
    else
	echo "$APP_NAME : INFO : $dir_name created"
    fi


    echo "$APP_NAME : INFO : $input_sd_data copying ..."
    
    #cp $input_sd_data $OUTPUT_DIR/$base_dir_name/.
    if [ $? -ne 0 ];
    then 
	echo "$APP_NAME : ERROR : Can not move $input_sd_data file"
	exit 1
    else
	echo "$APP_NAME : INFO : brio file moved in $base_dir_name successfully"
    fi


    echo "$APP_NAME : INFO : Prepare $OUTPUT_DIR/modules.conf"
    cat $input_module_file |  sed -e "s#XXXXXXXXXXXXXXXXXXXXX#$OUTPUT_DIR/$base_dir_name/$output_ft_no_data#g" > /tmp/module1.conf
    cat /tmp/module1.conf |  sed -e "s#YYYYYYYYYYYYYYYYYYYYY#$OUTPUT_DIR//$base_dir_name//$output_ft_yes_data#g" > $OUTPUT_DIR//$base_dir_name/modules.conf
    echo "$APP_NAME : INFO : Prepare $OUTPUT_DIR/module_manager.conf"
    cat $input_module_mgr_file | sed -e "s#ZZZZZZZZZZZZZZZZZZZZ#${OUTPUT_DIR}//${base_dir_name}//modules.conf#g"  > $OUTPUT_DIR//$base_dir_name/module_manager.conf
    
    

    echo "$APP_NAME : COMMAND processing  "
    bxdpp_processing  \
	-c "${OUTPUT_DIR}/$base_dir_name/module_manager.conf" \
	--load-dll "Falaise_Digitization@${FALAISE_BUILD_DIR}/BuildProducts/lib/x86_64-linux-gnu/Falaise/modules" \
	-P "fatal" \
	-i "${input_sd_data}" \
	--modulo 1 \
	-m "faketriggercut" \
	-M 100000 \
	--preserve-existing-files &

    #>> $OUTPUT_DIR//$base_dir_name/$log_file 
    
    if [ $? -ne 0 ];
    then 
	echo "$APP_NAME : ERROR : Bad day my padawan, bxdpp_processing can not process ${input_sd_data} ..."
	exit 1
    else
	echo "$APP_NAME : COMMAND done suceesfully!"
    fi
    



done




# input_sd_data="${FALAISE_DIGITIZATION_TESTING_DIR}/data/BiPo214-source_strips_bulk_SD_100_events.brio"
# output_sd_data="${FALAISE_DIGITIZATION_TESTING_DIR}/data/trigger_yes_cut.brio"

# echo "Running bxdpp_processing..." >&2
# bxdpp_processing  \
#     -c "${PROCESSING_DIR}/module_manager.conf" \
#     --load-dll "Falaise_Digitization@${FALAISE_BUILD_DIR}/BuildProducts/lib/x86_64-linux-gnu/Falaise/modules" \
#     -P "fatal" \
#     -i "${input_sd_data}" \
#     --modulo 1 \
#     -m "faketriggercut" \
#     --preserve-existing-files

#  #    -o "${output_sd_data}" \
# echo "Stopped." >&2

# exit 0

# end
