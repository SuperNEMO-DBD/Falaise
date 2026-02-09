

.. code:: shell

   $ flsimulate \
      -c simu.conf \
      -o testSimRC-sd.xml \
      -N 10
..

.. code:: shell

   $ flreconstruct \
     -p simrc.conf \
     -i testSimRC-sd.xml \
     -o testSimRC-rec-rc.xml
     
..

.. code:: shell

   $ flvisualize \
     -f simu.profile \
     -i testSimRC-rec-rc.xml
     
..
