! gsl-histogram 0 1000 100 < test-tracker_drift_time_generator-random.data > test-tracker_drift_time_generator-random.his
set grid

N=100000
dx=10.0
plot[0:1000] 'test-tracker_drift_time_generator-random.his' u 1:3 w steps lw 1 
pause -1


# end
