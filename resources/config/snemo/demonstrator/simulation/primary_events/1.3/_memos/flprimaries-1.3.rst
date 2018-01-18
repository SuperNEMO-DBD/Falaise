==================================================
Variant repository ``"primary_events_generation"``
==================================================


.. contents::

*SuperNEMO primary event generation configuration parameters*


General informations
====================

* Display name : *SuperNEMO simulation primary events generation*
* Organization : ``"snemo"``
* Application : ``"falaise"``
* Number of variant registries: ``1``

Registry ``"primary_events"``
=============================

*SuperNEMO demonstrator primary events generation*


* Display name: ``"Primary events"``
* Top variant model: ``"peg.VM"``
* Number of supported parameters: ``6``

  * ``"generator"`` `(description) <primary_events-generator_>`__
  * ``"generator/if_flat_versatile/particle"`` `(description) <primary_events-generator-if_flat_versatile-particle_>`__
  * ``"generator/if_flat_versatile/energy_min"`` `(description) <primary_events-generator-if_flat_versatile-energy_min_>`__
  * ``"generator/if_flat_versatile/energy_max"`` `(description) <primary_events-generator-if_flat_versatile-energy_max_>`__
  * ``"generator/if_versatile/particle"`` `(description) <primary_events-generator-if_versatile-particle_>`__
  * ``"generator/if_versatile/energy"`` `(description) <primary_events-generator-if_versatile-energy_>`__



Description of parameters
~~~~~~~~~~~~~~~~~~~~~~~~~


1. Parameter ``"generator"`` :

