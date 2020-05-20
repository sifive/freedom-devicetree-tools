/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_SIMUART0__H
#define __BARE_HEADER_SIFIVE_SIMUART0__H

#include "bare_header/device.h"

#include <regex>

class sifive_simuart0 : public Device {
public:
  sifive_simuart0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,simuart0") {}

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
      emit_offset("TXCTRL", 0x8);
      emit_offset("DIV", 0x18);

      os << std::endl;
    }
  }
};

#endif
