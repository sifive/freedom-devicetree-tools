/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_FIXED_FACTOR_CLOCK__H
#define __BARE_HEADER_FIXED_FACTOR_CLOCK__H

#include "bare_header/device.h"

#include <regex>

class fixed_factor_clock : public Device {
public:
  fixed_factor_clock(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "fixed-factor-clock") {}

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_property_u32(n, METAL_CLOCK_DIV_LABEL,
                        n.get_field<uint32_t>("clock-div"));
      emit_property_u32(n, METAL_CLOCK_MULT_LABEL,
                        n.get_field<uint32_t>("clock-mult"));

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat();

      os << std::endl;
    }
  }
};

#endif
