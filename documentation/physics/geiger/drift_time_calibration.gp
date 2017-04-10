nanosecond=1.0
microsecond=1000*nanosecond
mm=1.0
cm=10*mm
m=100*cm

rcell=22.0*mm

tcut=10*microsecond
t0=4.0*microsecond

A1 = 0.570947153108633
B1 = 0.580148313540993
C1 = 1.6567483468611
A2 = 1.86938462695651
B2 = 0.949912427483918

# x == time in usec
# return: radius in cm
drift_radius_usec_cm(x) = (x < (t0 / microsecond)) ? (A1 * (10*x) / ((10*x)**B1 + C1)) : (A2 * (10*x) / ((10*x)**B2))
drift_radius_usec_cm_mode1(x) = (A1 * (10*x) / ((10*x)**B1 + C1))

drift_radius(x) = drift_radius_usec_cm(x/microsecond) * cm


sigma_r_a = 0.425 * mm
sigma_r_b = 0.0083
sigma_r_r0= 12.25 * mm
# x  == radius
sigma_drift_radius(x) = sigma_r_a * (1.0 + sigma_r_b * ((x - sigma_r_r0) / mm)** 2);

set xlabel "t_{drift} [usec]"
set ylabel "r_{drift}[cm]"
set xrange [0.0: tcut/microsecond]
set yrange [0.0: 1.5*rcell/cm]
set grid
set samples 1000
plot [0.0: tcut/microsecond] drift_radius(x*microsecond)/cm notitle
pause -1

set terminal pdfcairo
set output "gg_drift.pdf"
replot
set output
set terminal qt


set xlabel "r_{drift} [cm]"
set ylabel "\sigma_r [cm]"
set xrange [0.0: tcut/microsecond]
set yrange [0.0: 1.5*rcell/cm]
plot [0.0: 2.5*rcell/cm] sigma_drift_radius(x*cm)/cm notitle
pause -1