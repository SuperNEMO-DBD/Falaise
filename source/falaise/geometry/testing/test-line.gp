set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

set title "Falaise/Geometry -- test-line"

set object 1 circle at 0,0 size 1.0 back fillstyle solid 1.0 fillcolor rgb "red"
plot[-500.0:500.0][-500:500] \
			  "test-line.data" index 0 title "Dummy" with lines, \
			  "test-line.data" index 13 title "AB" with lines, \
			  "test-line.data" index 14 title "perp1" with lines, \
			  "test-line.data" index 15 title "perp2" with lines, \
			  "test-line.data" index 16 title "perp3" with lines
pause -1 "Hit [Enter]..."

plot[-500.0:500.0][-500:500] \
			  "test-line.data" index 0 title "Dummy" with lines, \
			  "test-line.data" index 11 title "D1" with lines, \
			  "test-line.data" index 12 title "D2" with lines			  
pause -1 "Hit [Enter]..."

plot[-500.0:500.0][-500:500] \
			  "test-line.data" index 0 title "Dummy" with lines, \
			  "test-line.data" index 1 title "Line" with lines lw 2, \
			  "test-line.data" index 2 title "Line2" with lines lw 2, \
			  "test-line.data" index 3 title "Line3" with lines lw 2, \
			  "test-line.data" index 4 title "Line4" with lines lw 2, \
			  "test-line.data" index 5 title "perpLine1" with lines lw 2, \
			  "test-line.data" index 6 title "perpLine2" with lines lw 2, \
			  "test-line.data" index 7 title "Line5" with lines lw 2, \
			  "test-line.data" index 8 title "Line6" with lines lw 2, \
			  "test-line.data" index 9 title "perpLine5" with lines lw 2, \
			  "test-line.data" index 10 title "perpLine6" with lines lw 2			  
pause -1 "Hit [Enter]..."

# set terminal push
# set terminal pdfcairo
# set output "test-line.pdf"
# replot
# set output
# set terminal pop

unset object

# end
