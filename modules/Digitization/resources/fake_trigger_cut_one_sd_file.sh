#!/usr/bin/env bash
# fake_trigger_cut_one_sd_file.sh
debug=1

APP_NAME="fake_trigger_cut_one_sd_file.sh"

echo "Starting..." >&2
PROCESSING_DIR="${FALAISE_DIGITIZATION_TESTING_DIR}/config/processing"

# To do : encapsulate this script into an other for all SD files
# Change INPUT and OUTPUT dir when your SD file is located

INPUT_DIR="${DATA_NEMO_PERSO_DIR}/simulated_data_raw_FT_DT/run_test"
OUTPUT_DIR="${DATA_NEMO_PERSO_DIR}/simulated_data_raw_FT_DT/run_test"

function usage(){
echo "--------------"
echo "Goal : Use the fake trigger cut to split an SD file into two files FT_YES_SD and FT_NO_SD"
echo "Definition of the base fake trigger is 1 PMT and 3 Geiger cells without spatial or time association"
echo "The fake trigger definition can change..."
echo "--------------" 
echo "How to use it"
echo " " 
echo "./fake_trigger_cut_one_sd_file.sh"
echo " " 
echo "--------------"
echo "Exemple : "
echo "./fake_trigger_cut_one_sd_file.sh"
echo " "
}

#### ->MAIN<- #####

if [ $debug -ne 0 ];
then
    echo " "
    echo "*****************************************************"
    echo "**** Welcome in wonderful shell script world !!! ****"
    echo "*****************************************************"
    echo " "
fi

while [ -n "$1" ]; do
    arg="$1"
    arg_value="$2"
    if [ "x$arg" = "x-h" -o "x$arg" = "x--help" ]; then
        usage
        exit 0
    fi
    shift 2
done

input_module_file="${PROCESSING_DIR}/modules.conf"
input_module_mgr_file="${PROCESSING_DIR}/module_manager.conf"

input_file=`ls $INPUT_DIR/*SD.brio`

for file in $input_file
do
    echo $file
    input_sd_data=$file
    mkdir $OUTPUT_DIR/FT_YES $OUTPUT_DIR/FT_NO
    
    full_dir_name=`echo "$input_sd_data"`
    base_name=`basename $full_dir_name`
    base_dir_name=`basename $full_dir_name | cut --d="." --f=1`
    output_ft_yes_data=`echo "$base_name"  |  sed 's/SD/FT_YES/g'`
    output_ft_no_data=`echo "$base_name"  |  sed 's/SD/FT_NO/g'`
    echo "$APP_NAME : INFO : " $output_ft_no_data $output_ft_yes_data
    log_file=$OUTPUT_DIR/logger.log
 
    echo "$APP_NAME : INFO : Prepare $OUTPUT_DIR/modules.conf"
    cat $input_module_file |  sed -e "s#XXXXXXXXXXXXXXXXXXXXX#$OUTPUT_DIR/$output_ft_no_data#g" > /tmp/module1.conf
    cat /tmp/module1.conf |  sed -e "s#YYYYYYYYYYYYYYYYYYYYY#$OUTPUT_DIR/$output_ft_yes_data#g" > $OUTPUT_DIR/modules.conf
    echo "$APP_NAME : INFO : Prepare $OUTPUT_DIR/module_manager.conf"
    cat $input_module_mgr_file | sed -e "s#ZZZZZZZZZZZZZZZZZZZZ#${OUTPUT_DIR}/modules.conf#g"  > $OUTPUT_DIR/module_manager.conf
    
    echo $OUTPUT_DIR
    
    # Activate Bayeux environment (for bxdpp_processing command)
    do_bayeuxbrew_setup
    
    echo "$APP_NAME : COMMAND processing  "
    bxdpp_processing  \
    	-c "${OUTPUT_DIR}/module_manager.conf" \
    	--load-dll "Falaise_Digitization@${FALAISE_BUILD_DIR}/BuildProducts/lib/Falaise/modules" \
    	-P "fatal" \
    	-i "${input_sd_data}" \
    	--modulo 1 \
    	-m "faketriggercut" \
    	-M 100000 \
    	--preserve-existing-files >> ${log_file} 2>&1

    #>> $OUTPUT_DIR/$base_dir_name/$log_file 
    
    if [ $? -ne 0 ];
    then 
	echo "$APP_NAME : ERROR : Bad day my padawan, bxdpp_processing can not process ${input_sd_data} ..."
	exit 1
    else
	echo "$APP_NAME : COMMAND done suceesfully!"
	sleep 5s
	echo "Moving produced files in good directories"
	mv ${OUTPUT_DIR}/$output_ft_no_data $OUTPUT_DIR/FT_NO
	mv ${OUTPUT_DIR}/$output_ft_yes_data $OUTPUT_DIR/FT_YES
    fi

done

# end
