/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_RTC1__H
#define __BARE_HEADER_SIFIVE_RTC1__H

#include "bare_header/device.h"

#include <regex>

class sifive_rtc1 : public Device {
public:
  sifive_rtc1(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,(rtc|aon)1") {}

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
      emit_compat("sifive,rtc1");

      emit_offset("sifive,rtc1", "RTCCOUNTLO", 0x00);
      emit_offset("sifive,rtc1", "RTCCOUNTHI", 0x04);
      emit_offset("sifive,rtc1", "RTCCFG", 0x08);
      emit_offset("sifive,rtc1", "RTCALLO", 0x0C);
      emit_offset("sifive,rtc1", "RTCALHI", 0x10);
      emit_offset("sifive,rtc1", "RTCALFEN", 0x14);
      emit_offset("sifive,rtc1", "RTCTOD", 0x18);
      emit_offset("sifive,rtc1", "RTCCAL", 0x1C);
      emit_offset("sifive,rtc1", "RTCPI", 0x20);
      emit_offset("sifive,rtc1", "RTCIE", 0x24);
      emit_offset("sifive,rtc1", "RTCIP", 0x28);
      emit_offset("sifive,rtc1", "RTCSTATUS", 0x2C);

      os << std::endl;
    }
  }
};

#endif
