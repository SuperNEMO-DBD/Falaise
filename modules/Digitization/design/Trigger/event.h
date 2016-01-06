// -*- mode: c++; -*-
#ifndef SNEMO_TRIGGER_EVENT_H
#define SNEMO_TRIGGER_EVENT_H

#include <random>

#include "tracker.h"
#include "zone.h"

namespace snemo {
  namespace trigger {

    struct event
    {

      event()
      {
        reset();
        return;
      }

      template<class Generator>
      void randomize_track(Generator & g_)
      {
        std::uniform_int_distribution<int> track_side_distribution(0, (int) tracker::NSIDES - 1);
        int track_side = track_side_distribution(g_);
        track_side = 1;
        // std::uniform_int_distribution<int> start_row_distribution(0, (int) tracker::NROWS - 1);
        std::uniform_int_distribution<int> start_row_distribution(10, (int) 22);
        int start_row = start_row_distribution(g_);
        std::uniform_int_distribution<int> start_layer_distribution(0, (int) tracker::NLAYERS - 1);
        int start_layer = start_layer_distribution(g_);
        std::poisson_distribution<int> track_length_distribution(3.0);
        int track_length = 1 + track_length_distribution(g_);
        std::cerr << "event::randomize_track:  -> track side        = " << track_side << std::endl;
        std::cerr << "event::randomize_track:  -> track start row   = " << start_row << std::endl;
        std::cerr << "event::randomize_track:  -> track start layer = " << start_layer << std::endl;
        std::cerr << "event::randomize_track:  -> track length      = " << track_length << std::endl;
        std::uniform_real_distribution<double> ud01(0.0, 1.0);
        std::uniform_real_distribution<double> track_angle_distribution(0.0, 2 * M_PI);
        double track_angle = track_angle_distribution(g_);
        std::cerr << "event::randomize_track:  -> track angle       = " << track_angle * 180 / M_PI << " °" << std::endl;
        int ihit = 0;
        double x = 1.0 * start_row   - 0.5 + ud01(g_);
        double y = 1.0 * start_layer - 0.5 + ud01(g_);
        // std::cerr << "event::randomize_track:  x=" << x << std::endl;
        // std::cerr << "event::randomize_track:  y=" << y << std::endl;
        double step = 0.15;
        double dx = step * std::cos(track_angle);
        double dy = step * std::sin(track_angle);
        // int il = start_layer;
        // int ir = start_row
        double length = 0.0;
        while (length <= track_length) {
          // std::cerr << "event::randomize_track:  hit #" << ihit << std::endl;
          int irow = (int) (x + 0.5);
          int ilayer = (int) (y + 0.5);
          // std::cerr << "event::randomize_track:  irow=" << irow << std::endl;
          // std::cerr << "event::randomize_track:  ilayer=" << ilayer << std::endl;
          if (irow >= 0 && irow < tracker::NROWS
              && ilayer >= 0 && ilayer < tracker::NLAYERS) {
            x += dx;
            y += dy;
            // std::cerr << "event::randomize_track:  x=" << x << std::endl;
            // std::cerr << "event::randomize_track:  y=" << y << std::endl;
            length += step;
            cells[track_side][ilayer][irow] = true;
            ihit++;
          } else {
            break;
          }
        }
        return;
      }

      template<class Generator>
      void randomize_tracks(Generator & g_)
      {
        std::default_random_engine generator;
        std::poisson_distribution<int> distribution(0.1);
        int ntracks = 1 + distribution(generator);
        std::cerr << "event::randomize_tracks: ntracks=" << ntracks << std::endl;
        for (int itrack = 0; itrack < ntracks; itrack++) {
          std::cerr << "event::randomize_tracks: generate track= #" << itrack << std::endl;
          randomize_track(g_);
        }
        return;
      }

      void reset()
      {
        for (int iside = 0; iside < tracker::NSIDES; iside++) {
          for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
            for (int irow = 0; irow < tracker::NROWS; irow++) {
              cells[iside][ilayer][irow] = false;
            }
          }
        }
        return;
      }

