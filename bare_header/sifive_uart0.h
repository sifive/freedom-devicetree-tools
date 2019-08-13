/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_UART0__H
#define __BARE_HEADER_SIFIVE_UART0__H

#include "bare_header/device.h"

#include <regex>

class sifive_uart0 : public Device {
public:
  sifive_uart0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,uart0") {}

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base(n);
      emit_size(n);

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat();
      emit_offset("TXDATA", 0x0);
      emit_offset("RXDATA", 0x4);
      emit_offset("TXCTRL", 0x8);
      emit_offset("RXCTRL", 0xC);
      emit_offset("IE", 0x10);
      emit_offset("IP", 0x14);
      emit_offset("DIV", 0x18);

      os << std::endl;
    }
  }
};

#endif
