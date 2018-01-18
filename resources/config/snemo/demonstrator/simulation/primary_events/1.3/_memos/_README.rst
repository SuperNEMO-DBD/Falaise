==========================================================
Notes on SuperNEMO demonstrator primary_events version 1.3
==========================================================



Testing Variant support
=======================

Run from the Falaise source directory:

.. code:: sh

   $ bxvariant_inspector \
          --datatools::resource-path="falaise@$(pwd)/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/variants/repository.conf" \
	  --action doc > flprimaries-1.3.rst
   $ rst2html flprimaries-1.3.rst > flprimaries-1.3.html


.. code:: sh

   $ bxvariant_inspector \
	  --datatools::resource-path="falaise@$(pwd)/resources" \
          --variant-config "@falaise:config/snemo/demonstrator/simulation/primary_events/1.3/variants/repository.conf" \
          --variant-gui \
	  --variant-store "myprofile.conf"
