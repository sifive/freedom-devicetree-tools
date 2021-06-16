/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_REMAPPER2__H
#define __METAL_HEADER_SIFIVE_REMAPPER2__H

#include <device.h>

#include <regex>

class sifive_remapper2 : public Device {
public:
  sifive_remapper2(std::ostream &os, const fdt &dtb);
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif /* __METAL_HEADER_SIFIVE_REMAPPER2__H */
