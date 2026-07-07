set grid

set xlabel "x (mm)"
set ylabel "y (mm)"
set zlabel "Correction factor" rotate by 90
set size ratio -1
set view equal xy

set palette defined (\
    0   98./255   0 255./255, \
    1   51./255   0 255./255, \
    2    0  20./255 255./255, \
    3    0  68./255 255./255, \
    4    0 139./255 255./255, \
    5    0 187./255 255./255, \
    6    0 255./255 255./255, \
    7    0 255./255 204./255, \
    8    0 255./255 133./255, \
    9    0 255./255  85./255, \
    10   0 255./255  13./255, \
    11  34./255 255./255   0, \
    12 105./255 255./255   0, \
    13 157./255 255./255   0, \
    14 255./255 255./255   0, \
    15 255./255 238./255   0, \
    16 255./255 167./255   0, \
    17 255./255 119./255   0, \
    18 255./255  47./255   0, \
    19 255./255   0   0 \
    )

########################
set title " Calo XY-uniformity correction factor (MWALL8) "
set zrange [0.9325:1.0175]
set xyplane at 0.93

splot 'calo_uniformity_xy_coeff-MWALL8.data' u 2:(-($1)):3 w lines title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xy_coeff-MWALL8.png"
replot
set output
set terminal pop

set cbrange [0.9325:1.0175]
plot 'calo_uniformity_xy_coeff-MWALL8.data' u 2:(-($1)):3 w image title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xy_coeff-MWALL8-map.png"
replot
set output
set terminal pop

########################
set title " Calo XY-uniformity correction factor (MWALL5) "
set zrange [0.919:1.012]
set xyplane at 0.919
set xlabel "x (mm)"
set ylabel "y (mm)"
splot 'calo_uniformity_xy_coeff-MWALL5.data' u 2:(-($1)):3 w lines title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xy_coeff-MWALL5.png"
replot
set output
set terminal pop

set cbrange [0.919:1.012]
plot 'calo_uniformity_xy_coeff-MWALL5.data' u 2:(-($1)):3 w image title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xy_coeff-MWALL5-map.png"
replot
set output
set terminal pop

########################
set title " Calo XY-uniformity correction factor (XWALL) "
set xyplane at 0.9265
set zrange [0.9265:1.0175]

splot 'calo_uniformity_xy_coeff-XWALL.data' u 2:(-($1)):3 w lines title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xy_coeff-XWALL.png"
replot
set output
set terminal pop

set cbrange [0.9265:1.0175]
plot 'calo_uniformity_xy_coeff-XWALL.data' u 2:(-($1)):3 w image title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xy_coeff-XWALL-map.png"
replot
set output
set terminal pop

# exit

########################
set title " Calo XY-uniformity correction factor (GVETO) "
set zrange [0.878:1.005]
set xyplane at 0.878
splot 'calo_uniformity_xy_coeff-GVETO.data' u 2:(-($1)):3 w lines title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xy_coeff-GVETO.png"
replot
set output
set terminal pop

set cbrange [0.878:1.005]
plot 'calo_uniformity_xy_coeff-GVETO.data' u 2:(-($1)):3 w image title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xy_coeff-GVETO-map.png"
replot
set output
set terminal pop

exit 

########################################################################
########################################################################
set xlabel "x (mm)"
set ylabel "z (mm)"
set zlabel "Correction factor" rotate by 90
set size ratio -1
set xyplane at 0.90

########################
set title " Calo XZ-uniformity correction factor (MWALL8) "
splot 'calo_uniformity_xz_coeff-MWALL8.data' w lines title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xz_coeff-MWALL8.png"
replot
set output
set terminal pop

plot 'calo_uniformity_xz_coeff-MWALL8.data' w image title ""
pause -1

set terminal push
set terminal pngcairo enhanced
set output "calo_uniformity_xz_coeff-MWALL8-map.png"
replot
set output
set terminal pop

# end