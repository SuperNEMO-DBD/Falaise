Using The FLPTD2Root Application {#usingflptd2root}
================================

\tableofcontents

Introduction to FLPTD2Root {#usingflptd2root_intro}
==========================
At present, the builtin ROOT formatter of `flreconstruct` only outputs
the basic raw and calibrated data banks. To help in user analysis,
a simple converter program, `flptd2root.py` is provided to convert all
data banks to a flat ROOT TTree for further analysis.

Installing FLPTD2Root {#usingflptd2root_install}
=====================
If you have installed Falaise via Cadfaelbrew, simply do

~~~~~
$ brew update
$ brew install ptd2root
~~~~~

Using FLPTD2Root.py  {#usingflptd2root_using}
===================
FLPTD2Root is implemented as a simple command line python program.
To get help on the available options, run `flptd2root.py` with the
`-h` option:

~~~~~
$ flptd2root.py -h
usage: flptd2root.py [-h] -i <infile> -o <outfile>

Convert flreconstruct BRIO format to ROOT

optional arguments:
  -h, --help    show this help message and exit
  -i <infile>   input BRIO file
  -o <outfile>  output ROOT file
~~~~~

To convert a full BRIO format file output by `flreconstruct`, simply
pass the path to this file as the `-i` argument, and use the `-o`
option to supply the name of the output ROOT format file. For example,

~~~~~
$ flptd2root.py -i example-reco.brio -o example-reco.root
~~~~~

Format of the FLPTD2Root ROOT TTree {#usingflptd2root_format}
===================================
The format of the TFile/TTree is fully documented in a [README PDF file](https://github.com/SuperNEMO-DBD/PTD2Root/blob/develop/readme.pdf).



