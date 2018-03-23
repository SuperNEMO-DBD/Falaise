// -*- mode: c++; -*-
#ifndef SNEMO_TRIGGER_BUILD_MEM1_H
#define SNEMO_TRIGGER_BUILD_MEM1_H

// Standard library:
#include <iostream>

// This project:
#include "memory.h"
#include "utils.h"

namespace snemo {
namespace trigger {

//! \brief Building of the sliding zone vertical classification programmable memory
struct mem1_builder {
  enum classification_type {
    VVOID = 0x0,   // 00
    VINNER = 0x1,  // 01
    VOUTER = 0x2,  // 10
    VFULL = 0x3    // 11
  };

  mem1_builder();

  void build(mem1_type& mem1_) const;
  void build_mult2(mem1_type& mem1_) const;

  std::size_t vfull_layer_min;
  std::size_t vfull_layer_max;
  std::size_t vfull_mult_min;
  std::size_t vfull_mult_max;
  std::size_t vinner_layer_min;
  std::size_t vinner_layer_max;
  std::size_t vinner_mult_max;
  std::size_t vouter_layer_min;
  std::size_t vouter_layer_max;
  std::size_t vouter_mult_max;
};

mem1_builder::mem1_builder() {
  vfull_layer_min = 0;
  vfull_layer_max = 8;
  vfull_mult_min = 6;
  vfull_mult_max = 9;
  vinner_layer_min = 0;
  vinner_layer_max = 5;
  vinner_mult_max = 6;
  vouter_layer_min = 4;
  vouter_layer_max = 8;
  vouter_mult_max = 5;
  return;
}

void mem1_builder::build(mem1_type& mem1_) const {
  std::size_t vinner_mult_min = 3;
  std::size_t vouter_mult_min = 3;

  for (unsigned long addr = 0; addr < mem1_.get_number_of_addresses(); addr++) {
    mem1_type::address_type address = addr;
    // Default VVOID pattern:
    classification_type clsf = VVOID;
    // Search for VOUTER pattern:
    {
      std::size_t vouter_count = 0;
      for (std::size_t ilayer = vouter_layer_min; ilayer <= vouter_layer_max; ilayer++) {
        if (address.test(ilayer)) vouter_count++;
      }
      if (vouter_count >= vouter_mult_min && vouter_count <= vouter_mult_max) {
        clsf = VOUTER;
      }
    }
    // Search for VINNER pattern:
    {
      std::size_t vinner_count = 0;
      for (std::size_t ilayer = vinner_layer_min; ilayer <= vinner_layer_max; ilayer++) {
        if (address.test(ilayer)) vinner_count++;
      }
      if (vinner_count >= vinner_mult_min && vinner_count <= vinner_mult_max) {
        clsf = VINNER;
      }
    }
    // Search for VFULL pattern:
    {
      std::size_t vfull_count = 0;
      for (std::size_t ilayer = vfull_layer_min; ilayer <= vfull_layer_max; ilayer++) {
        if (address.test(ilayer)) vfull_count++;
      }
      if (vfull_count >= vfull_mult_min && vfull_count <= vfull_mult_max) {
        clsf = VFULL;
      }
    }
    mem1_type::data_type data = clsf;
    mem1_.push(address, data);
  }
  mem1_.memory_map_display();
  return;
}

void mem1_builder::build_mult2(mem1_type& mem1_) const {
  std::size_t vinner_mult_min = 2;
  std::size_t vouter_mult_min = 2;

  for (unsigned long addr = 0; addr < mem1_.get_number_of_addresses(); addr++) {
    mem1_type::address_type address = addr;
    // Default VVOID pattern:
    classification_type clsf = VVOID;
    // Search for VOUTER pattern:
    {
      std::size_t vouter_count = 0;
      for (std::size_t ilayer = vouter_layer_min; ilayer <= vouter_layer_max; ilayer++) {
        if (address.test(ilayer)) vouter_count++;
      }
      if (vouter_count >= vouter_mult_min && vouter_count <= vouter_mult_max) {
        clsf = VOUTER;
      }
    }
    // Search for VINNER pattern:
    {
      std::size_t vinner_count = 0;
      for (std::size_t ilayer = vinner_layer_min; ilayer <= vinner_layer_max; ilayer++) {
        if (address.test(ilayer)) vinner_count++;
      }
      if (vinner_count >= vinner_mult_min && vinner_count <= vinner_mult_max) {
        clsf = VINNER;
      }
    }
    // Search for VFULL pattern:
    {
      std::size_t vfull_count = 0;
      for (std::size_t ilayer = vfull_layer_min; ilayer <= vfull_layer_max; ilayer++) {
        if (address.test(ilayer)) vfull_count++;
      }
      if (vfull_count >= vfull_mult_min && vfull_count <= vfull_mult_max) {
        clsf = VFULL;
      }
    }
    mem1_type::data_type data = clsf;
    mem1_.push(address, data);
  }
  mem1_.memory_map_display();
}

}  // namespace trigger
}  // namespace snemo

#endif  // SNEMO_TRIGGER_BUILD_MEM1_H
