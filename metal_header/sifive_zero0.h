/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_ZERO0__H
#define __METAL_HEADER_SIFIVE_ZERO0__H

#include <device.h>

#include <regex>

class sifive_zero0 : public Device {
public:
  sifive_zero0(std::ostream &os, const fdt &dtb);
  void include_headers();
};

#endif
