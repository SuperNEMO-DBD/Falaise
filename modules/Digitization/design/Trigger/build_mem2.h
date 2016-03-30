// -*- mode: c++; -*-
#ifndef SNEMO_TRIGGER_BUILD_MEM2_H
#define SNEMO_TRIGGER_BUILD_MEM2_H

// Standard library:
#include <iostream>

// This project:
#include "memory.h"
#include "utils.h"

namespace snemo {
  namespace trigger {

    //! \brief Building of the sliding zone horizontal classification programmable memory
    struct mem2_builder
    {
      /// \brief Mode
      enum mode_type {
        MODE_INVALID = 0, //!< Invalid mode
        MODE_MULT    = 1, //!< Build memory programming from multiplicity criteria
        MODE_PATTERN = 2  //!< Build memory programming from pattern recognition
      };

      /// \brief Horizontal/row classification
      enum classification_type {
        NO_HTRACK           = 0x0, //!< No track detected           : 00
        NARROW_RIGHT_HTRACK = 0x1, //!< Right narrow track detected : 01
        NARROW_LEFT_HTRACK  = 0x2, //!< Left narrow track detected  : 10
        WIDE_HTRACK         = 0x3  //!< Wide track detected         : 11
      };

      /// Default constructor
      mem2_builder();

      /// Build
      void build(mem2_type & mem2_) const;

      /// Build
      void build_mult(mem2_type & mem2_) const;

      /// Build
      void build_pattern(mem2_type & mem2_) const;

      mode_type mode;
      std::size_t wide_row_min;
      std::size_t wide_row_max;
      std::size_t wide_mult_min;
      std::size_t wide_mult_max;
      std::size_t narrow_left_row_min;
      std::size_t narrow_left_row_max;
      std::size_t narrow_left_mult_min;
      std::size_t narrow_left_mult_max;
      std::size_t narrow_right_row_min;
      std::size_t narrow_right_row_max;
      std::size_t narrow_right_mult_min;
      std::size_t narrow_right_mult_max;

    };

    mem2_builder::mem2_builder()
    {
      mode = MODE_MULT;
      wide_row_min          = 0;
      wide_row_max          = 7;
      wide_mult_min         = 6;
      wide_mult_max         = 8;
      narrow_left_row_min   = 0;
      narrow_left_row_max   = 4;
      narrow_left_mult_min  = 3;
      narrow_left_mult_max  = 5;
      narrow_right_row_min  = 3;
      narrow_right_row_max  = 7;
      narrow_right_mult_min = 3;
      narrow_right_mult_max = 5;
      return;
    }

    void mem2_builder::build(mem2_type & mem2_) const
    {
      if (mode == MODE_MULT) {
        build_mult(mem2_);
      }
      if (mode == MODE_PATTERN) {
        build_pattern(mem2_);
      }
      mem2_.memory_map_display();
      return;
    }

