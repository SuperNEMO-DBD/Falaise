# $ a2ps --columns=2 --chars-per-line=180 b.log -o b.ps
NO_ANIMATION = 1

iloop=0

RGB(R,G,B) =  int(255.*R) * 2**16 + int(255.*G) * 2**8  + int(255.*B)
RGB1(x) = RGB(cos((x/20.)*(pi/2)), sin((x/20.)*(pi/2)), 0.0)
set palette maxcolors 12
set palette defined ( \
    0 '#ff0000',\
    1 '#00ff00',\
    2 '#0000ff',\
    3 '#00ffff',\
    4 '#ffff00',\
    5 '#ff00ff',\
    6 '#ee0000',\
    7 '#00ee00',\
    8 '#0000ee',\
    9 '#00eeee',\
   10 '#eeee00',\
   11 '#ee00ee')

RGB2(x) = \
	(x < 0.1)  ? 0xff0000 : \
	(x < 1.1)  ? 0x00ff00 : \
	(x < 2.1)  ? 0x0000ff : \
	(x < 3.1)  ? 0xff9000 : \
	(x < 4.1)  ? 0x00ffff : \
	(x < 5.1)  ? 0xff00ff : \
	(x < 6.1)  ? 0xff8000 : \
	(x < 7.1)  ? 0x00ff80 : \
	(x < 8.1)  ? 0x8000ff : \
	(x < 9.1)  ? 0x7F00FF : \
	(x < 10.1) ? 0xFF007F : \
	(x < 11.1) ? 0x990000 : \
	0xFF99FF


set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

## set object 1 circle at 0,0 size 5.0 front fillstyle solid 1.0 fillcolor rgb "red"

set title "Tracker, event, tracks and hits"
plot [-500.0:500.0][-1000:1000] \
     "test-lttc_algo-tracker-geometry.data"   title "Tracker"      with lines lw 1, \
     "test-lttc_algo-tracker-dead-cells.data" title "Dead cells"   with lines lt 7 lw 1, \
     "test-lttc_algo-tracks2.data"            title "Event tracks" with lines lw 1, \
     "test-lttc_algo-hits.data"               title "Event hits"   with lines lw 1
pause -1 "Hit [Enter]..."


# set terminal push
# set terminal pdfcairo
# set output "test-lttc_algo-1.pdf"
# replot
# set output
# set terminal pop

## unset object

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
# set view map # scale 1
# set samples 600, 600
# set isosamples 600, 600
# unset surface 
# set style data lines
# set xyplane relative 0
# set pm3d implicit at b
# set palette negative nops_allcF maxcolors 0 gamma 1.5 gray
# set palette gray negative
# set palette color negative
# set palette defined (0 "#ffffff", 2 "#bbbbbb", 4 "#777777", 6 "#333333", 8 "#222222", 16 "#111111", 32 "#000000" )
# set cbrange [0:32]
# set title "Step-1 t-r bin map"
# plot [:][:] \
#        "step1-bins.data" using 1:2:(($3)/10) title "Step-1 : Bins" with points lt 4 pt 6 ps variable 
# pause -1 "Hit [Enter]..."

step1_loop_bins_data=sprintf("test-lttc_algo-loop_%d-step1-bins.data", iloop)
step1_loop_sorted_bins_data=sprintf("test-lttc_algo-loop_%d-step1-sorted-bins.data", iloop)
step1_loop_clustering_data=sprintf("test-lttc_algo-loop_%d-step1-clustering.data", iloop)
step2_loop_sorted_bins_data=sprintf("test-lttc_algo-loop_%d-step2-sorted-bins.data", iloop)
step2_loop_clustering_data=sprintf("test-lttc_algo-loop_%d-step2-clustering.data", iloop)
step2_loop_lines_data=sprintf("test-lttc_algo-loop_%d-step2-lines.data", iloop)
print("step1_loop_bins_data        = ", step1_loop_bins_data)
print("step1_loop_sorted_bins_data = ", step1_loop_sorted_bins_data)
print("step1_loop_clustering_data  = ", step1_loop_clustering_data)
print("step2_loop_sorted_bins_data = ", step2_loop_sorted_bins_data)
print("step2_loop_clustering_data  = ", step2_loop_clustering_data)
#exit

# splot [:][:] \
#        step1_loop_bins_data title "" with lines lw 1
# pause -1 "Hit [Enter]..."

# unset view 
# unset pm3d 
# unset palette 
# set palette defined (0 "#ffffff", 2 "#bbbbbb", 4 "#777777", 6 "#333333", 8 "#222222", 16 "#111111", 32 "#000000" )
# set cbrange [0:32]

