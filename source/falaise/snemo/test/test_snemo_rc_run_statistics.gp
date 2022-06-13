tMin=-2e10
tMax=2e11
nBins=4000
dtBin=tMax/nBins
nbDecays=1000000

nbDecaysRef42=`cat run_decay_random-42.data | grep ^#@nb-decays-ref= | cut -d= -f2`
nbDecays42=`cat run_decay_random-42.data | grep ^#@nb-decays= | cut -d= -f2`
timeMin42=`cat run_decay_random-42.data | grep ^#@time-min= | cut -d= -f2`
timeMax42=`cat run_decay_random-42.data | grep ^#@time-max= | cut -d= -f2`

nbDecaysRef43=`cat run_decay_random-43.data | grep ^#@nb-decays-ref= | cut -d= -f2`
nbDecays43=`cat run_decay_random-43.data | grep ^#@nb-decays= | cut -d= -f2`
timeMin43=`cat run_decay_random-43.data | grep ^#@time-min= | cut -d= -f2`
timeMax43=`cat run_decay_random-43.data | grep ^#@time-max= | cut -d= -f2`

nbDecaysRef44=`cat run_decay_random-44.data | grep ^#@nb-decays-ref= | cut -d= -f2`
nbDecays44=`cat run_decay_random-44.data | grep ^#@nb-decays= | cut -d= -f2`
timeMin44=`cat run_decay_random-44.data | grep ^#@time-min= | cut -d= -f2`
timeMax44=`cat run_decay_random-44.data | grep ^#@time-max= | cut -d= -f2`

print "Run 42:"
print " - nbDecaysRef = " , nbDecaysRef42
print " - nbDecays    = " , nbDecays42
print " - timeMin     = " , timeMin42
print " - timeMax     = " , timeMax42

print "Run 43:"
print " - nbDecaysRef = " , nbDecaysRef43
print " - nbDecays    = " , nbDecays43
print " - timeMin     = " , timeMin43
print " - timeMax     = " , timeMax43

print "Run 44:"
print " - nbDecaysRef = " , nbDecaysRef44
print " - nbDecays    = " , nbDecays44
print " - timeMin     = " , timeMin44
print " - timeMax     = " , timeMax44

! grep -v "^#@" run_decay_random-42.data | gsl-histogram 0.0 2e11 4000 > run_decay_random-42.his
print "Histo 42 done."
! grep -v "^#@" run_decay_random-43.data | gsl-histogram 0.0 2e11 4000 > run_decay_random-43.his
print "Histo 43 done."
! grep -v "^#@" run_decay_random-44.data | gsl-histogram 0.0 2e11 4000 > run_decay_random-44.his
print "Histo 44 done."

set grid

set xrange [tMin:tMax]
plot \
     'activity.data' i 0 u 1:(($2)) title "Activity (run span)" w l lw 4 lc rgb "magenta", \
     'activity.data' i 1 u 1:(($2)) title "Activity model" w l lw 1 lc rgb "yellow" , \
     'run_decay_dist-42.data' title "Run 42 (decay CDF)" w l lw 2 lc rgb "red", \
     'run_decay_dist-43.data' title "Run 43 (decay CDF)" w l lw 2 lc rgb "green", \
     'run_decay_dist-44.data' title "Run 44 (decay CDF)" w l lw 2 lc rgb "blue"
pause -1

plot [0.0:tMax] \
     'run_decay_random-42.his' using (0.5*(($1)+($2))):(($3)) title "Run 42" with histeps lw 2 , \
     'run_decay_random-43.his' using (0.5*(($1)+($2))):(($3)) title "Run 43" with histeps lw 2 , \
     'run_decay_random-44.his' using (0.5*(($1)+($2))):(($3)) title "Run 44" with histeps lw 2 
pause -1

