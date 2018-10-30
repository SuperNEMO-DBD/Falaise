===================================
Variant repository ``"simulation"``
===================================


.. contents::

*SuperNEMO simulation configuration parameters*


General informations
====================

* Display name : *SuperNEMO Geant4 simulation variant repository*
* Organization : ``"snemo"``
* Application : ``"falaise"``
* Number of variant registries: ``4``

Registry ``"geometry"``
=======================

*SuperNEMO demonstrator geometry*


* Display name: ``"Demonstrator geometry"``
* Top variant model: ``"geometry.VM"``
* Number of supported parameters: ``16``

  * ``"layout"`` `(description) <geometry-layout_>`__
  * ``"layout/if_basic/magnetic_field"`` `(description) <geometry-layout-if_basic-magnetic_field_>`__
  * ``"layout/if_basic/magnetic_field/is_active/type"`` `(description) <geometry-layout-if_basic-magnetic_field-is_active-type_>`__
  * ``"layout/if_basic/magnetic_field/is_active/type/if_mapped/map"`` `(description) <geometry-layout-if_basic-magnetic_field-is_active-type-if_mapped-map_>`__
  * ``"layout/if_basic/magnetic_field/is_active/type/if_mapped/map/if_user/map_file"`` `(description) <geometry-layout-if_basic-magnetic_field-is_active-type-if_mapped-map-if_user-map_file_>`__
  * ``"layout/if_basic/magnetic_field/is_active/type/if_mapped/z_inverted"`` `(description) <geometry-layout-if_basic-magnetic_field-is_active-type-if_mapped-z_inverted_>`__
  * ``"layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/magnitude"`` `(description) <geometry-layout-if_basic-magnetic_field-is_active-type-if_uniform_vertical-magnitude_>`__
  * ``"layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/direction"`` `(description) <geometry-layout-if_basic-magnetic_field-is_active-type-if_uniform_vertical-direction_>`__
  * ``"layout/if_basic/source_layout"`` `(description) <geometry-layout-if_basic-source_layout_>`__
  * ``"layout/if_basic/source_layout/if_basic/thickness"`` `(description) <geometry-layout-if_basic-source_layout-if_basic-thickness_>`__
  * ``"layout/if_basic/source_layout/if_basic/material"`` `(description) <geometry-layout-if_basic-source_layout-if_basic-material_>`__
  * ``"layout/if_basic/source_calibration"`` `(description) <geometry-layout-if_basic-source_calibration_>`__
  * ``"layout/if_basic/source_calibration/is_active/type"`` `(description) <geometry-layout-if_basic-source_calibration-is_active-type_>`__
  * ``"layout/if_basic/shielding"`` `(description) <geometry-layout-if_basic-shielding_>`__
  * ``"layout/if_half_commissioning/gap"`` `(description) <geometry-layout-if_half_commissioning-gap_>`__
  * ``"calo_film_thickness"`` `(description) <geometry-calo_film_thickness_>`__



Description of parameters
~~~~~~~~~~~~~~~~~~~~~~~~~


1. Parameter ``"layout"`` :

.. _geometry-layout:


   *The geometry layout of the demonstrator module*

   * Full path: ``"geometry:layout"``
   * Model: ``"demonstrator.geometry.layout.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Associated variants : 

     * ``"if_basic"`` (model: ``"demonstrator.geometry.basic.VM"``)
     * ``"if_half_commissioning"`` (model: ``"demonstrator.geometry.half_commissioning.VM"``)

   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"Basic"``  : 

        * Triggered variant : ``"if_basic"``

     * ``"HalfCommissioning"``  : 

        * Triggered variant : ``"if_half_commissioning"``


   * Default value  : ``"Basic"``
   

2. Parameter ``"layout/if_basic/magnetic_field"`` :

.. _geometry-layout-if_basic-magnetic_field:


   *The activation flag of the magnetic field*

   * Full path: ``"geometry:layout/if_basic/magnetic_field"``
   * Model: ``"magnetic_field.on_off.PM"``
   * Type: ``boolean`` 
   * Mutability : ``variable``
   * Associated variants : 

     * ``"is_active"`` (model: ``"magnetic_field.description.VM"``)
     * ``"is_inactive"`` (model: ``"datatools::basic::is_off.VM"``)

   * Variable mode  : ``enumeration``
   * Supported boolean values:

     * ``false`` : 
        *No magnetic field*

        * Triggered variant : ``"is_inactive"``

     * ``true`` : 
        *Apply a magnetic field*

        * Triggered variant : ``"is_active"``


   * Default value  : ``true``
   

3. Parameter ``"layout/if_basic/magnetic_field/is_active/type"`` :

