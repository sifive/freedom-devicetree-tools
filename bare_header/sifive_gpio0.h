/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_GPIO0__H
#define __BARE_HEADER_SIFIVE_GPIO0__H

#include "bare_header/device.h"

#include <regex>

class sifive_gpio0 : public Device {
public:
  sifive_gpio0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,gpio0") {}

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
      emit_offset("VALUE", 0x0);
      emit_offset("INPUT_EN", 0x4);
      emit_offset("OUTPUT_EN", 0x8);
      emit_offset("PORT", 0xC);
      emit_offset("PUE", 0x10);
      emit_offset("DS", 0x14);
      emit_offset("RISE_IE", 0x18);
      emit_offset("RISE_IP", 0x1C);
      emit_offset("FALL_IE", 0x20);
      emit_offset("FALL_IP", 0x24);
      emit_offset("HIGH_IE", 0x28);
      emit_offset("HIGH_IP", 0x2C);
      emit_offset("LOW_IE", 0x30);
      emit_offset("LOW_IP", 0x34);
      emit_offset("IOF_EN", 0x38);
      emit_offset("IOF_SEL", 0x3C);
      emit_offset("OUT_XOR", 0x40);

      os << std::endl;
    }
  }
};

#endif
