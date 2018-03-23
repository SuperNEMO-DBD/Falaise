===================================================
SuperNEMO Demonstrator primary event generation
===================================================


History
=======

* Version 1.3:

  * Use addon generators from ``resources/config/common/simulation/primary_events/1.2/``

  * New generator ``Se82.2nubb_2MeV`` : Two neutrino double beta decay
    of Se-82, 2nubb with Esum > 2 MeV (from ``resources/config/common/simulation/primary_events/1.2/generators/dbd_more.def``)

  * New  generator ``flat_versatile_generator``  : Arbitrary  particle
    with flat energy in [100keV-10MeV] range (with variant support).

    This variant generator supports an energy range for which users can select
    the minimum and maximum energies. The Bayeux variant system cannot ensure Emin < Emax
    so users must take care to the validity of the energy range they choose.


* Version 1.2:

  * Initial set of generators.
