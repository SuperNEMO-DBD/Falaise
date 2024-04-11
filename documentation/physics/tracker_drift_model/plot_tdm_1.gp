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








