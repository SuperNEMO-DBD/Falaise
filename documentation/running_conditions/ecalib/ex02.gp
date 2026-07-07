

set grid
set xlabel "E (MeV)"
set ylabel "Q (nV.s)"
set xrange [0.0:1.25]
set yrange [0.0:8.0]

a = 5.0
b = 0.1
covbb = 0.0625
covba = -0.085
covaa = 0.13

a_err=sqrt(covaa)
b_err=sqrt(covbb)
print "a = " , a , " +/- ", a_err
print "b = " , b , " +/- ", b_err

f(x) = a * x + b

plot "ex02.data" index 0 with yerrorbars notitle, \
     f(x) notitle, \
     "ex02.res" index 0 with yerrorbars title "est"
pause -1

a = 5.0
b = 0.1
covbb = 0.0570351
covba = -0.0816736
covaa = 0.127975

a_err=sqrt(covaa)
b_err=sqrt(covbb)
print "a = " , a , " +/- ", a_err
print "b = " , b , " +/- ", b_err

f(x) = a * x + b

plot "ex02.data" index 1 with yerrorbars notitle, \
     f(x) notitle, \
     "ex02.res" index 1 with yerrorbars title "est"
pause -1

