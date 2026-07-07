set grid
set xrange [0:1024]
set yrange [0:4096]

plot 'test_processing_calo_signal_baseline-1.data' using 1:2 with points pt 6 ps 0.5
pause -1

plot 'test_processing_calo_signal_baseline-1.data' using 1:2 with steps
pause -1



# end

