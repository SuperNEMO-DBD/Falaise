# SuperNEMO Lazarus Module

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/0/0b/S._Apollinare_Nuovo_Resurr_Lazzaro.jpg/436px-S._Apollinare_Nuovo_Resurr_Lazzaro.jpg" alt="drawing" width="200"/>

*Paolo Franchini 2021* - p.franchini@imperial.ac.uk

---

SuperNEMO module to resuscitate hits in order to improve the reconstruction in the Tracker.

The module resuscitate hits from the simulated data correspondent to a list of dead cells in the tracker.
The list could have been randomly generated from the Dead Cells Module or created as text file.

Input: simulated data with CD bank. \
Output: simulated data with CD bank, to be used in a full reconstruction.

## Build (TO BE UPDATED):
```
git clone https://github.com/pfranchini/SuperNEMO-LazarusModule
mkdir SuperNEMO-LazarusModule.build
cd SuperNEMO-LazarusModule.build
cmake -DCMAKE_PREFIX_PATH=<path_Falaise_build> -DGSL_ROOT_DIR=<path_GSL_build> ../SuperNEMO-LazarusModule
make
make test
```

## Configure the module:
This is the way to configure the module, for the name of the output file, the list of dead cells and the method used for the resuscitation, using `LazarusModule.conf` produced in the build directory:
```
# - Module configuration:                                                                                                                                                  

[name="Lazarus" type="LazarusModule"]
filename_out : string[1] = "test-lazarus.brio"
dead_cells : string[1] = "random_dead_cells_300.txt"       # file with a list of dead cells (side, layer, column)
method : string[1] = "ALL" 				   # method for the resuscitation (NONE, ALL, NEAR, BETWEEN)
```

## Run the module:
```
flreconstruct -i <brio_file_with_calibration_bank.brio> -p LazarusModule.conf
```

## Run the remaining reconstruction:
```
flreconstruct -i <test-lazarus.brio> -p official-2.0.0_from_CAT.conf -o <test-reco.brio>
```
