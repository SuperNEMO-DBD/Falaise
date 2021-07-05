#
# Usage: ./process_one.sh <number_of_dead_cells>
#

#==========================================================================================================

#INPUT="~/SuperNEMO/MC/20200305-1134/se82_2nubb-sim.brio"         # input simulated data from flsimulate
INPUT="~/SuperNEMO/MC/20200315-2347/se82_2nubb-sim.brio"         # input simulated data from flsimulate  -  10k

FILE=se82_2nubb                                                  # name of the output file

FALAISE="/vols/build/snemo/Falaise.build/bin/"                   # Falaise binaries location
WHERE=output                                                     # output directory

#==========================================================================================================

echo -e "\nUsing simulation in "$INPUT

DEAD_CELLS=$1

mkdir -p ${WHERE}

# Purge output
rm -f ${WHERE}/${FILE}*

# Reconstruct only with the calibration
echo -e "\nReconstruct only with the calibration..."
${FALAISE}flreconstruct -i ${INPUT} -p ../official-2.0.0_only_calibration.conf -o ${WHERE}/${FILE}-cal.brio &> /dev/null
if [[ "$?" -ne "0" ]]; then
    echo "Problem in flreconstruct..."
    exit
fi

# Create Dead Cells conf file
cp DeadCellsModule.conf.in ${WHERE}/DeadCellsModule.conf
sed -i "s/XXXXX/${WHERE}\/${FILE}-dc.brio/g" ${WHERE}/DeadCellsModule.conf
sed -i "s/YYYYY/${DEAD_CELLS}/g" ${WHERE}/DeadCellsModule.conf

# Run the Dead Cells module
echo -e "\nRun the Dead Cells module...\n"
${FALAISE}flreconstruct -i ${WHERE}/${FILE}-cal.brio -p ${WHERE}/DeadCellsModule.conf
if [[ "$?" -ne "0" ]]; then
    exit
fi
mv random_dead_cells_${DEAD_CELLS}.txt ${WHERE}

# Create Lazarus conf file
cp LazarusModule.conf.in ${WHERE}/LazarusModule.conf
sed -i "s/XXXXX/${WHERE}\/${FILE}-lazarus.brio/g" ${WHERE}/LazarusModule.conf
sed -i "s/YYYYY/${WHERE}\/random_dead_cells_${DEAD_CELLS}.txt/g" ${WHERE}/LazarusModule.conf

# Run the Lazarus module
echo -e "\nRun the Lazarus module...\n"
${FALAISE}flreconstruct -i ${WHERE}/${FILE}-dc.brio -p ${WHERE}/LazarusModule.conf
if [[ "$?" -ne "0" ]]; then
    exit
fi

# Run the remaining reconstruction
echo -e "\nRun the remaning reconstruction..."
${FALAISE}flreconstruct -i ${WHERE}/${FILE}-lazarus.brio -p ../official-2.0.0_from_CAT.conf -o ${WHERE}/${FILE}-reco.brio &> /dev/null
if [[ "$?" -ne "0" ]]; then
    exit
fi

# Run the sensitivity module
cd ${WHERE}
/home/hep/pfranchi/SuperNEMO/MC/do_sensitivity.sh ${FILE}-reco
cd ..

# Find the efficiency of track reconstruction and writes into a file
~/SuperNEMO/SuperNEMO-DeadCellsModule/scripts/efficiency -i ${WHERE}/${FILE}-reco.root 