.. _geometry-layout-if_basic-magnetic_field-is_active-type:


   *The type of the magnetic field*

   * Full path: ``"geometry:layout/if_basic/magnetic_field/is_active/type"``
   * Model: ``"magnetic_field.type.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Associated variants : 

     * ``"if_mapped"`` (model: ``"magnetic_field.mapped.VM"``)
     * ``"if_uniform_vertical"`` (model: ``"magnetic_field.uniform_vertical.VM"``)

   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"Mapped"``  : 
        *Magnetic field is mapped*

        * Triggered variant : ``"if_mapped"``

     * ``"UniformVertical"``  : 
        *Magnetic field is uniform vertical*

        * Triggered variant : ``"if_uniform_vertical"``


   * Default value  : ``"UniformVertical"``
   

4. Parameter ``"layout/if_basic/magnetic_field/is_active/type/if_mapped/map"`` :

.. _geometry-layout-if_basic-magnetic_field-is_active-type-if_mapped-map:


   *Selection of the mapped magnetic field*

   * Full path: ``"geometry:layout/if_basic/magnetic_field/is_active/type/if_mapped/map"``
   * Model: ``"magnetic_field.mapped.map.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Associated variants : 

     * ``"if_map0"`` (model: ``"magnetic_field.mapped.map.map0.VM"``)
     * ``"if_user"`` (model: ``"magnetic_field.mapped.map.user.VM"``)

   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"Map0"``  : 
        *Registered mapped magnetic field #0*

        * Triggered variant : ``"if_map0"``

     * ``"User"``  : 
        *User defined mapped magnetic field*

        * Triggered variant : ``"if_user"``


   * Default value  : ``"Map0"``
   

5. Parameter ``"layout/if_basic/magnetic_field/is_active/type/if_mapped/map/if_user/map_file"`` :

.. _geometry-layout-if_basic-magnetic_field-is_active-type-if_mapped-map-if_user-map_file:


   *The mapping file of the user defined mapped magnetic field*

   * Full path: ``"geometry:layout/if_basic/magnetic_field/is_active/type/if_mapped/map/if_user/map_file"``
   * Model: ``"magnetic_field.mapped.map.user.map_file.PM"``
   * Type: ``string`` (as ``input path``)
   * Mutability : ``variable``
   * Variable mode  : ``free``
   

6. Parameter ``"layout/if_basic/magnetic_field/is_active/type/if_mapped/z_inverted"`` :

.. _geometry-layout-if_basic-magnetic_field-is_active-type-if_mapped-z_inverted:


   *The Z inversion of the mapped magnetic field*

   * Full path: ``"geometry:layout/if_basic/magnetic_field/is_active/type/if_mapped/z_inverted"``
   * Model: ``"magnetic_field.mapped.z_inverted.PM"``
   * Type: ``boolean`` 
   * Mutability : ``variable``
   * Associated variants : 

     * ``"is_active"`` (model: ``"datatools::basic::is_on.VM"``)
     * ``"is_inactive"`` (model: ``"datatools::basic::is_off.VM"``)

   * Variable mode  : ``enumeration``
   * Supported boolean values:

     * ``false`` : 
        *The mapped magnetic field points to the Z>0 axis*

        * Triggered variant : ``"is_inactive"``

     * ``true`` : 
        *The mapped magnetic field points to the Z<0 axis*

        * Triggered variant : ``"is_active"``


   * Default value  : ``false``
   

7. Parameter ``"layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/magnitude"`` :

.. _geometry-layout-if_basic-magnetic_field-is_active-type-if_uniform_vertical-magnitude:


   *The magnitude of the magnetic field*

   * Full path: ``"geometry:layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/magnitude"``
   * Model: ``"magnetic_field.uniform_vertical.magnitude.PM"``
   * Type: ``real`` 
   * Unit label : ``"magnetic_flux_density"``
   * Preferred unit : ``"gauss"``
   * Real precision : ``0 gauss``
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[0.1 gauss;100 gauss]``
   * Default value  : ``25 gauss``
   

8. Parameter ``"layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/direction"`` :

.. _geometry-layout-if_basic-magnetic_field-is_active-type-if_uniform_vertical-direction:


   *The direction of the magnetic field*

   * Full path: ``"geometry:layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/direction"``
   * Model: ``"magnetic_field.uniform_vertical.direction.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"+z"`` 
     * ``"-z"`` 

   * Default value  : ``"+z"``
   

9. Parameter ``"layout/if_basic/source_layout"`` :

