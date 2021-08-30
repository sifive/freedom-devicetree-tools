/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_UART1__H
#define __METAL_HEADER_SIFIVE_UART1__H

#include <device.h>

#include <regex>

class sifive_uart1 : public Device {
public:
  uint32_t num_uarts = 0;
  uint32_t max_interrupts = 0;

  sifive_uart1(std::ostream &os, const fdt &dtb);
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
