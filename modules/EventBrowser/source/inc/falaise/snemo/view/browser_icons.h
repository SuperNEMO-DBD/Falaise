// -*- mode: c++ ; -*-
/* browser_icons.h
 * Author (s) :   Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2012-03-25
 * Last modified: 2012-03-25
 *
 * Copyright (C) 2012 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *   Browser icons in XPM file format
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_BROWSER_ICONS_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_BROWSER_ICONS_H 1

#include <string>

/* XPM */
const char* xpm_vertex[] = {
    "16 16 2 1",        ". c None",         "# c #303030",      "................",
    "................", "................", ".......#........", ".......#........",
    ".......#........", ".......#........", "...#########....", ".......#........",
    ".......#........", ".......#........", ".......#........", "................",
    "................", "................", "................"};

const char* xpm_calorimeter[] = {
    "16 16 2 1",        ". c None",         "# c #303030",      "................",
    "................", "......########..", "......#......#..", "......#......#..",
    "......#......#..", "..########...#..", "..#......#...#..", "..#......#...#..",
    "..#......#...#..", "..#......#####..", "..#......#......", "..#......#......",
    "..########......", "................", "................"};

const char* xpm_geiger[] = {
    "16 16 2 1",        ". c None",         "# c #303030",      "...........###..",
    "...###....#...#.", "..#...#..#.....#", "..#.#.#..#..#..#", "..#...#..#.....#",
    "...###....#...#.", "...........###..", "...###..........", ".##...##........",
    ".#.....#........", "#.......#.......", "#...#...#.......", "#.......#.......",
    ".#.....#........", ".##...##........", "...###.........."};

const char* xpm_cluster[] = {
    "16 16 2 1",        ". c None",         "# c #303030",      "............####",
    "............#..#", "............#..#", "............####", "........####....",
    "........#..#....", "........#..#....", "........####....", "....####........",
    "....#..#........", "....#..#........", "....####........", "####............",
    "#..#............", "#..#............", "####............"};

const char* xpm_helix[] = {
    "16 16 2 1",        ". c None",         "# c #303030",      "................",
    "............##..", "...........#....", "..........#.....", "..........#.....",
    "........#####...", ".......#..#..#..", "......#...#..#..", "......#....###..",
    "....#####.......", "...#..#..#......", "..#...#..#......", "..#....###......",
    "..#.............", "................", "................"};

const char* xpm_line[] = {
    "16 16 2 1",        ". c None",         "# c #303030",      "................",
    "................", "................", ".............#..", "............#...",
    "...........#....", "..........#.....", ".........#......", "........#.......",
    ".......#........", "......#.........", ".....#..........", "....#...........",
    "...#............", "................", "................"};

const char* xpm_step[] = {
    "16 16 2 1",        ". c None",         "# c #303030",      "................",
    "................", "................", ".............#..", "............#...",
    "...........#....", "........#.#.....", "........#.#.....", ".....#######....",
    ".......##.......", "......#.#.......", ".....#..........", "....#...........",
    "...#............", "................", "................"};

const char* xpm_track[] = {
    "16 16 2 1",        ". c None",         "# c #303030",      "................",
    "................", "................", "................", ".....#######....",
    "...##......##...", "..##............", "..#.............", "..#.............",
    "..##............", "...###..........", ".....##.........", "................",
    "................", "................", "................"};

const char* xpm_flag[] = {
    "16 16 2 1",        ". c None",         "# c #303030",      "................",
    "................", ".....#..........", ".....###........", ".....#####......",
    ".....#######....", ".....######.....", ".....#####......", ".....###........",
    ".....#..........", ".....#..........", ".....#..........", ".....#..........",
    ".....#..........", "................", "................"};

const char* xpm_folder[] = {
    "16 16 3 1",        "  c None s None",  ". c #cfcfcf",      "o c #cfcfcf",
    "                ", "   .....        ", "  .ooooo.       ", " .ooooooo...... ",
    " .oooooooooooo. ", " .oooooooooooo. ", " .oooooooooooo. ", " .oooooooooooo. ",
    " .oooooooooooo. ", " .oooooooooooo. ", " .oooooooooooo. ", " .oooooooooooo. ",
    " .............. ", "                ", "                ", "                "};

const char* xpm_ofolder[] = {
    "16 16 3 1",        "  c None s None",  ". c #cfcfcf",      "o c #cfcfcf",
    "                ", "   .....        ", "  .ooooo.       ", " .ooooooo...... ",
    " .oooooooooooo. ", " .oooooooooooo. ", ".           .o. ", ".oooooooooo .o. ",
    ".ooooooooooo .. ", " .oooooooooo .. ", " .ooooooooooo . ", "  .oooooooooo . ",
    "  ............. ", "   ............ ", "                ", "                "};

#endif