.. _geometry-layout-if_basic-source_layout:


   *The layout of the source foil*

   * Full path: ``"geometry:layout/if_basic/source_layout"``
   * Model: ``"source_betabeta.layout.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Associated variants : 

     * ``"if_basic"`` (model: ``"source_betabeta.basic.VM"``)

   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"Basic"``  : 

        * Triggered variant : ``"if_basic"``


   * Default value  : ``"Basic"``
   

10. Parameter ``"layout/if_basic/source_layout/if_basic/thickness"`` :

.. _geometry-layout-if_basic-source_layout-if_basic-thickness:


    *The thickness of the source foil*

    * Full path: ``"geometry:layout/if_basic/source_layout/if_basic/thickness"``
    * Model: ``"source_betabeta.foil_thickness.PM"``
    * Type: ``real`` 
    * Unit label : ``"length"``
    * Preferred unit : ``"um"``
    * Real precision : ``0 um``
    * Mutability : ``variable``
    * Variable mode  : ``interval``
    * Domain: ``[5 um;500 um]``
    * Default value  : ``250 um``
    

11. Parameter ``"layout/if_basic/source_layout/if_basic/material"`` :

.. _geometry-layout-if_basic-source_layout-if_basic-material:


    *The material of the source foil*

    * Full path: ``"geometry:layout/if_basic/source_layout/if_basic/material"``
    * Model: ``"source_betabeta.foil_material.PM"``
    * Type: ``string`` 
    * Mutability : ``variable``
    * Associated variants : 

      * ``"if_ca48"`` (model: ``"source_betabeta.basic.ca48.VM"``)
      * ``"if_mo100"`` (model: ``"source_betabeta.basic.mo100.VM"``)
      * ``"if_nd150"`` (model: ``"source_betabeta.basic.nd150.VM"``)
      * ``"if_se82"`` (model: ``"source_betabeta.basic.se82.VM"``)

    * Variable mode  : ``enumeration``
    * Supported string values:

      * ``"Ca48"``  : 

         * Triggered variant : ``"if_ca48"``

      * ``"Mo100"``  : 

         * Triggered variant : ``"if_mo100"``

      * ``"Nd150"``  : 

         * Triggered variant : ``"if_nd150"``

      * ``"Se82"``  : 

         * Triggered variant : ``"if_se82"``


    * Default value  : ``"Se82"``
    

12. Parameter ``"layout/if_basic/source_calibration"`` :

.. _geometry-layout-if_basic-source_calibration:


    *The activation flag of the source calibration system*

    * Full path: ``"geometry:layout/if_basic/source_calibration"``
    * Model: ``"source_calibration.off_on.PM"``
    * Type: ``boolean`` 
    * Mutability : ``variable``
    * Associated variants : 

      * ``"is_active"`` (model: ``"source_calibration.description.VM"``)
      * ``"is_inactive"`` (model: ``"datatools::basic::is_off.VM"``)

    * Variable mode  : ``enumeration``
    * Supported boolean values:

      * ``false`` : 

         * Triggered variant : ``"is_inactive"``

      * ``true`` : 

         * Triggered variant : ``"is_active"``


    * Default value  : ``false``
    

13. Parameter ``"layout/if_basic/source_calibration/is_active/type"`` :

.. _geometry-layout-if_basic-source_calibration-is_active-type:


    *The type of the source calibration setup*

    * Full path: ``"geometry:layout/if_basic/source_calibration/is_active/type"``
    * Model: ``"source_calibration.type.PM"``
    * Type: ``string`` 
    * Mutability : ``variable``
    * Associated variants : 

      * ``"if_bi207"`` (model: ``"source_calibration.bi207.basic.VM"``)

    * Variable mode  : ``enumeration``
    * Supported string values:

      * ``"Bi207"``  : 

         * Triggered variant : ``"if_bi207"``


    * Default value  : ``"Bi207"``
    

14. Parameter ``"layout/if_basic/shielding"`` :

.. _geometry-layout-if_basic-shielding:


    *The activation flag of the shielding*

    * Full path: ``"geometry:layout/if_basic/shielding"``
    * Model: ``"datatools::basic::on_off.PM"``
    * Type: ``boolean`` 
    * Mutability : ``variable``
    * Associated variants : 

      * ``"is_off"`` (model: ``"datatools::basic::is_off.VM"``)
      * ``"is_on"`` (model: ``"datatools::basic::is_on.VM"``)

    * Variable mode  : ``enumeration``
    * Supported boolean values:

      * ``false`` : 

         * Triggered variant : ``"is_off"``

      * ``true`` : 

         * Triggered variant : ``"is_on"``


    * Default value  : ``true``
    

15. Parameter ``"layout/if_half_commissioning/gap"`` :

