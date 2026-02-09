==========================
FLsimulate example 01
==========================

Run the simulation:

.. code:: shell

   $ flsimulate \
      -d "flsimuex01@$(pwd)" \
      -c flsimulate-ex01.conf \
      -o flsimuex01.brio
..

Visualize simulated data:

.. code:: shell

   $ flvisualize --variant-profile flsimulate-ex01.profile -i flsimuex01.brio
..

Extract a slice of simulated event:

.. code:: shell

   $ flreconstruct -F 98 -N 2 -i flsimuex01.brio -o slice.xml
..


.. end

   
