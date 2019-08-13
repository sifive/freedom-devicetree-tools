/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_CPU_INTC__H
#define __METAL_HEADER_RISCV_CPU_INTC__H

#include <device.h>

#include <regex>

using std::string;

class riscv_cpu_intc : public Device {
public:
  riscv_cpu_intc(std::ostream &os, const fdt &dtb);
  string handle(node n);
  void declare_structs();
  void define_structs();
};

#endif /* __METAL_HEADER_RISCV_CPU_INTC__H */
