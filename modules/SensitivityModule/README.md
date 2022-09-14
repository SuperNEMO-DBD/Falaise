# SensitivityModule readme


Cheryl Patrick (UCL)
Last updated Jan 4, 2018

SensitivityModule  is a Falaise pipeline module to process selected data from the SD, CD, TCD, TTD and PTD banks and output a ROOT ntuple file. It has significant overlap with the ParticleID module, but is not the same. In the long term, there is scope for consolidation.


## Files:

- SensitivityModule.cpp
- SensitivityModule.h
- CMakeLists.txt
- SensitivityModuleExample.conf.in


## Description

Add to an flreconstruct pipeline to generate a ROOT ntuple file with some pertinent branches. To build it, do

``` console
$ ls
CMakeLists.txt                   SensitivityModule.h
README.md                        SensitivityModuleExample.conf.in
SensitivityModule.cpp
$ mkdir build
$ cd build
$ cmake -DCMAKE_PREFIX_PATH=<pathtoyourfalaiseinstall> ..
...
$ make
...
... If you are developing the module, you can test it by doing ...
$ make test
```

Note: if you get a QT5 error, you may need to specify the QT5 path when you run the cmake line, as given by `brew --prefix qt5-base`. For example, you can run:
``` console
$ cmake -DCMAKE_PREFIX_PATH="$(brew --prefix qt5-base);$(brew --prefix)" ..
``` 

The build will create the `libSensitivityModule` shared library plus the example `flreconstruct` pipeline
script `SensitivityModuleExample.conf`. Assuming that you have an `input.brio` file that contains
the `SD`, `CD`, `TCD`, `TTD` and `PTD` banks from the full reconstruction pipeline of `flreconstruct`
(up to and including gamma clustering), this can be run as:

``` console
... Assume we run in the build dir ...
$ ls
CMakeCache.txt                SensitivityModuleExample.conf
CMakeFiles                    cmake_install.cmake
Makefile
...
$ flreconstruct -i /path/to/input.brio -p SensitivityModuleExample.conf
...
$ ls
CMakeCache.txt                SensitivityModuleExample.conf
CMakeFiles                    cmake_install.cmake
Makefile                      sensitivity.root
```

The output file will by default be called `sensitivity.root` so don’t run it multiple times concurrently in the same directory
or you will overwrite the previous file! Use the falaise flreconstruct pipeline instructions to see how to integrate this module in your pipeline.

There is now the option to configure the output filename in the module configuration file.
The final two lines of the configuration file must read:

[name="processing" type="SensitivityModule"]
filename_out : string[1] = "my_filename.root"


## Output tuple structure - standard cuts

**reco.passes_two_calorimeters** : True if there are exactly 2 reconstructed calorimeter hits over 50 keV, of which at least 1 is over 150keV (CD bank)

**reco.passes_two_plus_calos**  :True if there are at least 2 reconstructed calorimeter hits over 50 keV, of which at least 1 is over 150keV (CD bank)

**reco.passes_two_clusters**  : True if there are exactly two reconstructed clusters with 3 or more hits (TCD bank)

**reco.passes_two_tracks** : True if there are exactly two reconstructed tracks (TTD bank)

**reco.passes_associated_calorimeters** : True if there are exactly two tracks, and they are both associated to one or more calorimeter hits. Equivalent (at the moment to a 2-electron topology)

## Output tuple structure - basic counts

**reco.calorimeter_hit_count** : Number of calorimeter hits

**reco.cluster_count** : Number of clusters with 3 or more hits

**reco.track_count** : Number of tracks in the tracker

**reco.associated_track_count** : Number of electron candidates

**reco.small_cluster_count** : Number of clusters with 2 hits

**reco.delayed_hit_count** : Number of delayed tracker hits (delay time is configurable, but is typically set to somewhere around 10-20 microseconds. Delayed hits are typically reconstructed as alphas, if they meet certain criteria)


## Output tuple structure - reconstructed particles

**reco.number_of_electrons** : Number of electron-candidate tracks - that is, tracks that are associated to one or more calorimeter hits. No check on the charge as of yet (but you can do it yourself looking at electron_charges). No requirement for a foil vertex. (PTD bank)

