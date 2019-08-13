/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_CLIC0__H
#define __BARE_HEADER_SIFIVE_CLIC0__H

#include "bare_header/device.h"

#include <regex>

class sifive_clic0 : public Device {
public:
  sifive_clic0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,clic0") {}

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base(n);
      emit_size(n);

      emit_property_u32(n, METAL_SIFIVE_NUMINTS_LABEL,
                        n.get_field<uint32_t>("sifive,numints"));
      emit_property_u32(n, METAL_SIFIVE_NUMLEVELS_LABEL,
                        n.get_field<uint32_t>("sifive,numlevels"));
      emit_property_u32(n, METAL_SIFIVE_NUMINTBITS_LABEL,
                        n.get_field<uint32_t>("sifive,numintbits"));

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat();

      emit_offset("MSIP_BASE", 0x0);
      emit_offset("MTIMECMP_BASE", 0x4000);
      emit_offset("MTIME", 0xBFF8);

      emit_offset("CLICINTIP_BASE", 0x0);
      emit_offset("CLICINTIE_BASE", 0x400);
      emit_offset("CLICINTCTL_BASE", 0x800);
      emit_offset("CLICCFG", 0xC00);

      emit_offset("MMODE_APERTURE", 0x800000);
      emit_offset("HSMODE_APERTURE", 0xA00000);
      emit_offset("SMODE_APERTURE", 0xC00000);
      emit_offset("UMODE_APERTURE", 0xE00000);

      os << std::endl;
    }
  }
};

#endif
