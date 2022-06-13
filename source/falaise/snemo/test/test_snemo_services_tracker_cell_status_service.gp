#

set xlabel "y (mm)"
set ylabel "x (mm)"
set cbrange [0:32]
set cblabel "Status"
set size ratio -1
set grid
set palette defined ( \
    0      "green"   ,  0.5 "green" , \
    0.501  "black"   ,  1.5 "black" , \
    1.501  "red"     ,  2.5 "red" , \
    2.501  "orange"  ,  4.5 "orange" , \
    4.501  "yellow"  ,  8.5 "yellow" , \
    8.501  "magenta" , 16.5 "magenta" , \
    16.501 "brown"   , 32.5 "brown"  \
    )
set object rectangle from -2500,-430 to +2500,+430 back fillstyle empty lw 2
set xrange [-2600:2600]
set yrange [-530:530]
set multiplot title "SuperNEMO tracker cell status for two timestamps"  layout 2,1 spacing screen 0, char 0
set label 1 "2022-06-13" at 1000,200 front
plot \
     'test-tracker_dead_cells-draw-period1.data' i 0 using 1:2:3 notitle w image , \
     'test-tracker_dead_cells-draw-period1.data' i 1 using 1:2:3 notitle w image
set label 1 "2022-06-25" at 1000,200 front
plot \
     'test-tracker_dead_cells-draw-period2.data' i 0 using 1:2:3 notitle w image , \
     'test-tracker_dead_cells-draw-period2.data' i 1 using 1:2:3 notitle w image
pause -1
unset multiplot
# end