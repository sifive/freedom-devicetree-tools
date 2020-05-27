/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_PMP__H
#define __METAL_HEADER_RISCV_PMP__H

#include <device.h>

#include <regex>

class riscv_pmp : public Device {
public:
  int pmp_present;
  riscv_pmp(std::ostream &os, const fdt &dtb);
  void include_headers();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
