set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

set object 1 circle at 0,0 size 10.0 front fillstyle solid 1.0 fillcolor rgb "red"

plot [:][:] \
 			  "test-kinked_curve.data" index 0 title "Curve" with lines, \
 			  "test-kinked_curve.data" index 1 title "Kinked" with lines
pause -1 "Hit [Enter]..."


set terminal push
set terminal pdfcairo
set output "test-kinked_curve.pdf"
replot
set output
set terminal pop


unset object

# end
