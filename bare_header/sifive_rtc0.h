/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_RTC0__H
#define __BARE_HEADER_SIFIVE_RTC0__H

#include "bare_header/device.h"

#include <regex>

class sifive_rtc0 : public Device {
public:
  sifive_rtc0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,(rtc|aon)0") {}

  void emit_defines() override {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base(n);
      emit_size(n);

      os << std::endl;
    });
  }

  void emit_offsets() override {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat("sifive,rtc0");

      emit_offset("sifive,rtc0", "RTCCFG", 0x40);
      emit_offset("sifive,rtc0", "RTCCOUNTLO", 0x48);
      emit_offset("sifive,rtc0", "RTCCOUNTHI", 0x4C);
      emit_offset("sifive,rtc0", "RTCS", 0x50);
      emit_offset("sifive,rtc0", "RTCCMP0", 0x60);

      os << std::endl;
    }
  }
};

#endif
