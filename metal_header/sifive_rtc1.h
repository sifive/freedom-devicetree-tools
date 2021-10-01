/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_RTC1__H
#define __METAL_HEADER_SIFIVE_RTC1__H

#include <device.h>

#include <regex>

class sifive_rtc1 : public Device {
public:
  int num_rtcs;

  sifive_rtc1(std::ostream &os, const fdt &dtb);

  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
