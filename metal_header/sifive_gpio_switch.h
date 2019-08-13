/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_GPIO_SWITCH__H
#define __METAL_HEADER_SIFIVE_GPIO_SWITCH__H

#include <device.h>

#include <regex>

class sifive_gpio_switch : public Device {
public:
  int num_switches;

  sifive_gpio_switch(std::ostream &os, const fdt &dtb);
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
