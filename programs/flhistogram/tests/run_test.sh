#!/usr/bin/bash

which gsl-randist 2>&1 > /dev/null
if [ $? -ne 0 ]; then
    echo >&2 "[error] cannot find gsl-randist"
    exit 1
fi
gsl-randist 314159 100000 gaussian 0.35 > _numbers.data

which flhistogram 2>&1 > /dev/null
if [ $? -ne 0 ]; then
    echo >&2 "[error] cannot find flhistogram"
    exit 1
fi
flhistogram -3.0 +3.0 127 12 < _numbers.data > _numbers.his

less _numbers.his

cat > _numbers.gp <<EOF
set grid	   
set xlabel "Value"
set ylabel "Counts"
plot '_numbers.his' using (0.5*(column(1)+column(2))):3 with histeps notitle
pause -1

EOF

gnuplot _numbers.gp

rm -f _numbers.data
rm -f _numbers.his
rm -f _numbers.gp

exit 0