set title "Step-1 t-r sorted bin map and clusters"
plot [0.0:pi][-1000:1000] \
     step1_loop_bins_data                   using 1:2:(($3) / 5)  title "Step-1 : Bins"        with points lt 4 pt 8 ps variable , \
      step1_loop_sorted_bins_data            using 1:2:(($3)/10)   title "Step-1 : Sorted bins" with points lt 6 pt 6 ps variable , \
      'test-lttc_algo-hits-lt.data'          using 1:2:($4)        title "Hits' sinograms" with lines lw 1.0 lc variable, \
      'test-lttc_algo-hits-lt.data'          using 1:3:($4)        title "Hits' sinograms" with lines lw 1.0 lc variable, \
      step1_loop_clustering_data  index 1::1 using 1:2:($3)        title "Step-1 : Clusters"    with lines  lw 0.75 lc variable
pause -1 "Hit [Enter]..."

# set title "Step-1 t-r sorted bin map and selected clusters"
# plot [0.0:pi][:] \
#      step1_loop_sorted_bins_data           using 1:2:(($3)/10)   title "Step-1 : Sorted bins" with points lt 6 pt 6 ps variable , \
#      'test-lttc_algo-hits-lt.data'         using 1:2:($4)        title "Hits' sinograms" with lines lw 1.0 lc variable, \
#      'test-lttc_algo-hits-lt.data'          using 1:3:($4)        title "Hits' sinograms" with lines lw 1.0 lc variable, \
#      step1_loop_clustering_data  index 1:6 using 1:2:($3)        title "Step-1 : 5 clusters"  with lines  lw 0.75 lc variable
# pause -1 "Hit [Enter]..."

set title "Step-2 t-r sorted bin map and clusters"
plot [0.0:pi][:] \
      step2_loop_sorted_bins_data            using 1:2:(($3)/10) title "Step-2 : Sorted bins" with points lt rgb "red" pt 4 ps variable , \
      'test-lttc_algo-hits-lt.data'          using 1:2:($4)        title "Hits' sinograms" with lines lw 1.0 lc variable, \
      'test-lttc_algo-hits-lt.data'          using 1:3:($4)        title "Hits' sinograms" with lines lw 1.0 lc variable, \
      step2_loop_clustering_data  index 1::1 using 1:2:($3)      title "Step-2 : Clusters"    with lines  lw 1.5 lc variable
pause -1 "Hit [Enter]..."

# set title "Steps-1/2 t-r sorted bin map and clusters"
# plot [0.0:pi][:] \
#      step1_loop_bins_data                   using 1:2:(($3) / 5)  title "Step-1 : Bins" with points lt 4 pt 8 ps variable , \
#      step1_loop_sorted_bins_data            using 1:2:(($3)/10)   title "Step-1 : Sorted bins" with points lt 6 pt 6 ps variable , \
#      step1_loop_clustering_data  index 1::1 using 1:2:($3)        title "Step-1 : Clusters"    with lines  lw 0.75 lc variable , \
#      step2_loop_sorted_bins_data            using 1:2:(($3)/10)   title "Step-2 : Sorted bins" with points lt rgb "red" pt 4 ps variable , \
#      'test-lttc_algo-hits-lt.data'          using 1:2:($4)        title "Hits' sinograms" with lines lw 1.0 lc variable, \
#      'test-lttc_algo-hits-lt.data'          using 1:3:($4)        title "Hits' sinograms" with lines lw 1.0 lc variable, \
#      step2_loop_clustering_data  index 1::1 using 1:2:($3)        title "Step-2 : Clusters"    with lines  lw 2.5 lc variable
# pause -1 "Hit [Enter]..."

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
nlines=`grep "#@step2-lines" test-lttc_algo-loop_0-step2-lines.data | wc -l`
min2(x,y)=x<y?x:y

set title "Clusters"
number_of_clusters=`grep "#@nb_hit_clusters=" test-lttc_algo-hit_clustering.data | cut -d'=' -f2`
number_of_tracks=`grep "#@number_of_tracks=" test-lttc_algo-clustering_tracks.data | cut -d'=' -f2`
print "***** number_of_clusters = ", number_of_clusters
print "***** number_of_tracks   = ", number_of_tracks
last_loop=iloop
step2_last_loop_lines_data=sprintf("test-lttc_algo-loop_%d-step2-lines.data", last_loop)
plot [-500.0:500.0][-1000:1000] \
     "test-lttc_algo-tracker-geometry.data"   title "Tracker" with lines, \
     "test-lttc_algo-tracker-dead-cells.data" title "Dead cells" with lines lt 7 lw 2, \
     "test-lttc_algo-tracks2.data"            title "Event tracks" with lines lt 6 , \
     "test-lttc_algo-hits.data"               title "Event hits" with lines lw 2 , \
     "test-lttc_algo-hit_clustering.data" index 0  using 1:2 title "Unclustered hits" with lines lw 1.5 lc rgb "black", \
     "test-lttc_algo-hit_clustering.data" index 1:(number_of_clusters):1  using 1:2:4 title "Candidate clusters" with lines lw 1.5 lc rgbcolor variable, \
     "test-lttc_algo-hit_clustering.data" index number_of_clusters+1  using 1:2:4 title "Candidate kinked tracks" with lines lw 1.0 lc rgbcolor variable, \
     "test-lttc_algo-clustering_tracks.data"  index 0:(number_of_tracks):1 using 1:2:4 title "Cluster track paths"  with lines lw 1.0 lc rgbcolor variable
