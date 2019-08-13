/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_AON0__H
#define __BARE_HEADER_SIFIVE_AON0__H

#include "bare_header/device.h"

#include <regex>

class sifive_aon0 : public Device {
public:
  sifive_aon0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,aon0") {}

  uint64_t base_address(const node &n) {
    uint64_t b;

    n.named_tuples("reg-names", "reg", "mem",
                   tuple_t<target_addr, target_size>(),
                   [&](target_addr base, target_size size) { b = base; });

    return b;
  }

  uint64_t size(const node &n) {
    uint64_t s;

    n.named_tuples("reg-names", "reg", "mem",
                   tuple_t<target_addr, target_size>(),
                   [&](target_addr base, target_size size) { s = size; });

    return s;
  }

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

      /* Watchdog Registers */
      emit_offset("WDOGCFG", 0x0);
      emit_offset("WDOGCOUNT", 0x8);
      emit_offset("WDOGS", 0x10);
      emit_offset("WDOGFEED", 0x18);
      emit_offset("WDOGKEY", 0x1C);
      emit_offset("WDOGCMP", 0x20);

      /* RTC Registers */
      emit_offset("RTCCFG", 0x40);
      emit_offset("RTCLO", 0x48);
      emit_offset("RTCHI", 0x48);
      emit_offset("RTCS", 0x50);
      emit_offset("RTCCMP", 0x60);

      /* LFROSC Registers */
      emit_offset("LFROSCCFG", 0x70);

      /* Backup Registers */
      emit_offset("BACKUP0", 0x80);
      emit_offset("BACKUP1", 0x84);
      emit_offset("BACKUP2", 0x88);
      emit_offset("BACKUP3", 0x8C);
      emit_offset("BACKUP4", 0x90);
      emit_offset("BACKUP5", 0x94);
      emit_offset("BACKUP6", 0x98);
      emit_offset("BACKUP7", 0x98);
      emit_offset("BACKUP8", 0xA0);
      emit_offset("BACKUP9", 0xA4);
      emit_offset("BACKUP10", 0xA8);
      emit_offset("BACKUP11", 0xAC);
      emit_offset("BACKUP12", 0xB0);
      emit_offset("BACKUP13", 0xB4);
      emit_offset("BACKUP14", 0xB8);
      emit_offset("BACKUP15", 0xBC);
      emit_offset("BACKUP16", 0xC0);
      emit_offset("BACKUP17", 0xC4);
      emit_offset("BACKUP18", 0xC8);
      emit_offset("BACKUP19", 0xCC);
      emit_offset("BACKUP20", 0xD0);
      emit_offset("BACKUP21", 0xD4);
      emit_offset("BACKUP22", 0xD8);
      emit_offset("BACKUP23", 0xDC);
      emit_offset("BACKUP24", 0xE0);
      emit_offset("BACKUP25", 0xE4);
      emit_offset("BACKUP26", 0xE8);
      emit_offset("BACKUP27", 0xEC);
      emit_offset("BACKUP28", 0xF0);
      emit_offset("BACKUP29", 0xF4);
      emit_offset("BACKUP30", 0xF8);
      emit_offset("BACKUP31", 0xFC);

      /* PMU Registers */
      emit_offset("PMU_WAKEUP_BASE", 0x100);
      emit_offset("PWM_SLEEP_BASE", 0x120);
      emit_offset("PMUIE", 0x140);
      emit_offset("PMUCAUSE", 0x144);
      emit_offset("PMUSLEEP", 0x148);
      emit_offset("PMUKEY", 0x14C);

      os << std::endl;
    }
  }
};

#endif
