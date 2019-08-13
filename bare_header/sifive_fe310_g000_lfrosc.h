/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_FE310_G000_LFROSC__H
#define __BARE_HEADER_SIFIVE_FE310_G000_LFROSC__H

#include "bare_header/device.h"

#include <regex>

class sifive_fe310_g000_lfrosc : public Device {
public:
  sifive_fe310_g000_lfrosc(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fe310-g000,lfrosc") {}

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

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
