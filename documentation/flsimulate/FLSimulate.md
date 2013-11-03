Using The FLSimulate Application
================================

\tableofcontents

Introduction {#intro}
============
FLSimulate's task is to simulate the generation and passage of particles
through the SuperNEMO detectors (BiPo3 and SuperNEMO), recording the
detector response and writing this to an output file. Its simulation
engine for geometry and physics uses the [Geant4](http://geant4.cern.ch) 
toolkit, with I/O handled by the Bayeux framework.

Here we present a brief overview of running FLSimulate from the command
line to generate an output file suitable for input to the
[FLReconstruct](md_FLReconstruct.html) application.

At present (Alpha 1), FLSimulate only supports simulation of the SuperNEMO
tracker module in its commissioning setup, with cosmic muon events. Further
functionality will
