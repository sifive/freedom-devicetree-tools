/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_UART1__H
#define __BARE_HEADER_SIFIVE_UART1__H

#include "bare_header/device.h"

#include <regex>

class sifive_uart1 : public Device {
public:
  sifive_uart1(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,uart1") {}

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
      emit_offset("AUTO_BAUD", 0x1C);
      emit_offset("WIRE_CFG", 0x20);
      emit_offset("DATA_SIZE", 0x24);
      emit_offset("LINCTRL", 0x28);
      emit_offset("UARTCTRL", 0x2C);
      emit_offset("STATUS", 0x30);

      os << std::endl;
    }
  }
};

#endif
