# $ a2ps --columns=2 --chars-per-line=180 b.log -o b.ps
NO_ANIMATION = 1

set grid
set key out
set ylabel "Counts"
set xzeroaxis lt -1
set yzeroaxis lt -1

set xlabel "r1-r2"
plot [:][0.0:] \
     "test-lttc_algo-circle_hist.data" index 0 using (0.5*(($1)+($2))):($3) notitle with impulses lw 2
pause -1 "Hit [Enter]..."

set xlabel "r1+r2 (theta=0)"
plot [:][0.0:] \
     "test-lttc_algo-circle_hist.data" index 1 using (0.5*(($1)+($2))):($3) notitle with impulses lw 2
pause -1 "Hit [Enter]..."

set xlabel "r1+r2 (theta=pi/2)"
plot [:][0.0:] \
     "test-lttc_algo-circle_hist.data" index 2 using (0.5*(($1)+($2))):($3) notitle with impulses lw 2
pause -1 "Hit [Enter]..."

# end
