/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_L2PF0__H
#define __BARE_HEADER_SIFIVE_L2PF0__H

#include "bare_header/device.h"

#include <regex>

class sifive_l2pf0 : public Device {
public:
  sifive_l2pf0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,l2pf0") {}

  int get_index(const node &n) { return Device::get_index(n, compat_string); }

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
      emit_offset("BASIC_CONTROL", 0x00);
      emit_offset("USER_CONTROL", 0x04);

      os << std::endl;
    }
  }
};

#endif
