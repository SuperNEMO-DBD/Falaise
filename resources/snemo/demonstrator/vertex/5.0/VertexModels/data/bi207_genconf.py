#!/usr/bin/python3
# Authors: F.Mauger, Y.Lemière
# Date: 2021-09-16 
# Description:

"""
Generate definition files for vertex generators associated to 
Bi207 calibration sources and useful composite generators.

Needs the ``bi207_calibration_sources.lis`` CSV files.

Usage:

   $ cd Falaise.git/resources/snemo/demonstrator/vertex/5.0/VertexModels/data
   $ find . -name "bi207_calibration_sources.lis" 
   $ python3 bi207_genconf.py
   $ ls -l bi207_calibration_source_*_vg.conf

Then copy the generated files typically in the ``Falaise.git/resources/snemo/demonstrator/vertex/5.0/VertexModels/`` directory.

Generated files:

  * ``sds_bi207_calibration_source_spots_vg.conf`` : configuration of each individual Bi207 calibration source (identified through its track and position IDs
  * ``sds_bi207_calibration_source_track_0_vg.conf`` : configuration of the weighted vertex generator combining all Bi207 calibration source in track 0
  * ``sds_bi207_calibration_source_track_1_vg.conf`` : configuration of the weighted vertex generator combining all Bi207 calibration source in track 1
  * ``sds_bi207_calibration_source_track_2_vg.conf`` : configuration of the weighted vertex generator combining all Bi207 calibration source in track 2  
  * ``sds_bi207_calibration_source_track_3_vg.conf`` : configuration of the weighted vertex generator combining all Bi207 calibration source in track 3
  * ``sds_bi207_calibration_source_track_4_vg.conf`` : configuration of the weighted vertex generator combining all Bi207 calibration source in track 4
  * ``sds_bi207_calibration_source_track_5_vg.conf`` : configuration of the weighted vertex generator combining all Bi207 calibration source in track 5
  * ``sds_bi207_calibration_source_all_tracks_vg.conf`` : configuration of the weighted vertex generator combining all track generators
  
"""

import sys

class bi207_calibration_source:

    def __init__(self):
        self.source_id              = -1
        self.source_row             = -1
        self.source_col             = -1
        self.source_activity        = -1.0

    def display(self):
        print("\n**** Calibration source dump ****")
        print("Source : ID=%d" % (self.source_id))
        print("    |-> row               : %d" % self.source_row)
        print("    |-> column            : %d" % self.source_col)
        print("    `-> Activity          : %f Bq\n" % self.source_activity)


