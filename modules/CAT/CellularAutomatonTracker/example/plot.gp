set grid
mm=1.
cm=10*mm

set xlabel "x"
set ylabel "y"
set zlabel "z"
set xrange [-50*cm:+50*cm]
set yrange [-100*cm:+100*cm]

set size ratio -1
set arrow 1 from 0,-275*cm to 0,+275*cm nohead lt 4
plot \
     'plot.data' index 0 title "Hits" with lines, \
     'plot.data' index 1 title "Clusters \#0" with lines, \
     'plot.data' index 2 title "Clusters \#1" with lines, \
     'plot.data' index 3 title "Clusters \#2" with lines
pause -1 'Hit [Enter]'
# splot  \
#      'plot.data' index 0 title "Hits" with lines, \
#      'plot.data' index 1 title "Clusters" with lines, \
#      'plot.data' index 2 title "Clusters" with lines, \
#      'plot.data' index 3 title "Clusters" with lines
# pause -1 'Hit [Enter]'

unset arrow