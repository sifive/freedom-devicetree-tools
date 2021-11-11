/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_PMC0__H
#define __METAL_HEADER_SIFIVE_PMC0__H

#include <device.h>

#include <regex>

class sifive_pmc0 : public Device {
public:
  sifive_pmc0(std::ostream &os, const fdt &dtb);
  void include_headers();
};

#endif
