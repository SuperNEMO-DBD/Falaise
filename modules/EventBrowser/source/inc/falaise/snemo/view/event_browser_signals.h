// -*- mode: c++ ; -*-
/* event_browser_signals.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-11
 * Last modified: 2012-12-19
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Description:
 *
 *   Definition of all signal entries
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_SIGNALS_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_SIGNALS_H 1

// list of signals
enum button_signals_type
  {
    //
    UNDEFINED,

    // File menu
    FILE_OPEN,
    FILE_RELOAD,
    CONNECT_TO_DAQ,
    SAVE_EVENT_TO_XML,
    SAVE_EVENT_AS,
    FILE_SAVE_TO_SNDST,
    LOAD_DETECTOR,
    LOAD_STYLE,
    FILE_CLEAR,
    FILE_QUIT,

    // Options
    SHOW_ALL_VERTICES,
    SHOW_ALL_MC_TRACKS,
    SHOW_ALL_MC_HITS,
    SHOW_ALL_MC_TRACKS_AND_HITS,
    SHOW_ALL_CALIBRATED_HITS,
    DUMP_EVENT,
    DUMP_INTO_TOOLTIP,
    DUMP_INTO_TERMINAL,
    DUMP_INTO_WINDOW,

    // MC menu
    SHOW_MC_VERTEX,
    SHOW_MC_TRACKS,
    SHOW_MC_HITS,
    SHOW_MC_CALORIMETER_HITS,
    SHOW_MC_TRACKER_HITS,
    SHOW_GG_CIRCLE,
    SHOW_GG_TIME_GRADIENT,

    // Calib. menu
    SHOW_CALIBRATED_HITS,
    SHOW_CALIBRATED_INFO,

    // Cluster & trajectory menu
    SHOW_TRACKER_CLUSTERED_HITS,
    SHOW_TRACKER_CLUSTERED_BOX,
    SHOW_TRACKER_CLUSTERED_CIRCLE,
    SHOW_TRACKER_TRAJECTORIES,
    SHOW_RECALIBRATED_TRACKER_HITS,

    // Particle track menu
    SHOW_PARTICLE_TRACKS,

    // Help menu
    ABOUT,
    DOCUMENTATION,

    // Selection
    UPDATE_SELECTION,
    RESET_SELECTION,
    SAVE_SELECTION,
    LOAD_SELECTION,
    COMBO_SELECTION,
    OR_SELECTION,
    AND_SELECTION,
    XOR_SELECTION,
    ENABLE_LOGIC_SELECTION,
    ENABLE_EH_SELECTION,
    ENABLE_SD_SELECTION,
    ENABLE_COMPLEX_SELECTION,

    // Signals for event navigation control
    STATUS_BAR,
    GOTO_EVENT,
    FIRST_EVENT,
    CURRENT_EVENT,
    NEXT_EVENT,
    PREVIOUS_EVENT,
    LAST_EVENT,

    // Signals for tab navigation control
    NEXT_TAB,
    PREVIOUS_TAB,

    // Area zoom list
    AREA_ZOOM_LIST,

    // View and print 3D canvas
    FOCUS_ROI,
    FULL_2D_VIEW,
    VIEW_X3D,
    VIEW_OGL,
    PRINT_3D,
    PRINT_2D,
    PRINT_3D_AS_EPS,
    PRINT_2D_AS_EPS,

    // Dumping and reloading style file
    RELOAD_STYLE_FILE,
    SAVE_STYLE_FILE,

    // Automatic reading
    AUTO_READ_ENABLE,
    AUTO_READ_DELAY,

    // Basic style
    COLOR_BACKGROUND,
    LINE_STYLE,
    LINE_WIDTH

  };

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_EVENT_BROWSER_SIGNALS_H

// end of event_browser_signals.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
