/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_L2PF0__H
#define __METAL_HEADER_SIFIVE_L2PF0__H

#include <device.h>

#include <regex>

class sifive_l2pf0 : public Device {
public:
  sifive_l2pf0(std::ostream &os, const fdt &dtb);
  void include_headers();
  void create_defines();
};

#endif