.. _geometry-layout-if_half_commissioning-gap:


    *The calibration source distance to closing plate*

    * Full path: ``"geometry:layout/if_half_commissioning/gap"``
    * Model: ``"demonstrator.geometry.half_commissioning.gap.PM"``
    * Type: ``real`` 
    * Unit label : ``"length"``
    * Preferred unit : ``"mm"``
    * Real precision : ``0 mm``
    * Mutability : ``variable``
    * Variable mode  : ``interval``
    * Domain: ``[0.25 mm;1000 mm]``
    * Default value  : ``0.25 mm``
    

16. Parameter ``"calo_film_thickness"`` :

.. _geometry-calo_film_thickness:


    *The thickness of the calorimeter tightness film*

    * Full path: ``"geometry:calo_film_thickness"``
    * Model: ``"demonstrator.geometry.calo_tightness_film_thickness.PM"``
    * Type: ``real`` 
    * Unit label : ``"length"``
    * Preferred unit : ``"um"``
    * Real precision : ``0 um``
    * Mutability : ``variable``
    * Variable mode  : ``interval``
    * Domain: ``[0 um;100 um]``
    * Default value  : ``25 um``
    



Registry ``"vertexes"``
=======================

*SuperNEMO demonstrator vertex generation*


* Display name: ``"Vertex generation"``
* Top variant model: ``"vertexes.VM"``
* Number of supported parameters: ``8``

  * ``"generator"`` `(description) <vertexes-generator_>`__
  * ``"generator/if_free_spot/x"`` `(description) <vertexes-generator-if_free_spot-x_>`__
  * ``"generator/if_free_spot/y"`` `(description) <vertexes-generator-if_free_spot-y_>`__
  * ``"generator/if_free_spot/z"`` `(description) <vertexes-generator-if_free_spot-z_>`__
  * ``"generator/if_half_commissioning_single_spot/column"`` `(description) <vertexes-generator-if_half_commissioning_single_spot-column_>`__
  * ``"generator/if_half_commissioning_single_spot/row"`` `(description) <vertexes-generator-if_half_commissioning_single_spot-row_>`__
  * ``"generator/if_source_calibration_single_spot/track"`` `(description) <vertexes-generator-if_source_calibration_single_spot-track_>`__
  * ``"generator/if_source_calibration_single_spot/position"`` `(description) <vertexes-generator-if_source_calibration_single_spot-position_>`__



Description of parameters
~~~~~~~~~~~~~~~~~~~~~~~~~


1. Parameter ``"generator"`` :

