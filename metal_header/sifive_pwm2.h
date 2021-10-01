/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_PWM2__H
#define __METAL_HEADER_SIFIVE_PWM2__H

#include <device.h>
#include <regex>

class sifive_pwm2 : public Device {
public:
  int num_pwms;
  uint32_t max_interrupts = 0;
  uint32_t max_ncmp = 0;

  sifive_pwm2(std::ostream &os, const fdt &dtb);
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
