# -*- mode: gnuplot; -*-

! ./proc_hamamatsu_R6594.py  > hamamatsu_R6594_shape_0.data

mm=1
set xrange [*:*]
set yrange [0.0:*]
set xlabel "z (mm)"
set ylabel "r (mm)"
set size ratio -1
set grid 
set title "Shape of a Hamamatsu R6594 PMT"
plot 'hamamatsu_R6594_shape_0.data' notitle with lines lw 3
pause -1 "Hit [Return]..."

#rm -f h.data

# end
