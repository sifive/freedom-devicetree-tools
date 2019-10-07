/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_UCB_HTIF0__H
#define __METAL_HEADER_UCB_HTIF0__H

#include "metal_header/device.h"

#include <regex>

class ucb_htif0 : public Device {
public:
  ucb_htif0(std::ostream &os, const fdt &dtb);
  void include_headers();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
