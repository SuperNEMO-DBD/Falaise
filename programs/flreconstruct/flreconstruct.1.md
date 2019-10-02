% FLRECONSTRUCT(1) Falaise Tools Documentation
% Ben Morgan
% September 2013

# NAME

flreconstruct - process data output by SuperNEMO detectors and detector simulations

# SYNOPSIS

flreconstruct [options]

# OVERVIEW

This program runs a pipeline based processing of event data from files
output by SuperNEMO detectors and detector simulations. The processing
pipeline consists of individual modules each performing a specific
processing task. The user may select the module type, configuration and
ordering in the pipeline via an input script.

# DESCRIPTION

Run event data from SuperNEMO through a processing pipeline

**-h, --help**
:    Print short help information to stdout.

**-i, --input-file**=FILE
:    Read data from FILE. Mandatory.

**-o, --output-file**=FILE
:    Write processed data to FILE. If not supplied, /dev/null or equivalent is used.

**-p, --pipeline**=SCRIPT
:    Configure pipeline using descripting in SCRIPT. If not supplied, data will be dumped to stdout.

**-v, --verbose**=LEVEL
:    Set logging verbosity to LEVEL, which may be selected from trace, debug, information, notice, warning, error, critical, fatal. The default level is fatal.

**--version**
:    Print version details including those of used libraries.

# SEE ALSO

`flsimulate`(1), `libFalaise`(3),

# COPYRIGHT

Copyright (C) 2013 SuperNEMO Collaboration

