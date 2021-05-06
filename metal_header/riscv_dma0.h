/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_DMA0__H
#define __METAL_HEADER_RISCV_DMA0__H

#include <device.h>

#include <regex>

class riscv_dma0 : public Device {
public:
  uint32_t num_dmas = 0;
  uint32_t max_interrupts = 0;

  riscv_dma0(std::ostream &os, const fdt &dtb);
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
