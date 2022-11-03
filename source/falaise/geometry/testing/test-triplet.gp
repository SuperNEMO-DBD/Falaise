set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

set object 1 circle at 0,0 size 2.0 front fillstyle solid 1.0 fillcolor rgb "red"

set title "Falaise/Geometry -- test-triplet"
plot [-500.0:500.0][-500:500] \
     "test-triplet.data" index 0 title "Triplet" with points pt 6 ps 0.75 lt 7, \
     "test-triplet.data" index 1 title "Circle"  with lines  lt 8 lc rgb "blue"

pause -1 "Hit [Enter]..."


# set terminal push
# set terminal pdfcairo
# set output "test-triplet.pdf"
# replot
# set output
# set terminal pop


unset object

# end