pause -1 "Hit [Enter]..."

plot [-500.0:500.0][-1000:1000] \
     "test-lttc_algo-tracker-geometry.data"   title "Tracker"      with lines , \
     "test-lttc_algo-tracker-dead-cells.data" title "Dead cells"   with lines lt 7 lw 2 , \
     "test-lttc_algo-tracks2.data"            title "Event tracks" with lines lt 6 , \
     "test-lttc_algo-hits.data"               title "Event hits"   with lines lw 2 , \
     "test-lttc_algo-hit_clustering.data"     index 0  using 1:2 \
         title "Unclustered hits" with lines lw 1.5 lc rgb "black", \
     "test-lttc_algo-hit_clustering.data"     index 1:(number_of_clusters):1 using 1:2:(RGB2($4)) \
         title "Candidate clusters"      with lines lw 1.5 lc rgb variable, \
     "test-lttc_algo-hit_clustering.data"     index number_of_clusters+1     using 1:2            \
         title "Candidate kinked tracks" with lines lw 2.0 lc rgb "red", \
     "test-lttc_algo-clustering_tracks.data"  index 0:(number_of_tracks):1   using 1:2:(RGB2($4)) \
         title "Cluster track paths"     with lines lw 1.0 lc rgb variable
pause -1 "Hit [Enter]..."

# step2_last_loop_lines_data index 0:iline_last:1 title "Cluster lines" with lines lt 2 lw 1., \

# plot [-500.0:500.0][-1000:1000] \
#      "test-lttc_algo-tracker-geometry.data"   title "Tracker" with lines, \
#      "test-lttc_algo-tracker-dead-cells.data" title "Dead cells" with lines lt 7 lw 2, \
#      "test-lttc_algo-tracks2.data"            title "Event tracks" with lines lt 6 , \
#      "test-lttc_algo-hits.data"               title "Event hits" with lines lw 1 , \
#      "test-lttc_algo-clustering_tracks.data"  index 0 using 1:2:4 title "Cluster track paths"  with lines lw 1 lc rgb variable, \
# pause -1 "Hit [Enter]..."

ntracks=3
set xlabel "x"
set ylabel "y"
set zlabel "z"
set view equal xyz
zmin = -1500
zproj = zmin+100
set xyplane at zmin
splot [-500.0:500.0][-1500:1500][-1500:1500] \
      "test-lttc_algo-tracker-geometry.data"   using 1:2:(zmin) title "Tracker" with lines lc rgb "gray", \
      "test-lttc_algo-tracker-dead-cells.data" using 1:2:(zmin) title "Dead cells" with lines lw 2 lc rgb "red", \
      "test-lttc_algo-tracks3.data"  index  0:(ntracks):1 using 1:2:3      title "Event tracks" with lines lt 6 lc rgb "blue" , \
      "test-lttc_algo-hits.data"               using 1:2:3:4    title "Event hits" with lines lw 1 lc rgb "green", \
      "test-lttc_algo-clustering_tracks.data"  using 1:2:(zmin-10*column(4)):(RGB2($4)) title "Cluster track paths"  with lines lw 1 lc rgb variable
pause -1 "Hit [Enter]..."


# splot [-500.0:500.0][-1500:1500][-1500:1500] \
#      "test-lttc_algo-tracker-geometry.data"   using 1:2:(zmin) title "Tracker" with lines lc rgb "gray", \
#      "test-lttc_algo-tracker-dead-cells.data" using 1:2:(zmin) title "Dead cells" with lines lw 2 lc rgb "red", \
# pause -1 "Hit [Enter]..."
zmin = -500
zproj = zmin+100
set xyplane at zmin
splot [-500.0:500.0][-1000:1000][-1500:1500] \
     "test-lttc_algo-tracker-geometry.data"  using 1:2:(zproj) title "Tracker"      with lines , \
     "test-lttc_algo-tracker-dead-cells.data" using 1:2:(zproj) title "Dead cells"   with lines lt 7 lw 2 , \
     "test-lttc_algo-tracks2.data"            title "Event tracks" with lines lt 6 , \
     "test-lttc_algo-hits.data"               title "Event hits"   with lines lw 2 , \
     "test-lttc_algo-hit_clustering.data"     index 0  using 1:2:3 title "Unclustered hits" with lines lw 1.5 lc rgb "black", \
     "test-lttc_algo-hit_clustering.data"     index 1:(number_of_clusters):1 using 1:2:3:(RGB2($4)) title "Candidate clusters"      with lines lw 1.5 lc rgb variable, \
     "test-lttc_algo-hit_clustering.data"     index number_of_clusters+1     using 1:2:3            title "Candidate kinked tracks" with lines lw 2.0 lc rgb "red", \
     "test-lttc_algo-clustering_tracks.data"  index 0:(number_of_tracks):1   using 1:2:3:(RGB2($4)) title "Cluster track paths"     with lines lw 1.0 lc rgb variable
pause -1 "Hit [Enter]..."

# end