**reco.electron_charges** : Vector of all electron-candidate charges. In descending order of energy. 1 = undefined (straight track), 4 = positive, 8 = negative.

**reco.number_of_gammas** : Number of gamma candidates - when calorimeter hits that aren’t associated to tracks have been grouped by the gamma tracko-clustering algorithm to correspond to what appear to be individual gammas (PTD bank)

**reco.alpha_count** : Number of alpha candidates (delayed clusters of 1 or more tracker hits with no associated calorimeter hit)

## Output tuple structure - energies

**reco.total_calorimeter_energy** : Summed energy of all reconstructed calorimeter hits (CD bank)

**reco.higher_electron_energy** : Energy of the highest-energy electron candidate, summed over all associated calorimeter hits (at the moment I don’t think more than 1 hit is allowed, but that could change in future). 0 if no electron candidates. Corresponds to ** reco.electron_energies[0].

**reco.lower_electron_energy** : Energy of the second-highest-energy electron candidate, summed over all associated calorimeter hits (at the moment I don’t think more than 1 hit is allowed, but that could change in future). 0 if less than 2 electron candidates. Corresponds to reco.electron_energies[1].

**reco.electron_energies** : Vector of all electron-candidate energies. In descending order of energy.

**reco.gamma_energies** : Vector of all electron-candidate energies. In descending order of energy.

**reco.highest_gamma_energy** : Highest energy gamma, may come from more than 1 calorimeter hit as specified by gamma tracko- clustering

## Output tuple structure - Tracks and vertices

**reco.electron_track_lengths** : Vector of all electron-candidate track lengths in mm. In descending order of energy.

**reco.electron_hit_counts** : Vector of the number of tracker hits in the track for each electron candidate. In descending order of energy.


**reco.first_vertex_x** : If there are two tracks, vertex x position of an arbitrary “first” track. Foil is at x ~ 0, main calo walls are at +/- 434.994 mm according to flvisualize. X direction is Italy to France. Unit is mm. **Legacy: for 2 tracks only.**

**reco.first_vertex_y** : If there are two tracks, vertex y position of an arbitrary “first” track. The y direction is horizontal, parallel to the foil, you can see it in top view (Tunnel to mountain). Unit is mm. X-wall is at +/- 2505.494 according to flvisualize. Unit is mm. **Legacy: for 2 tracks only.**

**reco.first_vertex_z** : If there are two tracks, vertex z position of an arbitrary “first” track. The z direction is vertical, parallel to the wires, you can see it in side view. Unit is mm. **Legacy: for 2 tracks only.**

**reco.second_vertex_x** : If there are two tracks, vertex x position of an arbitrary “2nd” track. Foil is at x ~ 0, main calo walls are at +/- 434.994 mm according to flvisualize. Unit is mm. **Legacy: for 2 tracks only.**

**reco.second_vertex_y** :  If there are two tracks, vertex y position of an arbitrary “2nd” track. The y direction is horizontal, parallel to the foil, you can see it in top view. Unit is mm. **Legacy: for 2 tracks only.**

**reco.second_vertex_z** : If there are two tracks, vertex z position of an arbitrary “2nd” track. The z direction is vertical, parallel to the wires, you can see it in side view. Unit is mm. **Legacy: for 2 tracks only.**

**reco.first_proj_vertex_y; reco.first_proj_vertex_z; reco.second_proj_vertex_y; reco.second_proj_vertex_z** : Only for 2-electron or 1e-n-gamma topologies. If both tracks were linearly projected back to the foil (x=0), these would be their y and z coordinates.  Unit is mm. **Legacy: for 2 tracks only.**

**reco.vertex_separation** : Distance between the inner-most (nearest to the foil) vertices of the two tracks (only if there are 2 electron candidates).  Unit is mm.

**reco.foil_projection_separation** : Only if there are 2 electron candidates. If both tracks were linearly projected back to the foil, this is the distance between where the tracks intersect the foil.  Unit is mm.

**reco.projection_distance_xy** : How far in the xy plane we had to project our longest-projected track. Proxy for how many cells we are claiming to have a track, but where we didn’t reconstruct a hit. Could be replaced by a mapping of broken cells etc.

**reco.foil_vertex_count** : Number of charged particle tracks with a vertex on the foil.

