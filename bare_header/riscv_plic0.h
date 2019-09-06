/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/* This is a device template. Copy and extend it to add more devices. */

#ifndef __BARE_HEADER_RISCV_PLIC0__H
#define __BARE_HEADER_RISCV_PLIC0__H

#include "bare_header/device.h"

#include <regex>

class riscv_plic0 : public Device {
public:
  riscv_plic0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "riscv,plic0") {}

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base(n);
      emit_size(n);

      emit_property_u32(n, METAL_RISCV_MAX_PRIORITY_LABEL,
                        n.get_field<uint32_t>("riscv,max-priority"));
      emit_property_u32(n, METAL_RISCV_NDEV_LABEL,
                        n.get_field<uint32_t>("riscv,ndev") + 1);

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat();
      emit_offset("PRIORITY_BASE", 0x0);
      emit_offset("PENDING_BASE", 0x1000);
      emit_offset("ENABLE_BASE", 0x2000);
      emit_offset("ENABLE_PER_HART", 0x80);
      emit_offset("CONTEXT_BASE", 0x200000);
      emit_offset("CONTEXT_PER_HART", 0x1000);
      emit_offset("CONTEXT_THRESHOLD", 0x00);
      emit_offset("CONTEXT_CLAIM", 0x04);

      os << std::endl;
    }
  }
};

#endif
