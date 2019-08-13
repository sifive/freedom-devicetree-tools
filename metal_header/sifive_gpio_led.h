//* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_GPIO_LED__H
#define __METAL_HEADER_SIFIVE_GPIO_LED__H

#include <device.h>

#include <regex>

class sifive_gpio_led : public Device {
public:
  int num_leds;

  sifive_gpio_led(std::ostream &os, const fdt &dtb);
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
