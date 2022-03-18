set grid
set xrange [-15:15]
set yrange [-15:15]
set xzeroaxis 
set yzeroaxis 
set size ratio -1
set key out font ",8"

set title "LTTC - Line in XY-plane"
plot \
     'test-line-3.data' w l
   
pause -1 "Hit [Enter]..."

