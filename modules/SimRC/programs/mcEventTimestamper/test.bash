cut -d' ' -f1 times-random-exp.lis | gsl-histogram 0 8e7 100 > times-random-exp.his

cat > times-random-exp.gp <<EOF

set grid
set xlabel "Time (s)"
set ylabel "Number of decays/bin"
plot 'times-random-exp.his' using 1:3 with histeps lw 2
pause -1

EOF

gnuplot times-random-exp.gp

exit 0
