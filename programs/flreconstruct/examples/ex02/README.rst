==========================
FLreconstruct example 02
==========================

Example of a simulation and reconstruction pipeline for 1 MeV electrons generated from the surface of the source foil, without magnetic field.


Step 1
======

Run the simulation:

.. code:: shell

   $ flsimulate \
      --verbosity "fatal" \
      --mount-directory "flrecoex02@$(pwd)" \
      --config "flsimulate-ex02.conf" \
      --number-events 100 \
      --output-file "flsimuex02.brio"
..

Step 2
======

Visualize simulated data:

.. code:: shell

   $ flvisualize \
      --variant-profile "flsimulate-ex02.profile" \
      --input-file flsimuex02.brio
..


Step 3
======

Run the reconstruction:

.. code:: shell

   $ flreconstruct \
      --verbosity "debug" \
      --mount-directory "flrecoex02@$(pwd)" \
      --config "flreconstruct-ex02.conf" \
      --input-file "flsimuex02.brio" \
      --output-file "flrecoex02.brio"
..

Step 4
======

Visualize simulated data:

.. code:: shell

   $ flvisualize \
      --variant-profile "flsimulate-ex02.profile" \
      --input-file flrecoex02.brio
..



.. end

   
