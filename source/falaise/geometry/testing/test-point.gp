set grid
set xrange [0:4]
set yrange [0:3]
set yzeroaxis 
set yzeroaxis 
set size ratio -1
set title "Fitted point (with oriented elliptic probability shape)"
plot \
     'test-point.data' index 0 title "Fitted point shape" with lines, \
     'test-point.data' index 1 title "Random point at 1.5 sigma" with point pt 6 ps 0.2
pause -1 "Hit [Enter]..."

# set terminal push
# set terminal pdfcairo transparent enhanced fontscale 0.5 size 5in, 5in
# set output "test-point.pdf"
# replot
# set output
# set terminal pop

# end