============================================================================
Materials definition for SuperNEMO virtual geometry setup (common resources)
============================================================================

:Authors: François Mauger
:Date:    2013-10-18

.. contents::
   :depth: 3
..


Presentation
============

This  directory contains  the  files needed  to  describe some  common
isotopes/elements  and materials  used  by  various SuperNEMO  virtual
geometry setups.

 * Setup label is : ``common::geometry::materials``
 * Setup version is : ``1.0``

Files:
======

 * ``README.rst`` : this file,
 * ``elements.def`` : list of elements,
 * ``materials.def`` : list of materials,
 * ``materials_aliases.def`` : list of material aliases.

   * Fix: move ``PFTE`` to ``PTFE``
   * Fix: make ``snemo::PTFE`` alias for ``std::ptfe``