class bi207_calibration_source_vg_generator :

    def __init__(self) :
        self.bi207_calibration_sources = []
        # Load description of Bi207 calib sources:
        bi207_calibration_sources_desc_filename = "bi207_calibration_sources.lis"
        bi207_calibration_sources_desc_file = open(bi207_calibration_sources_desc_filename, "r")
        source_counter = 0
        rawlines = bi207_calibration_sources_desc_file.readlines()
        for rawline in rawlines:
            line = rawline[:-1]
            if line[0] == "#":
                continue
            splitted_line = line.split()
            if splitted_line[0] == "#ID":
                continue
            a_source = bi207_calibration_source()
            a_source.source_id  = int(splitted_line[0])
            a_source.source_row = int(splitted_line[1])
            a_source.source_col = int(splitted_line[2])
            a_source.source_activity = float(splitted_line[3])
            source_counter = source_counter + 1
            self.bi207_calibration_sources.append(a_source)
        print("--> Description of Bi207 calibration sources is loaded.")
        bi207_calibration_sources_desc_file.close()
        return

    def generate_bulk_vg_def(self):
            
        calibration_indiv_vg_def_filename = "sds_bi207_calibration_source_spots_vg.conf"
        calibration_indiv_vg_def_file     = open(calibration_indiv_vg_def_filename, "w")
        calibration_indiv_vg_def_file.write('# -*- mode: conf-unix; -*-\n')
        calibration_indiv_vg_def_file.write('''
#@description A list of vertex generators
#@key_label   "name"
#@meta_label  "type"
''')

        for sid in range(len(self.bi207_calibration_sources)):
            current_col = self.bi207_calibration_sources[sid].source_col # track (horizontal y+)
            current_row = self.bi207_calibration_sources[sid].source_row # position (vertical z+)
            current_act = self.bi207_calibration_sources[sid].source_activity # Bq
    
            calibration_indiv_vg_def_file.write('\n##########################################################################\n')
            calibration_indiv_vg_def_file.write('''
[name="sds_bi207_calibration_source_spot_%d_%d" type="genvtx::cylinder_model_vg"]

#@config Bulk vertex generator for the Bi207 calibration source at (track=%d,position=%d);variant=if_source_calibration_sds_track_%d_vg

#@description Logging priority
logging.priority : string = "fatal"

#@description Geometry version requirement (not working with version lower than 5.0)
geometry.setup_requirement : string = "snemo::demonstrator(>=5)"

#@description Geometry origin of vertex in term of geometry category and addresses (geom ID)
origin : string = "category='source_calibration_spot' module={0} track={%d} position={%d}"

#@description Generation mode ("surface" or "bulk")
mode : string = "bulk"
''' % (current_col, current_row, current_row, current_col, current_col, current_col, current_row))
            pass

        calibration_indiv_vg_def_file.write('''\n# end''')
        calibration_indiv_vg_def_file.close()

        track_activity_map={}
        for track_id in range(6):
            track_activity_map[track_id] = 0.0
            calibration_track_vg_def_filename = "sds_bi207_calibration_source_track_%d_vg.conf" % (track_id)
            calibration_track_vg_def_file     = open(calibration_track_vg_def_filename, 'w')
            calibration_track_vg_def_file.write('# -*- mode: conf-unix; -*-\n')
            calibration_track_vg_def_file.write('''
#@description A list of vertex generators
#@key_label   "name"
#@meta_label  "type"
''')

            calibration_track_vg_def_file.write('''
###########################################################
# Generate vertex on bulk for Bi207 calibration track %d  #
###########################################################
[name="sds_bi207_calibration_source_tracks_%d" type="genvtx::combined_vg"]

#@config Generate vertexes on all Bi207 calibration source in track %d using weights from realistic individual sources' activities;variant=if_source_calibration_sds_track_%d_vg

#@description Logging priority
logging.priority  : string = "fatal"

#@description Geometry version requirement (not working with version lower than 5.0)
geometry.setup_requirement : string = "snemo::demonstrator(>=5)"

#@description Vertex generator names
generators : string[7]  = \\
''' % (track_id, track_id, track_id, track_id))

            spot_activity_map={}
            count=0
            for sid in range(len(self.bi207_calibration_sources)):
                current_track = self.bi207_calibration_sources[sid].source_col # track (horizontal y+)
                current_row = self.bi207_calibration_sources[sid].source_row # position (vertical z+)
                current_act = self.bi207_calibration_sources[sid].source_activity # Bq
                if current_track != track_id :
                    continue
                count = count + 1
                calibration_track_vg_def_file.write("    \"sds_bi207_calibration_source_spot_%d_%d\" " % (track_id, current_row))
                spot_activity_map["sds_bi207_calibration_source_spot_%d_%d" % (track_id, current_row)] = current_act
                track_activity_map[track_id] += current_act
                if count < 7 :
                    calibration_track_vg_def_file.write("\\")
                calibration_track_vg_def_file.write("\n")

            calibration_track_vg_def_file.write("\n#@description Vertex generator weights\n")
            for spot_key in spot_activity_map.keys() :
                splot_act = spot_activity_map[spot_key]
                calibration_track_vg_def_file.write("generators.%s.absolute_weight : real = %.2f # Bq \n"
                                                    % (spot_key, splot_act))
                            
            calibration_track_vg_def_file.write('''\n# end''')
            calibration_track_vg_def_file.close()
            continue
        print("[debug] track_activity_map = ", track_activity_map)
        calibration_all_tracks_vg_def_filename = "sds_bi207_calibration_source_all_tracks_vg.conf"
        calibration_all_tracks_vg_def_file     = open(calibration_all_tracks_vg_def_filename, 'w')
        calibration_all_tracks_vg_def_file.write('# -*- mode: conf-unix; -*-\n')
        calibration_all_tracks_vg_def_file.write('''
#@description A list of vertex generators
#@key_label   "name"
#@meta_label  "type"
''')

        calibration_all_tracks_vg_def_file.write('''
############################################################
# Generate vertex on bulk for all Bi207 calibration tracks #
############################################################
[name="sds_bi207_calibration_source_all_tracks" type="genvtx::combined_vg"]

#@config Generate vertexes on all deployed Bi207 calibration source tracks with realistic weights;variant=if_source_calibration_sds_vg

#@description Logging priority
logging.priority  : string = "fatal"

#@description Geometry version requirement (not working with version lower than 5.0)
geometry.setup_requirement : string = "snemo::demonstrator(>=5)"

#@description Vertex generator names
generators : string[6]  = \\
''')
        track_count=0
        for track_id in range(6) :
            track_count = track_count + 1
            calibration_all_tracks_vg_def_file.write("    \"sds_bi207_calibration_source_tracks_%d\" " % (track_id))
            if track_count < 6 :
                calibration_all_tracks_vg_def_file.write("\\")
            calibration_all_tracks_vg_def_file.write("\n")

        calibration_all_tracks_vg_def_file.write("\n#@description Vertex generator weights\n")
        for track_id in range(6) :
            track_vg_name = "sds_bi207_calibration_source_tracks_%d" % (track_id)
            track_act = track_activity_map[track_id]
            print("[debug] track #%d activity = %f" % (track_id, track_act))
            calibration_all_tracks_vg_def_file.write("generators.%s.absolute_weight : real = %.2f # Bq \n"
                                                     % (track_vg_name, track_act))                            
  
        calibration_all_tracks_vg_def_file.write("\n# end")
        calibration_all_tracks_vg_def_file.close()
       
        return
            
if __name__ == "__main__" :

    VgConfGen = bi207_calibration_source_vg_generator();
    VgConfGen.generate_bulk_vg_def()
    print("The end")
    sys.exit(0)

# end

