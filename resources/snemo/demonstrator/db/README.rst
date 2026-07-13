==================================================
SuperNEMO Database configuration files
==================================================

- ``0.1/services/`` : Description of the SuperNEMO DB service

  - ``DbService.conf`` : Main configuration file of the DB service

    This configuration file describes:

    - The ``~/.sndb.conf`` file which contains the credentials to connect the
      SuperNEMO DB at CCIN2P3 in readonly mode.

    - The description of all DB tables available from the server at CCIN2P3: ``sndb/1.0/main.conf`` (see below)
      

- ``sndb/1.0/`` : Description of the databases and embedded tables

    This configuration directory contains:

    - The ``main.conf`` file which describes the various DB hosted
      by the DB server for SuperNEMO and their DB tables.

      

  

.. end
