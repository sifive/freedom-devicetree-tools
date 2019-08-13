/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_FIXED_CLOCK__H
#define __METAL_HEADER_FIXED_CLOCK__H

#include <device.h>

#include <regex>

class fixed_clock : public Device {
public:
  int num_clocks;

  fixed_clock(std::ostream &os, const fdt &dtb);
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
};

#endif /* __METAL_HEADER_FIXED_CLOCK__H */
