/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_TRACE__H
#define __METAL_HEADER_SIFIVE_TRACE__H

#include <device.h>

#include <regex>

class sifive_trace : public Device {
public:
  sifive_trace(std::ostream &os, const fdt &dtb);

  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
};

#endif
