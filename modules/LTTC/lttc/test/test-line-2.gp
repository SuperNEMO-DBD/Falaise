set grid
set xrange [-150:150]
set yrange [-250:250]
set xzeroaxis 
set yzeroaxis 
set size ratio -1
set key out font ",8"

set title "LTTC - Orthogonal projection on a line\nOrth. proj. on a fitted line\n(with uncertainties on theta and r)"
plot \
     'tl.data' index 0 title "Line 1" with lines, \
     'tl.data' index 1 title "M" with lines, \
     'tl.data' index 2 title "H" with lines, \
     'tl.data' index 3 title "Line MH" with lines, \
     'tl.data' index 4:7:1 title "Fitted line" with lines lc rgb "magenta", \
     'tl.data' index 8  title "N1"          with lines lc rgb "red", \
     'tl.data' index 10 title "Rand proj 1" with dots  lc rgb "red", \
     'tl.data' index 11 title "N2"          with lines lc rgb "green", \
     'tl.data' index 13 title "Rand proj 2" with dots  lc rgb "green", \
     'tl.data' index 14 title "N3"          with lines lc rgb "blue", \
     'tl.data' index 16 title "Rand proj 3" with dots  lc rgb "blue", \
     'tl.data' index 17 title "N4"          with lines lc rgb "brown", \
     'tl.data' index 19 title "Rand proj 4" with dots  lc rgb "brown", \
     'tl.data' index 20 title "N5"          with lines lc rgb "orange", \
     'tl.data' index 22 title "Rand proj 5" with dots  lc rgb "orange", \
     'tl.data' index 9  title "Fitted pN 1" with lines lc rgb "black" lw 1, \
     'tl.data' index 12 title "Fitted pN 2" with lines lc rgb "black" lw 1, \
     'tl.data' index 15 title "Fitted pN 3" with lines lc rgb "black" lw 1, \
     'tl.data' index 18 title "Fitted pN 4" with lines lc rgb "brown" lw 1, \
     'tl.data' index 21 title "Fitted pN 5" with lines lc rgb "black" lw 1
    
pause -1 "Hit [Enter]..."

set terminal push
set terminal pdfcairo transparent enhanced fontscale 0.5 size 8in, 8in
set output "test-line-2.pdf"
replot
set output
set terminal pop

# end
