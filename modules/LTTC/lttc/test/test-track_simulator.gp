set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

set object 1 circle at 0,0 size 7.0 front fillstyle solid 1.0 fillcolor rgb "red"

# ! ./test-track_simulator > test-track_simulator.data

plot [-500.0:500.0][-1000:1000] \
 			  "test-track_simulator.data" index 0 title "Tracker" with lines, \
 			  "test-track_simulator.data" index 1 title "Track 1" with points pt 6 ps 0.15, \
 			  "test-track_simulator.data" index 2 title "Track 2" with points pt 6 ps 0.15 
pause -1 "Hit [Enter]..."

set terminal push
set terminal pdfcairo
set output "test-track_simulator.pdf"
replot
set output
set terminal pop

unset object

# ! rm -f test-track_simulator.data

# end
