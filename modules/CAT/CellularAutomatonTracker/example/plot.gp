set grid
mm=1.
cm=10*mm

set xlabel "x"
set ylabel "y"
set zlabel "z"
set xrange [-50*cm:+50*cm]
set yrange [-75*cm:+75*cm]

set size ratio -1
set arrow 1 from 0,-75*cm to 0,+200*cm nohead lt 4
set key outside
plot \
     'plot.data' index 0 title "Hits" with lines, \
     'plot.data' index 1 title "Clusters \#0" with lines, \
     'plot.data' index 2 title "Clusters \#1" with lines, \
     'plot.data' index 3 title "Clusters \#2" with lines, \
     'plot.data' index 4 title "Clusters \#3" with lines, \
     'plot.data' index 5 title "Clusters \#4" with lines, \
     'plot.data' index 6 title "Clusters \#5" with lines, \
     'plot.data' index 7 title "Clusters \#6" with lines
pause -1 'Hit [Enter]'

unset arrow