**reco.vertices_in_tracker** : Number of charged particle tracks with a vertex on the foil or on the wires (was `reco.vertices_on_foil`).

**reco.electrons_from_foil** : Vector of booleans corresponding to the electron candidates in descending order of energy. True if the electron candidate has a vertex on the source foil (only), false if not.

**reco.electron_vertex_x(y,z)** : Vector of the x(y,z) position of the foilmost vertex of each electron candidate in descending order of energy.  The x direction is Italy to France (foil is at x ~ 0). The y direction is horizontal, parallel to the foil, you can see it in top view (tunnel to mountain).  The z direction is vertical (0 in the middle).

**reco.electron_dir_x(y,z)** : Vector of the x(y,z) component of a unit vector in the initial direction of the electron track at its foilmost end, assuming it travels away from the foil. 1 vector entry for each electron candidate in descending order of energy.

**reco.electron_proj_vertex_x(y,z)** : Vector of the x(y,z) component of where the track would intersect the foil, if projected back with a straight line in its initial direction (thus x should be 0 or negligible). In future, we should do helix projection. 1 vector entry for each electron candidate in descending order of energy.

**reco.alpha_vertex_x(y,z), reco.alpha_dir_x(y,z), reco.alpha_proj_vertex_x(y,z)** : As with the electron equivalents, but for alphas. In this case there is no energy ordering (we don't measure alphas' energy). It is anticipated that there will rarely be more than 1 entry in these vectors except in the case of misreconstruction.

**reco.edgemost_vertex** : Absolute y position (in mm) of the vertex that is nearest to the edge of the detector in the y dimension. This could possibly be used with small cluster identification to find events near the edge of the detector who have two tracks, each associated with a calorimeter and with close vertices on the foil, but for one of which there are only 2 hits (because it is too near the edge to pass through 3 cells).



## Output tuple structure - topologies

**reco.topology_1e1gamma** : True if event has 1 electron candidate (track with associated hits) and 1 gamma candidate (collection of unassociated hits with timings corresponding to a single gamma)

**reco.topology_1e1alpha** : True if event has 1 electron candidate (track with associated hits) and 1 alpha candidate (cluster of delayed tracker hits with no associated calorimeter hit)

**reco.topology_1engamma** : True if event has 1 electron candidate (track with associated hits) and 1 or more gamma candidates (collections of unassociated hits with timings corresponding to a gamma)

**reco.topology_2e** : True if event has a 2-electron topology (2 tracks with associated calorimeter hits, no gammas, no other tracks). False if not.

**reco.topology_1e** : True if event has a 1-electron topology (1 track with associated calorimeter hits, no gammas, no other tracks). False if not.

## Output tuple structure - Multi-track topology info

**reco.angle_between_tracks** : For 2-electron events: Angle between the initial momentum vectors of the two tracks. Does not require them to share a vertex (maybe it should). Similar for 1e-1 alpha. For 1-electron-n-gamma events, this calculates the angle between the electron track and the highest-energy gamma track associated to the electron vertex. The calculation will be rough as it will assume that the gamma impacts the centre of the optical module - the best we can do, as we have no tracking for gammas, is to assume a straight line from the electron vertex to the centre of the first activated calorimeter. It would be possible to estimate an uncertainty based on the geometry, if one felt so inclined, but this is not done.

**reco.same_side_of_foil** : If 2 tracks: True if both tracks are on the same side of the foil, false if not

**reco.first_track_direction_x; reco.first_track_direction_y ;                          **reco.first_track_direction_z; reco.second_track_direction_x;                              **reco.second_track_direction_y ; reco.second_track_direction_z** :  Initial direction vectors for the two tracks (Only if two tracks, arbitrary which is which)

**reco.internal_probability** :  Calculates the probability that it is an internal event (both tracks are initiated in the foil). Available for 2-electron events, or 1-electron-n-gamma events, in which case it uses the path of the highest energy gamma. If internal, this should be equally distributed from 0 to 1. If external (particle leaves one calorimeter, travels to foil, then to another calorimeter) this will be very close to 0. Calculated from energy and time of calorimeter hits vs length of tracks.

