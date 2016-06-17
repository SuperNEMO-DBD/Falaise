==============================
EventBrowser
==============================


Example
=======

1. Run flsimulate and generate a set of MC events with the ``HalfCommissioning``
   geometry layout:

.. raw:: sh

   $ flsimulate \
     --variant-set="geometry:layout=HalfCommissioning" \
     --variant-gui \
     --variant-store="hc1.rep" \
     --experiment "demonstrator" \
     --event-generator "Co60" \
     --vertex-generator "commissioning.all_spots" \
     --number 1000 \
     --modulo 100 \
     --output-file "hc1_mc.brio"
   $ ls -l hc1_mc.brio
   $ ls -l hc1.rep
   $ cat hc1.rep
 ..

2. Run flreconstruct:

.. raw:: sh

   $ flreconstruct \
     --variant-config="@falaise:config/snemo/demonstrator/simulation/geant4_control/2.0/variants/repository.conf" \
     --variant-load="hc1.rep" \
     --input-file "hc1_mc.brio" \
     --modulo=100 \
     --pipeline "__default__" \
     --output-file "hc1_mc.rec.brio"
   $ ls -l hc1_mc.rec.brio
..

3. Run flvisualize:

.. raw:: sh
   $ flvisualize \
     --variant-config="@falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf" \
     --variant-load="hc1.rep" \
     --input-file "hc1_mc.rec.brio"
..

.. @falaise:config/snemo/demonstrator/geometry/4.0/variants/repository.conf
.. @falaise:config/snemo/demonstrator/simulation/geant4_control/2.0/variants/repository.conf
