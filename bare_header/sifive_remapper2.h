/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_REMAPPER2__H
#define __BARE_HEADER_SIFIVE_REMAPPER2__H

#include "bare_header/device.h"

#include <regex>

class sifive_remapper2 : public Device {
public:
  sifive_remapper2(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,remapper2") {}

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

      emit_offset("CFG", 0x0);
      emit_offset("VALID_BASE", 0x4);
      emit_offset("FLUSH", 0x20);
      emit_offset("VERSION", 0x1f4);
      emit_offset("ENTRIES", 0x1f8);
      emit_offset("KEY", 0x1fc);
      emit_offset("FROM_BASE", 0x200);

      os << std::endl;
    }
  }
};

#endif /* __BARE_HEADER_SIFIVE_REMAPPER2__H */
