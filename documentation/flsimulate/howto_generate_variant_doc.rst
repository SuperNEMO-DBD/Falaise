===========================================================================
Generate PDF document about the simulation variant parameters (version 2.1)
===========================================================================

:Author: F.Mauger
:Date: 2017-03-03

For developpers only: this memo describes the steps to generate a PDF document
about the variant parameters used in SuperNEMO simulation (version 2.1).

1. From the Falaise ``resources`` source directory:

   .. code:: sh

      $ cd {Falaise source dir}/resources/
      $ bxvariant_inspector \
	     --datatools::resource-path="falaise@$(pwd)" \
	     --variant-config "@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/variants/repository.conf" \
	     --action doc > /tmp/FLSimulateVariantsDoc-2.1.rst

2. From the Falaise ``documentation/flsimulate`` source directory (because we need the ``../style/geo.sty`` file there):

   .. code:: sh

      $ cd {Falaise source dir}/documentation/flsimulate/
      $ tail -n +4 /tmp/FLSimulateVariantsDoc-2.1.rst > /tmp/FLSimulateVariantsDoc-2.1-mod.rst
      $ cat> /tmp/FLSimulateVariantsDoc-2.1-head.rst<<EOF
=============================================================
SuperNEMO Simulation Variant parameters (version 2.1)
=============================================================
EOF
      $ cat /tmp/FLSimulateVariantsDoc-2.1-head.rst \
            /tmp/FLSimulateVariantsDoc-2.1-mod.rst \
            > /tmp/FLSimulateVariantsDoc-2.1-mod2.rst

      $ rst2latex --documentclass=article \
	     --title="SuperNEMO Simulation Variant parameters (version 2.1)"\
	     --documentoptions=a4paper,a4wide,10pt \
	     --use-latex-docinfo \
	     --use-verbatim-when-possible \
	     --stylesheet="../style/geo" \
	     --use-latex-toc \
	     --syntax-highlight="long" \
	     --section-numbering \
	     /tmp/FLSimulateVariantsDoc-2.1-mod2.rst \
	     > /tmp/FLSimulateVariantsDoc-2.1.tex

      $ pdflatex /tmp/FLSimulateVariantsDoc-2.1.tex
      $ pdflatex /tmp/FLSimulateVariantsDoc-2.1.tex
      $ rm -f FLSimulateVariantsDoc-2.1.toc FLSimulateVariantsDoc-2.1.aux FLSimulateVariantsDoc-2.1.out FLSimulateVariantsDoc-2.1.log
