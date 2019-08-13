/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_TEST0__H
#define __BARE_HEADER_SIFIVE_TEST0__H

#include "bare_header/device.h"

#include <regex>

class sifive_test0 : public Device {
public:
  sifive_test0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,test0") {}

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

      emit_offset("FINISHER_OFFSET", 0x0);

      os << std::endl;
    }
  }
};

#endif
