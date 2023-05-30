=====================================================
Build Falaise in development mode
=====================================================


This directory contains some useful scripts to build and install Falaise locally
in development mode.

#. From your Bash shell:

   #. Setup Bayeux >= 3.5.3
   #. Setup SNRS >= 1.1.0

#. From the Falaise source directory:
   
   #. Run the buil script:

      .. code:: shell

	 $ bash ./devel/build.bash
      ..

#. Check the Falaise installation in the ``_install.d/`` directory:

   .. code:: shell

      $ ls -l _install.d/
   ..

#. Check the Falaise setup script in the ``_setup.d/`` directory:

   .. code:: shell

      $ ls -l _setup.d/falaise_develop_run_env.bash
   ..
   

#. Source the Falaise setup script, setup Falaise from the current shell and check:

   .. code:: shell

      $ source _setup.d/falaise_develop_run_env.bash
      $ falaise_develop_setup
      [info] Falaise version 'develop' is now setup
      $ falaise-config --version
      5.0.0
      $ falaise-config --prefix
      /path/to/the/Falaise/source/dir/_install.d/develop
      $ which flsimulate-configure 
      /path/to/the/Falaise/source/dir/_install.d/develop/bin/flsimulate-configure
      $ which flsimulate
      /path/to/the/Falaise/source/dir/_install.d/develop/bin/flsimulate
      $ which flconfigure
      /path/to/the/Falaise/source/dir/_install.d/develop/bin/flconfigure
   ..


.. end
