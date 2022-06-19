set terminal qt
set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1
set title "Tracker, event, tracks, hits"

plot [-500.0:500.0][-1000:1000] \
     "tracker-geometry.data" title "Tracker" with lines, \
     "tracker-dead-cells.data" title "Dead cells" with lines lt 7 lw 2, \
     "tracks.data" title "Event tracks" with points pt 6 ps 0.25 , \
     "hits-0.data" title "Event hits" with lines lw 1 
pause -1 "Hit [Enter]..."

set title "Tracker, event, tracks, hits, clusters, lines"
plot [-500.0:500.0][-1000:1000] \
     "tracker-geometry.data" title "Tracker" with lines, \
     "tracker-dead-cells.data" title "Dead cells" with lines lt 7 lw 2, \
     "tracks.data" title "Event tracks" with points pt 6 ps 0.25 , \
     "hits-0.data" title "Event hits" with lines lw 1 , \
     "hit_clustering.data" index 0 title "Unclustered hits" with lines lt 8 lw 2 , \
     "hit_clustering.data" index 1::1 using 1:2:3 title "Clusters of hits" with lines lt 10 lw 2 lc variable
pause -1 "Hit [Enter]..."

set terminal push
set terminal pdfcairo
set output "tracks-1.pdf"
replot
set output
set terminal pop


pause -1 "Hit [Enter]..."

# end