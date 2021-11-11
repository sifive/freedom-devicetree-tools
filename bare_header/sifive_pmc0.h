/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_PMC0__H
#define __BARE_HEADER_SIFIVE_PMC0__H

#include "bare_header/device.h"

#include <regex>

class sifive_pmc0 : public Device {
public:
  sifive_pmc0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,pmc0") {}

  int get_index(const node &n) { return Device::get_index(n, compat_string); }

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base("sifive,pmc0", n);
      emit_size("sifive,pmc0", n);

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {

      emit_compat("sifive,pmc0");
      emit_offset("sifive,pmc0", METAL_SIFIVE_PMC_SYSPORT_BB_LABEL, 0x0000);
      emit_offset("sifive,pmc0", METAL_SIFIVE_PMC_PWRGATE_LABEL, 0x0008);
      emit_offset("sifive,pmc0", METAL_SIFIVE_PMC_DEBUG_LABEL, 0X000c);
      emit_offset("sifive,pmc0", METAL_SIFIVE_PMC_TIMER_LABEL, 0X0010);

      os << std::endl;
    }
  }
};

#endif