.. _vertexes-generator:


   *The selected primary vertex generator*

   * Full path: ``"vertexes:generator"``
   * Model: ``"vertexes.generator.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Associated groups : 

     * ``"Calibration"`` 
     * ``"HalfCommissioning"`` 
     * ``"Hall"`` 
     * ``"OpticalModule"`` 
     * ``"Shielding"`` 
     * ``"Source"`` 
     * ``"Tracker"`` 
     * ``"Tracker0"`` 
     * ``"Tracker1"`` 

   * Associated variants : 

     * ``"if_free_spot"`` (model: ``"free_spot_vertex.VM"``)
     * ``"if_half_commissioning_single_spot"`` (model: ``"half_commissioning.single_spot.VM"``)
     * ``"if_source_calibration_single_spot"`` (model: ``"source_calibration.single_spot.VM"``)

   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"anode_wire_bulk"``  : 
        *Vertex generation from the bulk volume of all anode wires*

        * Group : ``"Tracker"``

     * ``"anode_wire_surface"``  : 
        *Vertex generation from the surface of all anode wires*

        * Group : ``"Tracker"``

     * ``"calo_5inch_back_scin_bulk"``  : 
        *Vertex generation from the bulk volume of the back part of all main calorimeter scintillator blocks with 5'' PMT*

        * Group : ``"OpticalModule"``

     * ``"calo_5inch_front_scin_bulk"``  : 
        *Vertex generation from the bulk volume of the front part of all main calorimeter scintillator blocks with 5'' PMT*

        * Group : ``"OpticalModule"``

     * ``"calo_5inch_scin_bulk"``  : 
        *Vertex generation from the bulk volume of all main calorimeter scintillator blocks with 5'' PMT*

        * Group : ``"OpticalModule"``

     * ``"calo_8inch_back_scin_bulk"``  : 
        *Vertex generation from the bulk volume of the back part of all main calorimeter scintillator blocks with 8'' PMT*

        * Group : ``"OpticalModule"``

     * ``"calo_8inch_front_scin_bulk"``  : 
        *Vertex generation from the bulk volume of the front part of all main calorimeter scintillator blocks with 8'' PMT*

        * Group : ``"OpticalModule"``

     * ``"calo_8inch_scin_bulk"``  : 
        *Vertex generation from the bulk volume of all main calorimeter scintillator blocks with 8'' PMT*

        * Group : ``"OpticalModule"``

     * ``"calo_wrapper_bulk"``  : 
        *Vertex generation from the bulk volume of the wrapper of all main calorimeter scintillator blocks*

        * Group : ``"OpticalModule"``

     * ``"calo_wrapper_surface"``  : 
        *Vertex generation from the surface of the wrapper of all main calorimeter scintillator blocks*

        * Group : ``"OpticalModule"``

     * ``"commissioning_all_spots"``  : 
        *Vertex generation from from a commissioning spot*

        * Group : ``"HalfCommissioning"``

     * ``"commissioning_single_spot"``  : 
        *Vertex generation from from a commissioning spot*

        * Triggered variant : ``"if_half_commissioning_single_spot"``
        * Group : ``"HalfCommissioning"``

     * ``"experimental_hall_bulk"``  : 
        *Vertex generation from the bulk volume (air) of the experimental hall*

        * Group : ``"Hall"``

     * ``"experimental_hall_ground_bulk"``  : 
        *Vertex generation from the bulk volume of the experimental hall's ground*

        * Group : ``"Hall"``

     * ``"experimental_hall_ground_floor"``  : 
        *Vertex generation from the top surface (floor) of the experimental hall's ground*

        * Group : ``"Hall"``

     * ``"experimental_hall_roof"``  : 
        *Vertex generation from the top surface (roof) of the experimental hall*

        * Group : ``"Hall"``

     * ``"experimental_hall_surface"``  : 
        *Vertex generation from all internal surfaces of the experimental hall*

        * Group : ``"Hall"``

     * ``"feedthrough_pins_bulk_all_spots"``  : 
        *Vertex generation from the bulk volume of all tracker feedthrough pins*

        * Group : ``"Tracker"``

     * ``"feedthrough_pins_bulk_side_0_bottom"``  : 
        *Vertex generation from the bulk volume of the tracker feedthrough pins on side 0, bottom*

        * Group : ``"Tracker0"``

     * ``"feedthrough_pins_bulk_side_0_top"``  : 
        *Vertex generation from the bulk volume of the tracker feedthrough pins on side 0, top*

        * Group : ``"Tracker0"``

     * ``"feedthrough_pins_bulk_side_1_bottom"``  : 
        *Vertex generation from the bulk volume of the tracker feedthrough pins on side 1, bottom*

        * Group : ``"Tracker1"``

     * ``"feedthrough_pins_bulk_side_1_top"``  : 
        *Vertex generation from the bulk volume of the tracker feedthrough pins on side 1, top*

        * Group : ``"Tracker1"``

     * ``"field_wire_bulk"``  : 
        *Vertex generation from the bulk volume of all field wires*

        * Group : ``"Tracker"``

     * ``"field_wire_surface"``  : 
        *Vertex generation from the surface of all field wires*

        * Group : ``"Tracker"``

     * ``"free_spot"``  : 
        *Vertex generation from an arbitrary spot in the geometry*

        * Triggered variant : ``"if_free_spot"``

     * ``"gveto_wrapper_bulk"``  : 
        *Vertex generation from the bulk volume of the wrapper of all gamma veto scintillator blocks*

        * Group : ``"OpticalModule"``

     * ``"gveto_wrapper_surface"``  : 
        *Vertex generation from the surface of the wrapper of all gamma veto scintillator blocks*

        * Group : ``"OpticalModule"``

     * ``"shielding_all_bulk"``  : 
        *Vertex generation from the bulk volume of all shielding walls*

        * Group : ``"Shielding"``

     * ``"shielding_all_internal_surfaces"``  : 
        *Vertex generation from internal surfaces of the all shielding  walls*

        * Group : ``"Shielding"``

     * ``"shielding_back_front_bulk"``  : 
        *Vertex generation from the bulk volume of the back/front shielding walls*

        * Group : ``"Shielding"``

     * ``"shielding_back_front_internal_surface"``  : 
        *Vertex generation from all internal surfaces of the back/front shielding walls*

        * Group : ``"Shielding"``

     * ``"shielding_bottom_bulk"``  : 
        *Vertex generation from the bulk volume of the bottom shielding wall*

        * Group : ``"Shielding"``

     * ``"shielding_bottom_internal_surface"``  : 
        *Vertex generation from the internal surface of the bottom shielding wall*

        * Group : ``"Shielding"``

     * ``"shielding_left_right_bulk"``  : 
        *Vertex generation from the bulk volume of the left/right shielding walls*

        * Group : ``"Shielding"``

     * ``"shielding_left_right_internal_surface"``  : 
        *Vertex generation from all internal surfaces of the left/right shielding walls*

        * Group : ``"Shielding"``

     * ``"shielding_top_bulk"``  : 
        *Vertex generation from the bulk volume of the top shielding wall*

        * Group : ``"Shielding"``

     * ``"shielding_top_internal_surface"``  : 
        *Vertex generation from the internal surface of the top shielding wall*

        * Group : ``"Shielding"``

     * ``"source_calibration_all_spots"``  : 
        *Vertex generation from the bulk volume of all source calibration spots*

        * Group : ``"Calibration"``

     * ``"source_calibration_single_spot"``  : 
        *Vertex generation from the bulk volume of all source calibration spots*

        * Triggered variant : ``"if_source_calibration_single_spot"``
        * Group : ``"Calibration"``

     * ``"source_pads_bulk"``  : 
        *Vertex generation from the bulk volume of all source pads*

        * Group : ``"Source"``

     * ``"source_pads_external_bulk"``  : 
        *Vertex generation from the bulk volume of all outer source pads*

        * Group : ``"Source"``

     * ``"source_pads_external_surface"``  : 
        *Vertex generation from the surface of all outer source pads*

        * Group : ``"Source"``

     * ``"source_pads_internal_bulk"``  : 
        *Vertex generation from the bulk volume of all inner source pads*

        * Group : ``"Source"``

     * ``"source_pads_internal_surface"``  : 
        *Vertex generation from the surface of all inner source pads*

        * Group : ``"Source"``

     * ``"source_pads_surface"``  : 
        *Vertex generation from the surface of all source pads*

        * Group : ``"Source"``

     * ``"xcalo_wrapper_bulk"``  : 
        *Vertex generation from the bulk volume of the wrapper of all X-wall calorimeter scintillator blocks*

        * Group : ``"OpticalModule"``

     * ``"xcalo_wrapper_surface"``  : 
        *Vertex generation from the surface of the wrapper of all X-wall calorimeter scintillator blocks*

        * Group : ``"OpticalModule"``


   * Default value  : ``"free_spot"``
   

