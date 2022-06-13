# Usage: ./process.sh [no arguments]
# Runs the full process
#

rm -f efficiency.txt

for DEAD_CELLS in {0..2000..100}
#for DEAD_CELLS in {0..500..25}
do
    
    echo ${DEAD_CELLS} "dead cells..."
    ./process_one.sh ${DEAD_CELLS} &> eff.log

    EFFICIENCY=`cat eff.log | grep Efficiency | awk '{print $2}'`
    ZERO=`cat eff.log | grep Zero | awk '{print $2}'`
    SINGLE=`cat eff.log | grep Single | awk '{print $2}'`
    MORE=`cat eff.log | grep More | awk '{print $2}'`

    echo $DEAD_CELLS $EFFICIENCY $ZERO $SINGLE $MORE >> efficiency.txt

    tail -n4 eff.log

    rm -f eff.log

done

python plot_efficiency.py
