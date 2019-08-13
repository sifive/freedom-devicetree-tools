/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/* This is a device template. Copy and extend it to add more devices. */

#ifndef __BARE_HEADER_SIFIVE_FE310_G000_PLL__H
#define __BARE_HEADER_SIFIVE_FE310_G000_PLL__H

#include "bare_header/device.h"

#include <regex>

class sifive_fe310_g000_pll : public Device {
public:
  sifive_fe310_g000_pll(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fe310-g000,pll") {}

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_property_u32(n, METAL_CLOCK_FREQUENCY_LABEL,
                        n.get_field<uint32_t>("clock-frequency"));

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