2. Parameter ``"generator/if_free_spot/x"`` :

.. _vertexes-generator-if_free_spot-x:


   *The vertex X coordinate*

   * Full path: ``"vertexes:generator/if_free_spot/x"``
   * Model: ``"free_vertex.coordinate.PM"``
   * Type: ``real`` 
   * Unit label : ``"length"``
   * Preferred unit : ``"mm"``
   * Real precision : ``0 mm``
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[-10 m;10 m]``
   * Default value  : ``0 mm``
   

3. Parameter ``"generator/if_free_spot/y"`` :

.. _vertexes-generator-if_free_spot-y:


   *The vertex Y coordinate*

   * Full path: ``"vertexes:generator/if_free_spot/y"``
   * Model: ``"free_vertex.coordinate.PM"``
   * Type: ``real`` 
   * Unit label : ``"length"``
   * Preferred unit : ``"mm"``
   * Real precision : ``0 mm``
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[-10 m;10 m]``
   * Default value  : ``0 mm``
   

4. Parameter ``"generator/if_free_spot/z"`` :

.. _vertexes-generator-if_free_spot-z:


   *The vertex Z coordinate*

   * Full path: ``"vertexes:generator/if_free_spot/z"``
   * Model: ``"free_vertex.coordinate.PM"``
   * Type: ``real`` 
   * Unit label : ``"length"``
   * Preferred unit : ``"mm"``
   * Real precision : ``0 mm``
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[-10 m;10 m]``
   * Default value  : ``0 mm``
   

5. Parameter ``"generator/if_half_commissioning_single_spot/column"`` :

.. _vertexes-generator-if_half_commissioning_single_spot-column:


   *Vertex horizontal position*

   * Full path: ``"vertexes:generator/if_half_commissioning_single_spot/column"``
   * Model: ``"half_commissioning.single_spot.column.PM"``
   * Type: ``integer`` 
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[0;112]``
   * Default value  : ``0``
   

6. Parameter ``"generator/if_half_commissioning_single_spot/row"`` :

.. _vertexes-generator-if_half_commissioning_single_spot-row:


   *Vertex vertical position*

   * Full path: ``"vertexes:generator/if_half_commissioning_single_spot/row"``
   * Model: ``"half_commissioning.single_spot.row.PM"``
   * Type: ``integer`` 
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[0;4]``
   * Default value  : ``0``
   

7. Parameter ``"generator/if_source_calibration_single_spot/track"`` :

