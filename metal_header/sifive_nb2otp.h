/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_NB2OTP__H
#define __METAL_HEADER_SIFIVE_NB2OTP__H

#include <device.h>

#include <regex>

class sifive_nb2otp : public Device {
public:
  int num_otp=0;
  uint32_t max_interrupts = 0;

  sifive_nb2otp(std::ostream &os, const fdt &dtb);

  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
