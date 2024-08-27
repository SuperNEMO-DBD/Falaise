set size ratio -1
set xlabel "x (mm)"
set ylabel "y (mm)"
set title "SuperNEMO Tracker drift model"
set xtics 500
set label "Italy" at -1300,0 right offset -1,0
set label "France" at +1300,0 right offset +1,0
set label "Tunnel" at 0,+2500 center offset 0,0.5
set label "Mountain" at 0,-2500 center offset 10,-0.5
plot 'data/test_tdm.data' notitle with lines lc variable
pause -1

unset label
set xrange [-500:500]
set yrange [-2500:-2000]
set label "Italy" at -300,-2000 right offset -1,1
set label "France" at +300,-2000 left offset +1,1
set label "Mountain" at 0,-2500 center offset 15,-0.75
# set label "Tunnel" at 0,+2000 center offset -10,0.5
plot 'data/test_tdm.data' notitle with lines lc variable
pause -1

set terminal push
set terminal jpeg
set output "./images/img_tdm_1.jpg"
replot
set output
set terminal pop

unset label
set size noratio 
set grid
set xlabel "Radial distance (mm)"
set ylabel "Drift time (us)"
set xrange [0.0:32.0]
set xtics 2.0
set yrange [0.0:*]
set key out
ut=1e4
set title "Quarter #0 (edge)"
plot 'data/test_tdm_drift_time.data' index 0 using 1:($2*ut) title "Low" with lines, \
     '' index 0 using 1:($3*ut) title "Med" with lines, \
     '' index 0 using 1:($4*ut) title "Up" with lines
pause -1
set terminal push
set terminal jpeg
set output "./images/img_tdm_dt1.jpg"
replot
set output
set terminal pop

set title "Quarter #1 (center)"
plot 'data/test_tdm_drift_time.data' index 1 using 1:($2*ut) title "Low" with lines, \
     '' index 1 using 1:($3*ut) title "Med" with lines, \
     '' index 1 using 1:($4*ut) title "Up" with lines
pause -1
set terminal push
set terminal jpeg
set output "./images/img_tdm_dt2.jpg"
replot
set output
set terminal pop

set title "Quarter #2 (edge)"
plot 'data/test_tdm_drift_time.data' index 2 using 1:($2*ut) title "Low" with lines, \
     '' index 2 using 1:($3*ut) title "Med" with lines, \
     '' index 2 using 1:($4*ut) title "Up" with lines
pause -1
set terminal push
set terminal jpeg
set output "./images/img_tdm_dt3.jpg"
replot
set output
set terminal pop

set title "Quarter #3 (corner)"
plot 'data/test_tdm_drift_time.data' index 3 using 1:($2*ut) title "Low" with lines, \
     '' index 3 using 1:($3*ut) title "Med" with lines, \
     '' index 3 using 1:($4*ut) title "Up" with lines
pause -1
set terminal push
set terminal jpeg
set output "./images/img_tdm_dt4.jpg"
replot
set output
set terminal pop


# plot 'data/test_tdm_drift_time.data' index 0 using 1:2 title "Low - Quarter #0 (edge)" with lines, \
#      '' index 0 using 1:3 title "Med - Quarter #0 (edge)" with lines, \
#      '' index 0 using 1:4 title "Up  - Quarter #0 (edge)" with lines, \
#      '' index 1 using 1:2 title "Low - Quarter #1 (center)" with lines, \
#      '' index 1 using 1:3 title "Med - Quarter #1 (center)" with lines, \
#      '' index 1 using 1:4 title "Up  - Quarter #1 (center)" with lines, \
#      '' index 2 using 1:2 title "Low - Quarter #2 (center)" with lines, \
#      '' index 2 using 1:3 title "Med - Quarter #2 (center)" with lines, \
#      '' index 2 using 1:4 title "Up  - Quarter #2 (center)" with lines, \
#      '' index 3 using 1:2 title "Low - Quarter #3 (center)" with lines, \
#      '' index 3 using 1:3 title "Med - Quarter #3 (center)" with lines, \
#      '' index 3 using 1:4 title "Up  - Quarter #3 (center)" with lines
# pause -1