    void mem2_builder::build_pattern(mem2_type & mem2_) const
    {
      for (unsigned long addr = 0; addr < mem2_.get_number_of_addresses(); addr++) {
        mem2_type::address_type address = addr;
        // Default NO_TRACK pattern:
        classification_type clsf = NO_HTRACK;
        std::size_t full_count = 0;
        for (int irow = 0; irow < 8; irow++) {
          if (address.test(irow)) {
            full_count++;
          }
        }
        // Search for patterns:
        std::string addr_str = address.to_string();
        std::string left_str = addr_str;
        std::reverse(left_str.begin(), left_str.end());
        std::string right_str = addr_str;

        // Left:
        std::size_t lpos0 = left_str.find("1111");
        std::size_t lpos1 = left_str.find("111");
        std::size_t lpos2 = left_str.find("1101");
        std::size_t lpos3 = left_str.find("1011");
        bool left = false;
        if (!left && lpos0 != left_str.npos) {
          if (lpos0 <= 2) {
            left = true;
          }
        }
        if (!left && lpos1 != left_str.npos) {
          if (lpos1 <= 2) {
            left = true;
          }
        }
        if (!left && lpos2 != left_str.npos) {
          if (lpos2 <= 2) {
            left = true;
          }
        }
        if (!left && lpos3 != left_str.npos) {
          if (lpos3 <= 2) {
            left = true;
          }
        }

        // Right:
        std::size_t rpos0 = right_str.find("1111");
        std::size_t rpos1 = right_str.find("111");
        std::size_t rpos2 = right_str.find("1101");
        std::size_t rpos3 = right_str.find("1011");
        bool right = false;
        if (!right && rpos0 != right_str.npos) {
          if (rpos0 <= 1) { // Trick to favor left classification
            right = true;
          }
        }
        if (!right && rpos1 != right_str.npos) {
          if (rpos1 <= 2) {
            right = true;
          }
        }
        if (!right && rpos2 != right_str.npos) {
          if (rpos2 <= 1) {
            right = true;
          }
        }
        if (!right && rpos3 != right_str.npos) {
          if (rpos3 <= 1) {
            right = true;
          }
        }

        char c12n2 = ' ';
        if (c12n2 == ' ' && full_count >= 6) c12n2 = 'W';
        if (c12n2 == ' ' && left && right) c12n2 = 'W';
        if (c12n2 == ' ' && left && ! right) c12n2 = 'L';
        if (c12n2 == ' ' && ! left && right) c12n2 = 'R';
        if (c12n2 == ' ') c12n2 = 'V';

        if (full_count < 6 && c12n2 == 'W') {
          if (right_str.substr(0,2) == "00" && left_str.substr(0,2) != "00") c12n2 = 'L';
          if (left_str.substr(0,2) == "00" && right_str.substr(0,2) != "00") c12n2 = 'R';
          if (left_str.substr(0,2) == "00" && right_str.substr(0,2) == "00") c12n2 = 'L';
        }

        if (c12n2 == 'V') {
          clsf = NO_HTRACK;
        } else if (c12n2 == 'R') {
          clsf = NARROW_RIGHT_HTRACK;
        } else if (c12n2 == 'L') {
          clsf = NARROW_LEFT_HTRACK;
        } else if (c12n2 == 'W') {
          clsf = WIDE_HTRACK;
        }
        mem2_type::data_type data = clsf;
        mem2_.push(address, data);
      }
      return;
    }

    void mem2_builder::build_mult(mem2_type & mem2_) const
    {
      for (unsigned long addr = 0; addr < mem2_.get_number_of_addresses(); addr++) {
        mem2_type::address_type address = addr;
        // std::cerr << "mem2_builder::build: addr=" << addr << " address=" << address << std::endl;
        // Default NO_TRACK pattern:
        classification_type clsf = NO_HTRACK;
        bool left = false;
        bool right = false;
        std::size_t full_count = 0;
        std::size_t left_count = 0;
        std::size_t right_count = 0;
        std::size_t anti_left_count = 0;
        std::size_t anti_right_count = 0;

        // Search for NARROW_RIGHT_HTRACK pattern:
        {
          std::size_t narrow_right_count = 0;
          for (std::size_t irow = narrow_right_row_min; irow <= narrow_right_row_max; irow++) {
            if (address.test(irow)) narrow_right_count++;
          }
          if (narrow_right_count >= narrow_right_mult_min && narrow_right_count <= narrow_right_mult_max) {
            clsf = NARROW_RIGHT_HTRACK;
          }
        }
        // Search for NARROW_LEFT_HTRACK pattern:
        {
          std::size_t narrow_left_count = 0;
          for (std::size_t irow = narrow_left_row_min; irow <= narrow_left_row_max; irow++) {
            if (address.test(irow)) narrow_left_count++;
          }
          if (narrow_left_count >= narrow_left_mult_min && narrow_left_count <= narrow_left_mult_max) {
            clsf = NARROW_LEFT_HTRACK;
          }
        }
        // Search for WIDE_HTRACK pattern:
        {
          std::size_t wide_count = 0;
          for (std::size_t irow = wide_row_min; irow <= wide_row_max; irow++) {
            if (address.test(irow)) wide_count++;
          }
          if (wide_count >= wide_mult_min && wide_count <= wide_mult_max) {
            clsf = WIDE_HTRACK;
          }
        }

        mem2_type::data_type data = clsf;
        mem2_.push(address, data);
      }
      mem2_.memory_map_display();

      return;
    }

  }
}

#endif // SNEMO_TRIGGER_ZONE_H
