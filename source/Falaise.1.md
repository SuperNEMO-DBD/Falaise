% FALAISE(1) Falaise Tools Documentation
% Ben Morgan
% September 2013

# NAME

Falaise - SuperNEMO Software System

# SYNOPSIS

flreconstruct [options]

# OVERVIEW

Falaise is a software suite to read, simulate, process and analyse raw data
from the SuperNEMO Double-Beta Decay search experiment.

Falaise is free software delivered as a ready-to-run binary distribution or
as source code that you can freely use, copy, modify and distribute. It is
licensed under the GPL version 3. It runs on POSIX operating systems.

The functionality of Falaise is broken down into several tools

**flreconstruct**
:    The main reconstruction program

**flsimulate**
:    The main simulation program

**Falaise**
:    A C++ library implementing needed core functionality. It can be used
     to build applications like `flreconstruct` and `flsimulate`. However,
     its primary use is for the development of plugins for `flreconstruct`.
     It builds heavily on the foundation `Bayeux` C++ library, which by
     default is distributed with `Falaise`.

For more information about Falaise, point your browser to docs...

# SEE ALSO

`flreconstruct`(1), `flsimulate`(1), `libFalaise`(3), `Bayeux`(1).

# COPYRIGHT

Copyright (C) 2013 SuperNEMO Collaboration

