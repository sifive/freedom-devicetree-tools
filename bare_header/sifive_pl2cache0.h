/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_PL2CACHE0__H
#define __BARE_HEADER_SIFIVE_PL2CACHE0__H

#include "bare_header/device.h"

#include <regex>

class sifive_pl2cache0 : public Device {
public:
  sifive_pl2cache0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,pL2Cache0") {}

  int get_index(const node &n) { return Device::get_index(n, compat_string); }

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base("sifive,pl2cache0", n);
      emit_size("sifive,pl2cache0", n);

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {

      emit_compat("sifive,pl2cache0");
      emit_offset("sifive,pl2cache0", METAL_SIFIVE_PL2CACHE_CONFIG_LABEL,
                  0x0000);
      emit_offset("sifive,pl2cache0",
                  METAL_SIFIVE_PL2CACHE_ECCINJECTERROR_LABEL, 0X0040);
      emit_offset("sifive,pl2cache0",
                  METAL_SIFIVE_PL2CACHE_ECCINJECTERRORDONE_LABEL, 0X0060);
      emit_offset("sifive,pl2cache0", METAL_SIFIVE_PL2CACHE_CFLUSH64_LABEL,
                  0X0200);
      emit_offset("sifive,pl2cache0", METAL_SIFIVE_PL2CACHE_FLUSHCOUNT_LABEL,
                  0X0208);
      emit_offset("sifive,pl2cache0", METAL_SIFIVE_PL2CACHE_CONFIGBITS_LABEL,
                  0x1008);
      emit_offset("sifive,pl2cache0",
                  METAL_SIFIVE_PL2CACHE_PMEVENTSELECT0_LABEL, 0X2000);
      emit_offset("sifive,pl2cache0",
                  METAL_SIFIVE_PL2CACHE_PMCLIENTFILTER_LABEL, 0X2800);
      emit_offset("sifive,pl2cache0",
                  METAL_SIFIVE_PL2CACHE_PMEVENTCOUNTER0_LABEL, 0X3000);

      os << std::endl;
    }
  }
};

#endif
