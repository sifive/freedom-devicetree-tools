/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_PLIC0__H
#define __METAL_HEADER_RISCV_PLIC0__H

#include <device.h>

#include <regex>

class riscv_plic0 : public Device {
public:
  int num_parents;
  uint32_t max_interrupts = 0;

  riscv_plic0(std::ostream &os, const fdt &dtb);
  void create_machine_macros();
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
