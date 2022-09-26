set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

set object 1 circle at 0,0 size 10.0 front fillstyle solid 1.0 fillcolor rgb "red"

# ! ./test-lt_1 

set title "Tracker, event, tracks and hits"
plot [0.0:500.0][-1000:1000] \
 			  "tracker-geometry.data" title "Tracker" with lines, \
 			  "tracker-dead-cells.data" title "Dead cells" with lines lt 7 lw 2, \
 			  "tracks.data" title "Event tracks" with points pt 6 ps 0.35 , \
 			  "hits.data" title "Event hits" with lines lw 2
pause -1 "Hit [Enter]..."


set terminal push
set terminal pdfcairo
set output "test-lt_1.pdf"
replot
set output
set terminal pop

unset object

set xlabel "theta"
set ylabel "r"
set xzeroaxis lt -1
set yzeroaxis lt -1
set size noratio

# set palette maxcolors 11
# set palette defined ( 0 '#ffffff',\
#                       1 '#eeeeee',\
#                       2 '#dddddd',\
#                       3 '#cccccc',\
#                       4 '#bbbbbb',\
#                       5 '#aaaaaa',\
#                       6 '#999999',\
#                       7 '#888888',\
#                       8 '#777777',\
#                       9 '#666666',\
#                       10 '#555555',\
#                       11 '#444444',\
#                       12 '#333333',\
#                       13 '#222222',\
#                       14 '#11111',\
#                       15 '#000000')
set view map # scale 1
set samples 600, 600
set isosamples 600, 600
unset surface 
set style data lines
set xyplane relative 0
set pm3d implicit at b
# set palette negative nops_allcF maxcolors 0 gamma 1.5 gray
NO_ANIMATION = 1
set palette gray negative
# set palette color negative
#set palette defined (0 "#ffffff", 2 "#bbbbbb", 4 "#777777", 6 "#333333", 8 "#222222", 16 "#111111", 32 "#000000" )
#set cbrange [0:32]
set title "Step-1 t-r bin map"
# plot [:][:] \
#        "step1-bins.data" using 1:2:(($3)/10) title "Step-1 : Bins" with points lt 4 pt 6 ps variable 
# pause -1 "Hit [Enter]..."
splot [:][:] \
       "step1-bins.data" title "" with lines lw 1
pause -1 "Hit [Enter]..."

unset view 
unset pm3d 
unset palette 

set title "Step-1 t-r sorted bin map and clusters"
plot [:][:] \
     "step1-bins.data" using 1:2:(($3) / 5)             title "Step-1 : Bins"        with points lt 4 pt 8 ps variable , \
     "step1-sorted-bins.data" using 1:2:(($3)/10)       title "Step-1 : Sorted bins" with points lt 6 pt 6 ps variable , \
     "step1-clustering.data"  index 1::1 using 1:2:($3) title "Step-1 : Clusters"    with lines  lw 0.75 lc variable

pause -1 "Hit [Enter]..."
pause -1 "Hit [Enter]..."

set title "Step-1 t-r sorted bin map and selected clusters"
plot [:][:] \
     "step1-sorted-bins.data" using 1:2:(($3)/10)       title "Step-1 : Sorted bins" with points lt 6 pt 6 ps variable , \
     "step1-clustering.data"  index 1:6 using 1:2:($3) title "Step-1 : 5 clusters"    with lines  lw 0.75 lc variable

pause -1 "Hit [Enter]..."
pause -1 "Hit [Enter]..."

# "step2-sorted-bins.data" using 1:2:(($3)/10)       title "Step-2 : Sorted bins" with points lt rgb "red" pt 4 ps variable , \
     # "step2-clustering.data"  index 1::1 using 1:2:($3) title "Step-2 : Clusters"    with lines  lw 1.5 lc variable

set title "Step-2 t-r sorted bin map and clusters"
plot [:][:] \
     "step2-sorted-bins.data" using 1:2:(($3)/10)       title "Step-2 : Sorted bins" with points lt rgb "red" pt 4 ps variable , \
     "step2-clustering.data"  index 1::1 using 1:2:($3) title "Step-2 : Clusters"    with lines  lw 1.5 lc variable

pause -1 "Hit [Enter]..."
pause -1 "Hit [Enter]..."

set title "Steps-1/2 t-r sorted bin map and clusters"
plot [:][:] \
     "step1-bins.data"        using 1:2:(($3) / 5)             title "Step-1 : Bins" with points lt 4 pt 8 ps variable , \
     "step1-sorted-bins.data" using 1:2:(($3)/10)       title "Step-1 : Sorted bins" with points lt 6 pt 6 ps variable , \
     "step1-clustering.data"  index 1::1 using 1:2:($3) title "Step-1 : Clusters"    with lines  lw 1.5 lc variable , \
     "step2-sorted-bins.data" using 1:2:(($3)/10)       title "Step-2 : Sorted bins" with points lt rgb "red" pt 4 ps variable , \
     "step2-clustering.data"  index 1::1 using 1:2:($3) title "Step-2 : Clusters"    with lines  lw 0.75 lc variable

pause -1 "Hit [Enter]..."
pause -1 "Hit [Enter]..."

unset object

set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1
set title "Tracker, event, tracks, hits, lines"
nlines=`grep "#@step2-lines" step2-lines.data | wc -l`
iline_max=nlines-1
min2(x,y)=x<y?x:y
iline_last=min2(4, iline_max)
print "***** nlines = ", nlines
print "***** iline_max = ", iline_max
plot [-500.0:500.0][-1000:1000] \
 			  "tracker-geometry.data" title "Tracker" with lines, \
 			  "tracker-dead-cells.data" title "Dead cells" with lines lt 7 lw 2, \
 			  "tracks.data" title "Event tracks" with points pt 6 ps 0.35 , \
 			  "hits.data" title "Event hits" with lines lw 2 , \
 			  "step2-lines.data" index 0:iline_last:1 title "Cluster lines" with lines lt 2 lw 1.5
pause -1 "Hit [Enter]..."

# ! rm -f test-lt_1.data  
# end