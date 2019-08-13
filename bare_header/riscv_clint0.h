/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_RISCV_CLINT0__H
#define __BARE_HEADER_RISCV_CLINT0__H

#include "bare_header/device.h"

#include <regex>

class riscv_clint0 : public Device {
public:
  riscv_clint0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "riscv,clint0") {}

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
      emit_offset(METAL_MSIP_BASE_LABEL, 0x0);
      emit_offset(METAL_MTIMECMP_BASE_LABEL, 0x4000);
      emit_offset(METAL_MTIME_LABEL, 0xBFF8);

      os << std::endl;
    }
  }
};

#endif
