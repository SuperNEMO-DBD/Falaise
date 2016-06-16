echo "Compilation..."
g++ calo_signal_calculation_bordeaux.cxx -o calo_signal_calculation_bordeaux
sleep 2s
echo "Executing..."
./calo_signal_calculation_bordeaux
sleep 1s
echo "Launch Gnuplot"
gnuplot
