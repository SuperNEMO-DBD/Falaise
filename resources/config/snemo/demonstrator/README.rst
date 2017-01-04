The SuperNEMO demonstrator module configuration files
=====================================================


Virtual geometry
----------------

The modelization of the virtual geometry is stored
from the ``geometry/`` directory, with a version number (i.e. `4.0`).

Simulation
----------

The  parameters  used  to  configure the  various  components  of  the
simulation   are    stored   in   files   from    the   ``simulation/``
directory. There are several components:

  * The  configuration  files for  the  generation  of primary  events
    (initial   particles   and   kinematics)  is   stored   from   the
    ``primary_events/`` subdirectory.  A  version number (i.e. ``1.0``)
    identifies a  given configuration of the  primary event generation
    engine based on the ``Bayeux/genbb_help`` library module.
  * The  configuration  files  for  generation  of  vertexes  (initial
    positions  of   the  generated  particles)  is   stored  from  the
    ``vertexes/``  subdirectory.   A  version  number  (i.e.   ``4.1``)
    identifies a  given configuration of the  vertex generation engine
    based on the ``Bayeux/genvtx`` library module.
  * The  configuration files  for  Geant4 based  simulation engine  is
    stored from the ``geant4_control/``  subdirectory. A version number
    (i.e.   ``2.1``) identifies  a given  configuration of  the Geant4
    simulation engine  based on the ``Bayeux/mctools``  library module
    and its Geant4 plugin.

Reconstruction
--------------

The   parameters  to   configure   the  various   components  of   the
reconstruction  are  stored  in   files  from  the  ``reconstruction/``
directory.

Virtual experiment
--------------------

The modelization of the virtual experiment is stored
from the ``experiment/`` directory, with a version number (i.e. `1.0`).
This part contains definition and configuration file for the Vire library
and tools (NOT READY YET).
