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

Usage
======

Online help is available through:

.. raw: sh

   flsimulate --help
..

Variant support
===============

FLsimulate automatically  configures a  *variance service*  adapted to
the experimental setup selected  with the ``--experiment`` option. For
now  only the  ``Demonstrator`` experiment  has its  specific variance
setup.

1. The  ``--variant-load``  option  enables  to load  an  input  *variant
   profile* file  and assign  specific values  to the  variant parameters
   addressed by this profile:

.. raw: sh

   --variant-load "Path/To/A/Variant/Profile/Input/File"
..

   where:  ``Path/To/A/Variant/Profile/Input/File``   locates  a  file
   which contains a set of preset variant parameters.

2. The ``--variant-set``  options enable to assign  specific values to
   variant parameters as  defined in one of the  variant registries of
   the application's  variant repository. Several such  options can be
   used, each one being related to a single parameter. The syntax is:

.. raw: sh

   --variant-set "Registry:Path/To/The/Variant/Parameter=Value"
..

   where :

    * ``Registry`` is the  name of an active  variant registry. Supported
      registries are currently :

       * ``geometry``  :  special  variant   parameters  related  to  the
         geometry layout.
       * ``commissioning_source``   :   special    setup   for   external
         calibration source in the commissioning layout.
       * ``simulation`` : special  variant   parameters  related  to  the
         Geant4 based simulation run.

    * The  ``Path/To/The/Variant/Parameter`` path  identifies a  specific
      variant parameter defined in the target ``Registry``.
    * The ``Value`` token is a textual  representation of the value to be
      assigned to the target parameter.

3. The ``--variant-gui``  option lauches  a variance Qt-based  GUI editor
   (this  feature  is  only  available  if  Bayeux  was  built  with  the
   ``BAYEUX_WITH_QT_GUI`` option).

4. The ``--variant-store`` option enables  to store the current selection
   of values  assigned to  the variant parameters  in an  output *variant
   profile* file.

.. raw: sh

   --variant-store "Path/To/A/Variant/Profile/Output/File"
..

   where:   ``Path/To/A/Variant/Profile/Output/File``  identifies   an
   output file where to store the current set of variant parameters.


Example:

.. raw: sh

   flsimulate \
     --number 10 \
     --experiment "Demonstrator" \
     --event-generator "Tl208" \
     --vertex-generator "field_wire_bulk" \
     --output-file "Tl208_10-field_wire_bulk_SD.brio" \
     --variant-load "my_profile.conf" \
     --variant-set "geometry:layout/if_basic/magnetic_field=false" \
     --variant-gui \
     --variant-store "new_profile.conf"

..
