
second = 1.0
minute = 60.0 * second
hour   = 60.0 * minute
day    = 24.0 * hour
Bq     = 1.0 / second
Thalf = 3.8215 * day
tau = Thalf / log(2.0)
print "tau=", tau/day , " day"
A0 = 1.0 * Bq
A(x) = A0 * exp(-x / tau)

do for [d=0:10:1] {
    print "A(",d,") =", A(d*day)
}
set xtics 1
set title "Radon activity"
set grid
plot [0: 5 * tau / day][0.0:A0*1.1] A(x*day) with lines lw 2
pause -1

# end