.. _vertexes-generator-if_source_calibration_single_spot-track:


   *Calibration track number*

   * Full path: ``"vertexes:generator/if_source_calibration_single_spot/track"``
   * Model: ``"source_calibration.single_spot.track.PM"``
   * Type: ``integer`` 
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[0;5]``
   * Default value  : ``0``
   

8. Parameter ``"generator/if_source_calibration_single_spot/position"`` :

.. _vertexes-generator-if_source_calibration_single_spot-position:


   *Calibration source position in a track*

   * Full path: ``"vertexes:generator/if_source_calibration_single_spot/position"``
   * Model: ``"source_calibration.single_spot.position.PM"``
   * Type: ``integer`` 
   * Mutability : ``variable``
   * Variable mode  : ``interval``
   * Domain: ``[0;2]``
   * Default value  : ``0``
   



Registry ``"primary_events"``
=============================

*SuperNEMO demonstrator primary events generation*


* Display name: ``"Primary events"``
* Top variant model: ``"peg.VM"``
* Number of supported parameters: ``3``

  * ``"generator"`` `(description) <primary_events-generator_>`__
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

     * ``"Po212"``  : 
        *Po-212 decay*

        * Group : ``"Background"``

     * ``"Po214"``  : 
        *Po-214 decay*

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
        *Electron with monokinetic energy*

        * Triggered variant : ``"if_versatile"``
        * Group : ``"User"``


   * Default value  : ``"electron.1MeV"``
   

2. Parameter ``"generator/if_versatile/particle"`` :

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
   

3. Parameter ``"generator/if_versatile/energy"`` :

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
   



Registry ``"simulation"``
=========================

*SuperNEMO demonstrator Geant4 simulation*


* Display name: ``"Geant4 simulation"``
* Top variant model: ``"simulation.VM"``
* Number of supported parameters: ``5``

  * ``"physics_mode"`` `(description) <simulation-physics_mode_>`__
  * ``"physics_mode/if_constructors/em_model"`` `(description) <simulation-physics_mode-if_constructors-em_model_>`__
  * ``"physics_mode/if_list/id"`` `(description) <simulation-physics_mode-if_list-id_>`__
  * ``"production_cuts"`` `(description) <simulation-production_cuts_>`__
  * ``"output_profile"`` `(description) <simulation-output_profile_>`__



Description of parameters
~~~~~~~~~~~~~~~~~~~~~~~~~


1. Parameter ``"physics_mode"`` :

.. _simulation-physics_mode:


   *The Geant4 simulation physics mode*

   * Full path: ``"simulation:physics_mode"``
   * Model: ``"simulation.physics_mode.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Associated variants : 

     * ``"if_constructors"`` (model: ``"simulation.physics_mode.constructors.VM"``)
     * ``"if_list"`` (model: ``"simulation.physics_mode.list.VM"``)

   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"Constructors"``  : 

        * Triggered variant : ``"if_constructors"``

     * ``"List"``  : 

        * Triggered variant : ``"if_list"``


   * Default value  : ``"Constructors"``
   

2. Parameter ``"physics_mode/if_constructors/em_model"`` :

.. _simulation-physics_mode-if_constructors-em_model:


   *Electromagnetic processes Geant4 model*

   * Full path: ``"simulation:physics_mode/if_constructors/em_model"``
   * Model: ``"simulation.physics_mode.constructors.em_model.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"livermore"`` 
     * ``"penelope"`` 
     * ``"standard"`` 

   * Default value  : ``"standard"``
   

3. Parameter ``"physics_mode/if_list/id"`` :

.. _simulation-physics_mode-if_list-id:


   *The identifier of the Geant4 physics list*

   * Full path: ``"simulation:physics_mode/if_list/id"``
   * Model: ``"simulation.physics_mode.list.id.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"CHIPS"`` 
     * ``"FTFP_BERT"`` 
     * ``"FTFP_BERT_HP"`` 
     * ``"FTFP_BERT_TRV"`` 
     * ``"FTF_BIC"`` 
     * ``"LBE"`` 
     * ``"LHEP"`` 
     * ``"QBBC"`` 
     * ``"QGSC_BERT"`` 
     * ``"QGSP"`` 
     * ``"QGSP_BERT"`` 
     * ``"QGSP_BERT_CHIPS"`` 
     * ``"QGSP_BERT_HP"`` 
     * ``"QGSP_BIC"`` 
     * ``"QGSP_BIC_HP"`` 
     * ``"QGSP_FTFP_BERT"`` 
     * ``"QGSP_INCLXX"`` 
     * ``"QGS_BIC"`` 
     * ``"Shielding"`` 

   * Default value  : ``"LBE"``
   

4. Parameter ``"production_cuts"`` :

