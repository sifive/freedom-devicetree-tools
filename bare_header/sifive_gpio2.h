/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_GPIO2__H
#define __BARE_HEADER_SIFIVE_GPIO2__H

#include "bare_header/device.h"

#include <regex>

class sifive_gpio2 : public Device {
public:
  sifive_gpio2(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,gpio2") {}

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
      emit_offset("INPUT_EN", 0x10);
      emit_offset("OUTPUT_EN", 0x20);
      emit_offset("OUTPUT_VALUE", 0x30);
      emit_offset("RISE_IE", 0x40);
      emit_offset("RISE_IP", 0x50);
      emit_offset("FALL_IE", 0x60);
      emit_offset("FALL_IP", 0x70);
      emit_offset("HIGH_IE", 0x80);
      emit_offset("HIGH_IP", 0x90);
      emit_offset("LOW_IE", 0xA0);
      emit_offset("LOW_IP", 0xB0);
      emit_offset("IOF_EN", 0xC0);
      emit_offset("OUT_XOR", 0xD0);
      emit_offset("PASS_THRU_HIGH_IE", 0xE0);
      emit_offset("PASS_THRU_LOW_IE", 0xF0);
      emit_offset("INPUT_MASK", 0x100);
      emit_offset("OUTPUT_MASK", 0x110);

      os << std::endl;
    }
  }
};

#endif
