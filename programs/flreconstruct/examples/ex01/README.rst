==========================
FLreconstruct example 01
==========================

Step 1
======

Run the simulation:

.. code:: shell

   $ flsimulate \
      --verbosity "debug" \
      --mount-directory "flrecoex01@$(pwd)" \
      --config "flsimulate-ex01.conf" \
      --number-events 100 \
      --output-file "flsimuex01.brio"
..

Step 2
======

Visualize simulated data:

.. code:: shell

   $ flvisualize \
      --experiment-setup "urn:snemo:demonstrator:setup:2.0" \
      --variant-profile "flsimulate-ex01.profile" \
      --input-file flsimuex01.brio
..

Step 3
======

Run the reconstruction:

.. code:: shell

   $ flreconstruct \
      --verbosity "debug" \
      --mount-directory "flrecoex01@$(pwd)" \
      --modulo 10 \
      --config "flreconstruct-ex01.conf" \
      --input-file "flsimuex01.brio" \
      --output-file "flrecoex01.brio"
..

Step 2
======

Visualize simulated data:

.. code:: shell

   $ flvisualize \
      --experiment-setup "urn:snemo:demonstrator:setup:2.0" \
      --variant-profile "flsimulate-ex01.profile" \
      --input-file flrecoex01.brio
..



.. end

   
