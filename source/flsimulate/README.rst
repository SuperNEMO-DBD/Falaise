==============================
Falaise Simulation Application
==============================

What's Here
===========

The main application for the simulation of the SuperNEMO detector.
Any other code that is specific to the simulation - e.g. digitization.

Status
======

This implementation  of FLsimulate uses the  mctools/Geant4 simulation
engine      through      a       dedicated      simulation      module
(mctools::g4::simulation_module) and  writes the simulated  event data
to a  file using  an output  module (dpp::output_module).   The output
file can then be processed by the FLreconstruct application.
