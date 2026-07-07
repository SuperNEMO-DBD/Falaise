

set grid
set xlabel "E (MeV)"
set ylabel "Q (nV.s)"
set xrange [0.0:1.25]
set yrange [0.0:8.0]
set key out

a = 0.20000
b = -0.02000
covaa = 0.00021
covbb = 0.00250
covba = -0.00068

# a = 0.20116
# b = -0.02968
# covaa = 0.00021
# covbb = 0.00231
# covba = -0.00066

a_err=sqrt(covaa)
b_err=sqrt(covbb)
print "a = " , a , " +/- ", a_err
print "b = " , b , " +/- ", b_err

# x = Q
fE(x) = a * x + b
# E = Q
fQ(x) = (x - b) / a

plot "ex03.res" index 0 using 2:1:3 with xerrorbars title "est"
pause -1

plot "ex02.data" index 0 with yerrorbars notitle, \
     fQ(x) notitle, \
     "ex03.res" index 0 using 2:1:3 with xerrorbars title "est", \
     "ex02.res" index 0 using 1:2:3 with yerrorbars title ""
pause -1
