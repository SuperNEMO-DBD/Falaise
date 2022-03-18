set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

plot [-3.0:6.0][-5:5] \
 			  "test-rectangle.data" index 0 title "R1" with lines , \
 			  "test-rectangle.data" index 1 title "R2" with lines , \
 			  "test-rectangle.data" index 2 title "R3" with lines  , \
 			  "test-rectangle.data" index 3 title "R4" with lines , \
 			  "test-rectangle.data" index 4 title "R1 (vertexes)" with points pt 6 ps 1.5 , \
 			  "test-rectangle.data" index 5 title "R4 (vertexes)" with points pt 6 ps 1.5  , \
 			  "test-rectangle.data" index 6 title "R2 (impact)" with linespoints pt 6 ps 0.5 
pause -1 "Hit [Enter]..."

set terminal push
set terminal pdfcairo
set output "test-rectangle.pdf"
replot
set output
set terminal pop

unset object

# end
