/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_WDOG0__H
#define __METAL_HEADER_SIFIVE_WDOG0__H

#include <device.h>

#include <regex>

class sifive_wdog0 : public Device {
public:
  int num_wdogs;

  sifive_wdog0(std::ostream &os, const fdt &dtb);

  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