.. _primary_events-generator:


   *The selected primary event generator*

   * Full path: ``"primary_events:generator"``
   * Model: ``"peg.generator.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Associated groups : 

     * ``"Background"`` 
     * ``"Calibration"`` 
     * ``"DBD/Ca48"`` 
     * ``"DBD/Mo100"`` 
     * ``"DBD/Nd150"`` 
     * ``"DBD/Se82"`` 
     * ``"Miscellaneous"`` 
     * ``"User"`` 

   * Associated variants : 

     * ``"if_flat_versatile"`` (model: ``"peg.generator.fvspg.VM"``)
     * ``"if_versatile"`` (model: ``"peg.generator.vspg.VM"``)

   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"Ac228"``  : 
        *Ac-228 decay*

        * Group : ``"Background"``

     * ``"Am241"``  : 
        *Am-241 decay*

        * Group : ``"Calibration"``

     * ``"Bi207"``  : 
        *Bi-207 decay*

        * Group : ``"Calibration"``

     * ``"Bi210"``  : 
        *Bi-210 decay*

        * Group : ``"Background"``

     * ``"Bi212"``  : 
        *Bi-212 decay*

        * Group : ``"Background"``

     * ``"Bi212_Po212"``  : 
        *Bi-212/Po-212 decay*

        * Group : ``"Background"``

     * ``"Bi214"``  : 
        *Bi-214 decay*

        * Group : ``"Background"``

     * ``"Bi214_Po214"``  : 
        *Bi-214/Po-214 decay*

        * Group : ``"Background"``

     * ``"Ca48.0nubb"``  : 
        *Neutrinoless double beta decay of Ca-48, 0nubb(mn)*

        * Group : ``"DBD/Ca48"``

     * ``"Ca48.0nubbM1"``  : 
        *Neutrinoless double beta decay of Ca-48, 0nubbM1*

        * Group : ``"DBD/Ca48"``

     * ``"Ca48.0nubbM2"``  : 
        *Neutrinoless double beta decay of Ca-48, 0nubbM2*

        * Group : ``"DBD/Ca48"``

     * ``"Ca48.0nubb_rhc_lambda_0"``  : 
        *Neutrinoless double beta decay of Ca-48, 0nubb(rhc-lambda) 0+ -> 0+ {2n}*

        * Group : ``"DBD/Ca48"``

     * ``"Ca48.0nubb_rhc_lambda_0_2"``  : 
        *Neutrinoless double beta decay of Ca-48, 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*}*

        * Group : ``"DBD/Ca48"``

     * ``"Ca48.2nubb"``  : 
        *Two neutrino double beta decay of Ca-48, 2nubb*

        * Group : ``"DBD/Ca48"``

     * ``"Co60"``  : 
        *Co-60 decay*

        * Group : ``"Calibration"``

     * ``"Cs137"``  : 
        *Cs-137 decay*

        * Group : ``"Calibration"``

     * ``"Eu152"``  : 
        *Eu-152 decay*

        * Group : ``"Background"``

     * ``"Eu154"``  : 
        *Eu-154 decay*

        * Group : ``"Background"``

     * ``"K40"``  : 
        *K-40 decay*

        * Group : ``"Background"``

     * ``"Mn54"``  : 
        *Mn-54 decay*

        * Group : ``"Calibration"``

     * ``"Mo100.0nubb"``  : 
        *Neutrinoless double beta decay of Mo-100, 0nubb(mn)*

        * Group : ``"DBD/Mo100"``

     * ``"Mo100.0nubbM1"``  : 
        *Neutrinoless double beta decay of Mo-100, 0nubbM1*

        * Group : ``"DBD/Mo100"``

     * ``"Mo100.0nubbM2"``  : 
        *Neutrinoless double beta decay of Mo-100, 0nubbM2*

        * Group : ``"DBD/Mo100"``

     * ``"Mo100.0nubb_rhc_lambda_0"``  : 
        *Neutrinoless double beta decay of Mo-100, 0nubb(rhc-lambda) 0+ -> 0+ {2n}*

        * Group : ``"DBD/Mo100"``

     * ``"Mo100.0nubb_rhc_lambda_0_2"``  : 
        *Neutrinoless double beta decay of Mo-100, 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*}*

        * Group : ``"DBD/Mo100"``

     * ``"Mo100.2nubb"``  : 
        *Two neutrino double beta decay of Mo-100, 2nubb*

        * Group : ``"DBD/Mo100"``

     * ``"Na22"``  : 
        *Na-22 decay*

        * Group : ``"Calibration"``

     * ``"Nd150.0nubb"``  : 
        *Neutrinoless double beta decay of Nd-150, 0nubb(mn)*

        * Group : ``"DBD/Nd150"``

     * ``"Nd150.0nubbM1"``  : 
        *Neutrinoless double beta decay of Nd-150, 0nubbM1*

        * Group : ``"DBD/Nd150"``

     * ``"Nd150.0nubbM2"``  : 
        *Neutrinoless double beta decay of Nd-150, 0nubbM2*

        * Group : ``"DBD/Nd150"``

     * ``"Nd150.0nubb_rhc_lambda_0"``  : 
        *Neutrinoless double beta decay of Nd-150, 0nubb(rhc-lambda) 0+ -> 0+ {2n}*

        * Group : ``"DBD/Nd150"``

     * ``"Nd150.0nubb_rhc_lambda_0_2"``  : 
        *Neutrinoless double beta decay of Nd-150, 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*}*

        * Group : ``"DBD/Nd150"``

     * ``"Nd150.2nubb"``  : 
        *Two neutrino double beta decay of Nd-150, 2nubb*

        * Group : ``"DBD/Nd150"``

     * ``"Pa231"``  : 
        *Pa-231 decay*

        * Group : ``"Background"``

     * ``"Pa234m"``  : 
        *Pa-234m decay*

        * Group : ``"Background"``

     * ``"Pb210"``  : 
        *Pb-210 decay*

        * Group : ``"Background"``

     * ``"Pb211"``  : 
        *Pb-211 decay*

        * Group : ``"Background"``

     * ``"Pb212"``  : 
        *Pb-212 decay*

        * Group : ``"Background"``

     * ``"Pb214"``  : 
        *Pb-214 decay*

        * Group : ``"Background"``

     * ``"Ra226"``  : 
        *Ra-226 decay*

        * Group : ``"Background"``

     * ``"Se82.0nubb"``  : 
        *Neutrinoless double beta decay of Se-82, 0nubb(mn)*

        * Group : ``"DBD/Se82"``

     * ``"Se82.0nubbM1"``  : 
        *Neutrinoless double beta decay of Se-82, 0nubbM1*

        * Group : ``"DBD/Se82"``

     * ``"Se82.0nubbM2"``  : 
        *Neutrinoless double beta decay of Se-82, 0nubbM2*

        * Group : ``"DBD/Se82"``

     * ``"Se82.0nubb_rhc_lambda_0"``  : 
        *Neutrinoless double beta decay of Se-82, 0nubb(rhc-lambda) 0+ -> 0+ {2n}*

        * Group : ``"DBD/Se82"``

     * ``"Se82.0nubb_rhc_lambda_0_2"``  : 
        *Neutrinoless double beta decay of Se-82, 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*}*

        * Group : ``"DBD/Se82"``

     * ``"Se82.2nubb"``  : 
        *Two neutrino double beta decay of Se-82, 2nubb*

        * Group : ``"DBD/Se82"``

     * ``"Sr90"``  : 
        *Sr-90 decay*

        * Group : ``"Background"``

     * ``"Th234"``  : 
        *Th-234 decay*

        * Group : ``"Background"``

     * ``"Tl207"``  : 
        *Tl-207 decay*

        * Group : ``"Background"``

     * ``"Tl208"``  : 
        *Tl-208 decay*

        * Group : ``"Background"``

     * ``"Y90"``  : 
        *Y-90 decay*

        * Group : ``"Background"``

     * ``"electron.100keV"``  : 
        *Electron with monokinetic energy @ 100 keV*

        * Group : ``"Miscellaneous"``

     * ``"electron.1MeV"``  : 
        *Electron with monokinetic energy @ 1 MeV*

        * Group : ``"Miscellaneous"``

     * ``"electron.200keV"``  : 
        *Electron with monokinetic energy @ 200 keV*

        * Group : ``"Miscellaneous"``

     * ``"electron.20keV"``  : 
        *Electron with monokinetic energy @ 20 keV*

        * Group : ``"Miscellaneous"``

     * ``"electron.2MeV"``  : 
        *Electron with monokinetic energy @ 2 MeV*

        * Group : ``"Miscellaneous"``

     * ``"electron.3MeV"``  : 
        *Electron with monokinetic energy @ 3 MeV*

        * Group : ``"Miscellaneous"``

     * ``"electron.50-2000keV_flat"``  : 
        *Electron with energy in the [50keV-2MeV] range*

        * Group : ``"Miscellaneous"``

     * ``"electron.500keV"``  : 
        *Electron with monokinetic energy @ 500 keV*

        * Group : ``"Miscellaneous"``

     * ``"electron.50keV"``  : 
        *Electron with monokinetic energy @ 50 keV*

        * Group : ``"Miscellaneous"``

     * ``"flat_versatile_generator"``  : 
        *Particle with flat energy*

        * Triggered variant : ``"if_flat_versatile"``
        * Group : ``"User"``

     * ``"gamma.100keV"``  : 
        *Gamma with monokinetic energy @ 100 keV*

        * Group : ``"Miscellaneous"``

     * ``"gamma.1MeV"``  : 
        *Gamma with monokinetic energy @ 1 MeV*

        * Group : ``"Miscellaneous"``

     * ``"gamma.20keV"``  : 
        *Gamma with monokinetic energy @ 20 keV*

        * Group : ``"Miscellaneous"``

     * ``"gamma.2615keV"``  : 
        *Gamma with monokinetic energy @ 2.615 MeV*

        * Group : ``"Miscellaneous"``

     * ``"gamma.2MeV"``  : 
        *Gamma with monokinetic energy @ 2 MeV*

        * Group : ``"Miscellaneous"``

     * ``"gamma.500keV"``  : 
        *Gamma with monokinetic energy @ 500 keV*

        * Group : ``"Miscellaneous"``

     * ``"gamma.50keV"``  : 
        *Gamma with monokinetic energy @ 50 keV*

        * Group : ``"Miscellaneous"``

     * ``"versatile_generator"``  : 
        *Particle with monokinetic energy*

        * Triggered variant : ``"if_versatile"``
        * Group : ``"User"``


   * Default value  : ``"electron.1MeV"``
   

