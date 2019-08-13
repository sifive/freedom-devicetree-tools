/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_TEST0__H
#define __METAL_HEADER_SIFIVE_TEST0__H

#include <device.h>

#include <regex>

class sifive_test0 : public Device {
public:
  sifive_test0(std::ostream &os, const fdt &dtb);
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
