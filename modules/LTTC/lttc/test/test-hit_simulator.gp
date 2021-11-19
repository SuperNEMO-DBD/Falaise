set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

set object 1 circle at 0,0 size 10.0 front fillstyle solid 1.0 fillcolor rgb "red"

# ! ./test-hit_simulator > test-hit_simulator.data

plot [0.0:500.0][-1000:1000] \
 			  "test-hit_simulator.data" index 0 title "Tracker" with lines linecolor rgb "grey", \
 			  "test-hit_simulator.data" index 1 title "Track #0" with lines lw 1  linecolor rgb "red" , \
 			  "test-hit_simulator.data" index 2 title "Track #1" with lines lw 1  linecolor rgb "green" , \
 			  "test-hit_simulator.data" index 3 title "Track #2" with lines lw 1  linecolor rgb "magenta" , \
 			  "test-hit_simulator.data" index 4 title "Hits" with lines lw 1 linecolor rgb "blue"
pause -1 "Hit [Enter]..."


set terminal push
set terminal pdfcairo
set output "test-hit_simulator.pdf"
replot
set output
set terminal pop


unset object

# ! rm -f test-hit_simulator.data  
# end
