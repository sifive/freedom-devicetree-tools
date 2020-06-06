/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_FLASH__H
#define __METAL_HEADER_SIFIVE_FLASH__H

#include <device.h>

#include <regex>

class sifive_flash : public Device {
public:
  int num_flash;

  sifive_flash(std::ostream &os, const fdt &dtb);

  void include_headers();
  void create_defines();
//  void declare_inlines();
//  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
