Parametrized pulse for calorimeter signals

This little program produce 3 signal shape for electron of 700 / 1000 and 1300 keV

The shape are not working for the moment -> certainly constants issues

Command to execute and plot the files :

g++ calo_signal_calculation_bordeaux.cxx -o calo_signal_calculation_bordeaux
./calo_signal_calculation_bordeaux

gnuplot 
> plot "calo_signal_electron_1300keV.out"