      void print_cells(std::ostream & out_) const
      {
        out_ << "Cells: \n";
        for (int ilayer = tracker::NLAYERS - 1; ilayer >= 0; ilayer--) {
          for (int irow = 0; irow < tracker::NROWS; irow++) {
            out_ << (cells[0][ilayer][irow] ? 'o' : '.');
          }
          out_ << '\n';
        }
        for (int irow = 0; irow < tracker::NROWS; irow++) {
          out_ << '=';
        }
        out_ << '\n';
        for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
          for (int irow = 0; irow < tracker::NROWS; irow++) {
            out_ << (cells[1][ilayer][irow] ? 'o' : '.');
          }
          out_ << '\n';
        }
        out_ << '\n';
        return;
      }

      void build_szone(szone & szone_, int side_, int szone_id_)
      {
        // std::cerr << "event::build_szone: size=" << side_ << " szone=" << szone_id_ << '\n';
        szone_.side = side_;
        szone_.szone_id = szone_id_;

        int stop_row = szone::stop_row(szone_id_);
        int start_row = szone::start_row(szone_id_);  //std::max(0, stop_row - 8);
        // std::cerr << "event::build_szone: -> start_row=" << start_row << '\n';
        // std::cerr << "event::build_szone: -> stop_row=" << stop_row << '\n';
        for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
          for (int irow = start_row;
               irow <= stop_row;
               irow++) {
            szone_.cells[ilayer][irow - start_row] = cells[side_][ilayer][irow];
          }
        }
        szone_.compute_lr_proj();
        return;
      }

      void build_szones(mem1_type & mem1_, mem2_type & mem2_)
      {
        for (int iside = 0; iside < tracker::NSIDES; iside++) {
          for (int iszone = 0; iszone < tracker::NSZONES; iszone ++) {
            build_szone(szones[iside][iszone], iside, iszone);
            szones[iside][iszone].build_pattern(mem1_, mem2_);
          }
        }
        return;
      }

      void build_zones()
      {
        for (int iside = 0; iside < tracker::NSIDES; iside++) {
          for (int izone = 0; izone < tracker::NZONES; izone++) {
            zone & z = zones[iside][izone];
            for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
              for (int irow = zone::start_row(izone);
                   irow <= zone::stop_row(izone);
                   irow++) {
                z.cells[ilayer][irow - zone::start_row(izone)] = cells[iside][ilayer][irow];
              }
            }
          }
        }

        return;
      }

      void print_zones(std::ostream & out_) const
      {
        out_ << "Zones: \n";
        for (int ilayer = tracker::NLAYERS - 1; ilayer >= 0; ilayer--) {
          for (int izone = 0; izone < tracker::NZONES; izone++) {
            const zone & z =  zones[0][izone];
            for (int irow = 0; irow < zone::width(izone); irow++) {
              out_ << (z.cells[ilayer][irow] ? 'o' : '.');
            }
            out_ << ' ';
          }
          out_ << '\n';
        }
        for (int irow = 0; irow < tracker::NROWS + tracker::NZONES - 1; irow++) {
          out_ << '=';
        }
        out_ << '\n';
        for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
          for (int izone = 0; izone < tracker::NZONES; izone++) {
            const zone & z =  zones[1][izone];
            for (int irow = 0; irow < zone::width(izone); irow++) {
              out_ << (z.cells[ilayer][irow] ? 'o' : '.');
            }
            out_ << ' ';
          }
          out_ << '\n';
        }
        out_ << '\n';

        return;
      }

      bool cells[tracker::NSIDES][tracker::NLAYERS][tracker::NROWS];
      zone zones[tracker::NSIDES][tracker::NZONES];
      szone szones[tracker::NSIDES][tracker::NSZONES];

    };

  }
}

#endif // SNEMO_TRIGGER_EVENT_H
