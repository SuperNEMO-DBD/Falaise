set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

# set object 1 circle at 0,0 size 7.0 front fillstyle solid 1.0 fillcolor rgb "red"

plot [-500.0:500.0][-1000:1000] \
     "test-track_simulator.data" index 0 title "Tracker" with lines, \
     "test-track_simulator.data" index 1 title "Track 1" with linespoints lw 1 pt 6 ps 0.5, \
     "test-track_simulator.data" index 2 title "Track 2" with linespoints lw 1 pt 6 ps 0.5 
pause -1 "Hit [Enter]..."

set terminal push
set terminal pdfcairo
set output "test-track_simulator.pdf"
replot
set output
set terminal pop

unset object

set xlabel "x"
set ylabel "y"
set zlabel "z"

set view equal xyz

splot [-500.0:500.0][-1000:1000][:] \
      "test-track_simulator.data" index 0 using 1:2:(-1000.0) title "Tracker" with lines, \
      "test-track_simulator.data" index 3 title "Track 1 (3D)" with linespoints lw 1 pt 6 ps 0.15, \
      "test-track_simulator.data" index 4 title "Track 2 (3D)" with linespoints lw 1 pt 6 ps 0.15 
pause -1 "Hit [Enter]..."

set terminal push
set terminal pdfcairo
set output "test-track_simulator2.pdf"
replot
set output
set terminal pop


# end
