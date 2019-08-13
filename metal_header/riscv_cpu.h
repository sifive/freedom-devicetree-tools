/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_CPU__H
#define __METAL_HEADER_RISCV_CPU__H

#include <device.h>

#include <regex>

class riscv_cpu : public Device {
public:
  int num_cpus;

  riscv_cpu(std::ostream &os, const fdt &dtb);
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif /* __METAL_HEADER_RISCV_CPU__H */
