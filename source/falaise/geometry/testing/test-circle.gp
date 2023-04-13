set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

# set object 1 circle at 0,0 size 5.0 front fillstyle solid 1.0 fillcolor rgb "red"

plot [-500:1500][-1500:1500] \
     "test-circle.data" index 0 title "Circle" with lines lw 2, \
     "test-circle.data" index 1 title "Circle (rotated/translated)" with lines lw 2
pause -1 "Hit [Enter]..."


# set terminal push
# set terminal pdfcairo
# set output "test-circle.pdf"
# replot
# set output
# set terminal pop


unset object

# end
