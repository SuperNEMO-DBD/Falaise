set grid
set key out
set size ratio -1
set clip two
set xlabel "x"
set ylabel "y"
set xzeroaxis lt -1
set yzeroaxis lt -1

set object 1 circle at 0,0 size 10.0 front fillstyle solid 1.0 fillcolor rgb "red"

! ./lttcc-test-triplet_tools > test-triplet_tools.data

plot [0.0:500.0][-500:500] \
     "test-triplet_tools.data" index 0 title "Tracker" with lines, \
     "test-triplet_tools.data" index 1 title "Track" with points pt 6 ps 0.35, \
     "test-triplet_tools.data" index 2 title "Hits" with lines lw 2 
# 			  "test-triplet_tools.data" index 4::1 title "Triplets" with points pt 4 ps 0.75 lc variable, \
# 			  "test-triplet_tools.data" index 5::1 title "Circles" with lines lw 1 lc variable
pause -1 "Hit [Enter]..."


set terminal push
set terminal pdfcairo
set output "test-triplet_tools.pdf"
replot
set output
set terminal pop


unset object

# ! rm -f test-triplet_tools.data  
# end