.. _simulation-production_cuts:


   *The activation flag of Geant4 production cuts*

   * Full path: ``"simulation:production_cuts"``
   * Model: ``"simulation.production_cuts_activation.PM"``
   * Type: ``boolean`` 
   * Mutability : ``variable``
   * Associated variants : 

     * ``"is_active"`` (model: ``"datatools::basic::is_on.VM"``)
     * ``"is_inactive"`` (model: ``"datatools::basic::is_off.VM"``)

   * Variable mode  : ``enumeration``
   * Supported boolean values:

     * ``false`` : 

        * Triggered variant : ``"is_inactive"``

     * ``true`` : 

        * Triggered variant : ``"is_active"``


   * Default value  : ``true``
   

5. Parameter ``"output_profile"`` :

.. _simulation-output_profile:


   *The output profile for Geant4*

   * Full path: ``"simulation:output_profile"``
   * Model: ``"simulation.output_profile.PM"``
   * Type: ``string`` 
   * Mutability : ``variable``
   * Associated variants : 

     * ``"if_all"`` (model: ``"datatools::basic::is_on.VM"``)
     * ``"if_calo"`` (model: ``"datatools::basic::is_on.VM"``)
     * ``"if_none"`` (model: ``"datatools::basic::is_on.VM"``)
     * ``"if_source"`` (model: ``"datatools::basic::is_on.VM"``)
     * ``"if_tracker"`` (model: ``"datatools::basic::is_on.VM"``)

   * Variable mode  : ``enumeration``
   * Supported string values:

     * ``"all_details"``  : 

        * Triggered variant : ``"if_all"``

     * ``"calo_details"``  : 

        * Triggered variant : ``"if_calo"``

     * ``"none"``  : 

        * Triggered variant : ``"if_none"``

     * ``"source_details"``  : 

        * Triggered variant : ``"if_source"``

     * ``"tracker_details"``  : 

        * Triggered variant : ``"if_tracker"``


   * Default value  : ``"none"``
   




Global dependency model
=======================

* Dependees : 8

  * Dependee slot [0] : ``"geometry:layout/if_half_commissioning"``
  * Dependee slot [1] : ``"geometry:layout/if_basic/source_calibration/is_active"``
  * Dependee slot [2] : ``"geometry:layout/if_basic/shielding/is_on"``
  * Dependee slot [3] : ``"geometry:layout/if_basic"``
  * Dependee slot [4] : ``"geometry:layout/if_basic/source_layout/if_basic/material/if_ca48"``
  * Dependee slot [5] : ``"geometry:layout/if_basic/source_layout/if_basic/material/if_se82"``
  * Dependee slot [6] : ``"geometry:layout/if_basic/source_layout/if_basic/material/if_nd150"``
  * Dependee slot [7] : ``"geometry:layout/if_basic/source_layout/if_basic/material/if_mo100"``

* Dependencies : 9

  * Dependency : ``"Ca48PrimariesDep"``

    * Depender : ``"primary_events:generator@DBD/Ca48"``
    * Input dependee slots : [4] 
    * Logic : ``"[4]"``
  * Dependency : ``"CalibrationVtxDep"``

    * Depender : ``"vertexes:generator@Calibration"``
    * Input dependee slots : [1] 
    * Logic : ``"[1]"``
  * Dependency : ``"HalfCommissioningVtxDep"``

    * Depender : ``"vertexes:generator@HalfCommissioning"``
    * Input dependee slots : [0] 
    * Logic : ``"[0]"``
  * Dependency : ``"Mo100PrimariesDep"``

    * Depender : ``"primary_events:generator@DBD/Mo100"``
    * Input dependee slots : [7] 
    * Logic : ``"[7]"``
  * Dependency : ``"Nd150PrimariesDep"``

    * Depender : ``"primary_events:generator@DBD/Nd150"``
    * Input dependee slots : [6] 
    * Logic : ``"[6]"``
  * Dependency : ``"Se82PrimariesDep"``

    * Depender : ``"primary_events:generator@DBD/Se82"``
    * Input dependee slots : [5] 
    * Logic : ``"[5]"``
  * Dependency : ``"ShieldingVtxDep"``

    * Depender : ``"vertexes:generator@Shielding"``
    * Input dependee slots : [2] 
    * Logic : ``"[2]"``
  * Dependency : ``"SourceVtxDep"``

    * Depender : ``"vertexes:generator@Source"``
    * Input dependee slots : [3] 
    * Logic : ``"[3]"``
  * Dependency : ``"Tracker1VtxDep"``

    * Depender : ``"vertexes:generator@Tracker1"``
    * Input dependee slots : [3] 
    * Logic : ``"[3]"``



