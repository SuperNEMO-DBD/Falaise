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
     "test-sngeometry.data" index 0 title "Tracker" with lines, \
     "test-sngeometry.data" index 1 title "Segment" with linespoint pt 6 ps 0.75 lw 2 lc rgb "black" , \
     "test-sngeometry.data" index 2 title "Crossed cells" with lines lw 2 lc rgb "red" 
     
pause -1 "Hit [Enter]..."

set terminal push
set terminal pdfcairo
set output "test-sngeometry.pdf"
replot
set output
set terminal pop

unset object


# end
