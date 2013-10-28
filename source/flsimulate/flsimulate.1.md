% FLSIMULATE(1) Falaise Tools Documentation
% Ben Morgan
% September 2013

# NAME

flsimulate - run a simulation of the SuperNEMO experiment detector systems

# SYNOPSIS

flsimulate [options]

# OVERVIEW

This program runs a Geant4 based simulation of a SuperNEMO detector
(Tracker Commissioning setup only in this version) in batch mode.

# DESCRIPTION

Run a Geant4 simulation of a SuperNEMO detector

**-e, --event-generator**=NAME
:    Use NAME as the generator for primary particle(s).

**-h, --help**
:    Print short help information to stdout.

**-n, --number**=N
:    Simulate N events. Default is 1.

**-o, --output-file**=FILE
:    Write events and simulation metadata to file FILE. Mandatory.

**-v, --verbose**
:    Increase verbosity of logging.

**--version**
:    Print version details including those of used libraries.

**-x, --vertex-generator**=NAME
:    Use NAME as the generator for event starting locations.

# SEE ALSO

`flreconstruct`(1), `libFalaise`(3),

# COPYRIGHT

Copyright (C) 2013 SuperNEMO Collaboration

