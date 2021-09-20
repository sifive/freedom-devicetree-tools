/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_WDOG2__H
#define __BARE_HEADER_SIFIVE_WDOG2__H

#include "bare_header/device.h"

#include <regex>
#include <set>

class sifive_wdog2 : public Device {
public:
  sifive_wdog2(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,(wdog|wdt|aon)2") {}

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
      emit_compat("sifive,wdog2");

      emit_offset("sifive,wdog2", "MAGIC_KEY", 0x51f15e);
      emit_offset("sifive,wdog2", "MAGIC_FOOD", 0xd09F00d);

      emit_offset("sifive,wdog2", "WDOGCFG", 0x0);
      emit_offset("sifive,wdog2", "WDOGCOUNTLO", 0x8);
      emit_offset("sifive,wdog2", "WDOGCOUNTHI", 0xC);
      emit_offset("sifive,wdog2", "WDOGS", 0x10);
      emit_offset("sifive,wdog2", "WDOGFEED", 0x18);
      emit_offset("sifive,wdog2", "WDOGKEY", 0x1c);
      emit_offset("sifive,wdog2", "WDOGCMP", 0x20);
      emit_offset("sifive,wdog2", "WDOGWIN", 0x24);
      emit_offset("sifive,wdog2", "WDOGINTEN", 0x20);
      emit_offset("sifive,wdog2", "WDOGINTPE", 0x20);

      os << std::endl;
    }
  }
};

#endif