**reco.external_probability** : If 2 associated tracks, this calculates the probability that it is an external event (particle leaves one calorimeter, travels to foil, then to another calorimeter).   Available for 2-electron events, or 1-electron-n-gamma events, in which case it uses the path of the highest energy gamma. If external, this should be equally distributed from 0 to 1. If internal  (both tracks are initiated in the foil) this will be very close to 0. Calculated from energy and time of calorimeter hits vs length of tracks.

**reco.foil_projected_internal_probability;
reco.foil_projected_external_probability** : As internal and external probability, if each track’s length were extended to project the track linearly back to the foil

**reco.calo_hit_time_separation** : If 2 calorimeter hits  - time delay in nanoseconds between the hits. Used in the past as a crude proxy for internal/external probability.

## Output tuple structure - alpha finding

**reco.delayed_track_time** : If a delayed track, the time of the first delayed hit in ns. This is a vector with an entry for each reconstructed alpha candidate.

**reco.delayed_cluster_hit_count** : Number of hits in the delayed cluster. Used to determine the correct
metric for calculating the alpha track and alpha projected track lengths

**reco.foil_alpha_count** : Number of alphas with a vertex identified as being on the source foil (only)

**reco.alpha_track_length** : Length in mm of the delayed track. This length is calculated in different ways depending on the number of hits in the delayed cluster. This is due to the way < 3 hit tracks are treated by alpha finder. More detail is provided in code comments.

**reco.proj_track_length_alpha** : Length in mm of delayed track when it is projected back to the back to the electron projected foil vertex. Again this calculation is different for delayed tracks with 1,2 or >2 hits. See code for more detail.

**reco.alpha_crosses_foil** : True if an alpha track has been reconstructed which crosses the foil. This is unlikely to happen in real life, but alpha finder allows this in its short track reconstruction. This is considered a bug in alpha finder that should be fixed - but until then, this variable allows a cut on those events.

## Output tuple structure - calorimeter positions

**reco.electron_hits_mainwall;
reco.electron_hits_xwall;
reco.electron_hits_gveto** :
Vectors corresponding to each reconstructed electron in the event, starting from the most energetic and decreasing in energy order. For each one, if the associated calo hit for that electron was in the specified wall, it will be true, otherwise false. (Therefore, for each entry number, only 1 of these three should be true).

**reco.gamma_fractions_mainwall;
reco.gamma_fractions_xwall;
reco.gamma_fractions_gveto** :
Vectors corresponding to each reconstructed gamma in the event, starting from the most energetic and decreasing in energy order. For each one, they give the fraction of the energy of that gamma that was deposited in the given wall. (With gamma tracker, a given gamma can scatter to several calorimeters). (Therefore, for each entry number, the values for the 3 vectors should add up to 1).

**reco.gamma_hits_mainwall;
reco.gamma_hits_xwall;
reco.gamma_hits_gveto** :
Vectors corresponding to each reconstructed gamma in the event, starting from the most energetic and decreasing in energy order. For each one, if the FIRST (in time) hit for that gamma was in the specified wall, it will be true, otherwise false. (Therefore, for each entry number, only 1 of these three should be true).


## Output tuple structure - For simulation only

**true.highest_primary_energy** : The energy of the highest-energy primary particle in the interaction (from SD bank)

**true.second_primary_energy** : The energy of the second-highest-energy primary particle in the interaction (from SD bank)

**true.total_energy** : Summed energy of every primary particle in the interaction (from SD bank)

**true.vertex_x** : X coordinate of the true event vertex (generator level, from SD bank). Foil is at x ~ 0, main calo walls are at +/- 434.994 mm according to flvisualize. Unit is mm.

**true.vertex_y** : Y coordinate of the true event vertex (generator level, from SD bank).  The y direction is horizontal, parallel to the foil, you can see it in top view. Unit is mm.

**true.vertex_z** : Z coordinate of the true event vertex (generator level, from SD bank).  The z direction is vertical, parallel to the wires, you can see it in side view. Unit is mm.

**true.higher_particle_type** : The particle type (electron, gamma, positron) of the highest energy particle
in the event. (1 - gamma, 3-electron, 47- alpha)

**true.lower_particle_type** : The particle type (electron, gamma, positron) of the lowest energy particle
in the event. (1 - gamma, 3-electron, 47- alpha)


