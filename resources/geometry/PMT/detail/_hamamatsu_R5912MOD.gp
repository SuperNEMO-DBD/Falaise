# -*- mode: gnuplot; -*-

! ./proc_hamamatsu_R5912MOD.py  > hamamatsu_R5912MOD_shape_0.data

mm=1
set xrange [*:*]
set yrange [0.0:*]
set xlabel "z (mm)"
set ylabel "r (mm)"
set size ratio -1
set grid 
set title "Shape of a Hamamatsu R5912MOD PMT"
plot 'hamamatsu_R5912MOD_shape_0.data' notitle with lines lw 3
pause -1 "Hit [Return]..."

#rm -f h.data

# end
