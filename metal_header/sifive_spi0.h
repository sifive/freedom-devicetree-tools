/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_SPI0__H
#define __METAL_HEADER_SIFIVE_SPI0__H

#include <device.h>

#include <regex>

class sifive_spi0 : public Device {
public:
  int num_spis;

  sifive_spi0(std::ostream &os, const fdt &dtb);
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
