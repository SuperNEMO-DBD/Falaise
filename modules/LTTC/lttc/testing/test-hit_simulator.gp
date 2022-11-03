set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

plot [-500.0:500.0][-1500:1500] \
     "test-hit_simulator.data" index 0 title "Tracker"  with lines      linecolor rgb "grey", \
     "test-hit_simulator.data" index 1 title "Track #0" with lines lw 1 linecolor rgb "red" , \
     "test-hit_simulator.data" index 2 title "Track #1" with lines lw 1 linecolor rgb "green" , \
     "test-hit_simulator.data" index 3 title "Track #2" with lines lw 1 linecolor rgb "magenta" , \
     "test-hit_simulator.data" index 4::1 title "Hits"  with lines lw 1 linecolor rgb "blue"
pause -1 "Hit [Enter]..."

set terminal push
set terminal pdfcairo
set output "test-hit_simulator.pdf"
replot
set output
set terminal pop


set xlabel "x"
set ylabel "y"
set zlabel "z"
set view equal xyz
set xyplane at -1500

splot [-500.0:500.0][-1500:1500][-1500:1500] \
     "test-hit_simulator.data" index 0 u 1:2:3 title "Tracker"  with lines      linecolor rgb "grey", \
     "test-hit_simulator.data" index 1 u 1:2:3 title "Track #0" with lines lw 1 linecolor rgb "red" , \
     "test-hit_simulator.data" index 2 u 1:2:3 title "Track #1" with lines lw 1 linecolor rgb "green" , \
     "test-hit_simulator.data" index 3 u 1:2:3 title "Track #2" with lines lw 1 linecolor rgb "magenta" , \
     "test-hit_simulator.data" index 4::1 u 1:2:3 title "Hits"  with lines lw 1 linecolor rgb "blue"
pause -1 "Hit [Enter]..."

unset object

# end