# plot [0.0:0.7*tMax] \
#      'activity.data' i 0 u 1:(($2)) notitle w l lw 4 lc rgb "magenta", \
#      'activity.data' i 1 u 1:(($2)) notitle w l lw 1 lc rgb "yellow" , \
#      'run_decay_dist-42.data' using 1:(($2)*nbDecays/dtBin) notitle w l lw 2 lc rgb "red", \
#      'run_decay_dist-43.data' using 1:(($2)*nbDecays/dtBin) notitle w l lw 2 lc rgb "green", \
#      'run_decay_dist-44.data' using 1:(($2)*nbDecays/dtBin) notitle w l lw 2 lc rgb "blue", \
#      'run_decay_random-42.his' using (0.5*(($1)+($2))+timeMin42):(($3)) notitle with histeps lw 2 lc rgb "red", \
#      'run_decay_random-43.his' using (0.5*(($1)+($2))+timeMin43):(($3)) notitle with histeps lw 2 lc rgb "green", \
#      'run_decay_random-44.his' using (0.5*(($1)+($2))+timeMin44):(($3)) notitle with histeps lw 2 lc rgb "blue"
# pause -1

plot [0.0:0.7*tMax] \
     'run_decay_random-42.his' using (0.5*(($1)+($2))+timeMin42):(($3)) notitle with histeps lw 2 lc rgb "red", \
     'run_decay_random-43.his' using (0.5*(($1)+($2))+timeMin43):(($3)) notitle with histeps lw 2 lc rgb "green", \
     'run_decay_random-44.his' using (0.5*(($1)+($2))+timeMin44):(($3)) notitle with histeps lw 2 lc rgb "blue"
pause -1

# set multiplot layout 2,1

# plot [0.0:0.7*tMax] \
#      'activity.data' i 0 u 1:(($2)) title "Activity model" w l lw 2 lc rgb "magenta"
# pause -1

# plot [0.0:0.7*tMax] \
#      'run_decay_random-42.his' using (0.5*(($1)+($2))+timeMin42):(($3)) notitle with histeps lw 1 lc rgb "red", \
#      'run_decay_random-43.his' using (0.5*(($1)+($2))+timeMin43):(($3)) notitle with histeps lw 1 lc rgb "green", \
#      'run_decay_random-44.his' using (0.5*(($1)+($2))+timeMin44):(($3)) notitle with histeps lw 1 lc rgb "blue"
# pause -1

# unset multiplot


set title "Randomized decay times with respect to activity model\nand DAQ run schedule (toy models)"
set xlabel "Time (us)"
set ylabel "Number of decays (/bin)"
set key font ",10" 
plot [0.0:0.7*tMax][0:1000] \
     'run_decay_random-42.his' using (0.5*(($1)+($2))+timeMin42):(($3)) notitle with histeps lw 1 lc rgb "red", \
     'run_decay_random-43.his' using (0.5*(($1)+($2))+timeMin43):(($3)) notitle with histeps lw 1 lc rgb "green", \
     'run_decay_random-44.his' using (0.5*(($1)+($2))+timeMin44):(($3)) notitle with histeps lw 1 lc rgb "blue", \
     'activity.data' i 0 u 1:(($2)*490) title "Activity model" w l lw 2 lc rgb "orange" , \
     'run_decay_dist-42.data' using 1:(($2)*500) title "Run 42" w l lw 2 lc rgb "red", \
     'run_decay_dist-43.data' using 1:(($2)*500) title "Run 43" w l lw 2 lc rgb "green", \
     'run_decay_dist-44.data' using 1:(($2)*500) title "Run 44" w l lw 2 lc rgb "blue", \
     'decay_times.data' u 1:(100.0):2 title "Decay times" with points pt 6 ps 0.35 lc variable
pause -1

set terminal push
set terminal pdfcairo
set output "test_snemo_rc_run_statistics-1.pdf"
replot
set output
set terminal pop

set terminal push
set terminal pngcairo
set output "test_snemo_rc_run_statistics-1.png"
replot
set output
set terminal pop

# set ylabel "Decay times"
# plot [0.0:0.7*tMax] 'decay_times.data' u 1:(0.0):2 with dots lc variable
# pause -1

# end