2. Parameter ``"generator/if_flat_versatile/particle"`` :

.. _primary_events-generator-if_flat_versatile-particle:


   *The particle type*

   * Full path: ``"primary_events:generator/if_flat_versatile/particle"``
   * Model: ``"peg.generator.vspg.particle.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"alpha"`` 
     * ``"electron"`` 
     * ``"gamma"`` 
     * ``"neutron"`` 
     * ``"positron"`` 

   * Default value  : ``"gamma"``
   

3. Parameter ``"generator/if_flat_versatile/energy_min"`` :

.. _primary_events-generator-if_flat_versatile-energy_min:


   *The particle min energy (flat)*

   * Full path: ``"primary_events:generator/if_flat_versatile/energy_min"``
   * Model: ``"peg.generator.vspg.energy.PM"``
   * Type: ``real`` 
   * Unit label : ``"energy"``
   * Preferred unit : ``"keV"``
   * Real precision : ``0 keV``
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[1 keV;10000 keV]``
   * Default value  : ``1000 keV``
   

4. Parameter ``"generator/if_flat_versatile/energy_max"`` :

.. _primary_events-generator-if_flat_versatile-energy_max:


   *The particle max energy (flat)*

   * Full path: ``"primary_events:generator/if_flat_versatile/energy_max"``
   * Model: ``"peg.generator.vspg.energy.PM"``
   * Type: ``real`` 
   * Unit label : ``"energy"``
   * Preferred unit : ``"keV"``
   * Real precision : ``0 keV``
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[1 keV;10000 keV]``
   * Default value  : ``1000 keV``
   

5. Parameter ``"generator/if_versatile/particle"`` :

.. _primary_events-generator-if_versatile-particle:


   *The particle type*

   * Full path: ``"primary_events:generator/if_versatile/particle"``
   * Model: ``"peg.generator.vspg.particle.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"alpha"`` 
     * ``"electron"`` 
     * ``"gamma"`` 
     * ``"neutron"`` 
     * ``"positron"`` 

   * Default value  : ``"gamma"``
   

6. Parameter ``"generator/if_versatile/energy"`` :

.. _primary_events-generator-if_versatile-energy:


   *The particle energy (monokinetic)*

   * Full path: ``"primary_events:generator/if_versatile/energy"``
   * Model: ``"peg.generator.vspg.energy.PM"``
   * Type: ``real`` 
   * Unit label : ``"energy"``
   * Preferred unit : ``"keV"``
   * Real precision : ``0 keV``
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[1 keV;10000 keV]``
   * Default value  : ``1000 keV``
   



