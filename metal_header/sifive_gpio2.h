/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_GPIO2__H
#define __METAL_HEADER_SIFIVE_GPIO2__H

#include <device.h>

#include <regex>

class sifive_gpio2 : public Device {
public:
  int num_gpios = 0;
  uint32_t max_interrupts = 0;

  sifive_gpio2(std::ostream &os, const fdt &dtb);
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
