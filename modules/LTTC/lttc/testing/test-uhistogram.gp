set grid
set key out
set xlabel "x"
set ylabel "Counts"
set xzeroaxis lt -1
set yzeroaxis lt -1

plot [:][0.0:] \
     "test-uhistogram.data" index 0 using (0.5*(($1)+($2))):($3) notitle with histeps lw 2
pause -1 "Hit [Enter]..."


set terminal push
set terminal pdfcairo
set output "test-uhistogram.pdf"
replot
set output
set terminal pop

unset object

# end
