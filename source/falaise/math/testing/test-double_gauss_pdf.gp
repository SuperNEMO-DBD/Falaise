! gsl-histogram 0 5 1000 < test-double_gauss_random.data > test-double_gauss_random.his
set grid

plot 'test-double_gauss_pdf.data' u 1:2 w l
pause -1

N=100000
dx=5e-3
plot [1:3] 'test-double_gauss_random.his' u 1:3 w steps lw 1 , \
     'test-double_gauss_pdf.data' u 1:(($2)*N*dx) w lines

pause -1

# end


