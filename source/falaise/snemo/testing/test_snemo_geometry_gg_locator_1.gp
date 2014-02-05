# 

mm=1
m=1000*mm
dim=5*m
view=dim*1.2
set xrange [-view:+view]
set yrange [-view:+view]
set xlabel "x (mm)"
set ylabel "y (mm)"
set size ratio -1
set grid

plot \
     'f1a.data' using 1:2 with dots, \
     'f2a.data' using 1:2 with dots
pause -1 "Hit [Enter]..."

plot \
     'f1b.data' using 1:2 with dots, \
     'f2b.data' using 1:2 with dots
pause -1 "Hit [Enter]..."

#, \
#     'f2.data' using 1:2 with points pt 6 ps 0.25	

set zlabel "z (mm)"
splot \
     'f1a.data' using 1:2:3 with dots, \
     'f2a.data' using 1:2:3 with dots
pause -1 "Hit [Enter]..."



# end
