/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_RISCV_DMA0__H
#define __BARE_HEADER_RISCV_DMA0__H

#include "bare_header/device.h"

#include <regex>

class riscv_dma0 : public Device {
public:
  riscv_dma0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "riscv,dma0") {}

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
      emit_offset("CTRL", 0x000);
      emit_offset("NEXT_CONFIG", 0x004);
      emit_offset("NEXT_BYTES", 0x008);
      emit_offset("NEXT_DEST", 0x010);
      emit_offset("NEXT_SRC", 0x018);
      emit_offset("EXEC_CFG", 0x104);
      emit_offset("EXEC_BYTES", 0x108);
      emit_offset("EXEC_DEST", 0x110);
      emit_offset("EXEC_SRC", 0x118);
      os << std::endl;
    }
  }
};

#endif
