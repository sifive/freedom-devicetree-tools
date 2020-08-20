/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_CCACHE0__H
#define __BARE_HEADER_SIFIVE_CCACHE0__H

#include "bare_header/device.h"

#include <regex>

class sifive_ccache0 : public Device {
public:
  sifive_ccache0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,(ccache0|fu540-c000,l2)") {}

  int get_index(const node &n) { return Device::get_index(n, compat_string); }

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base("sifive,ccache0", n);
      emit_size("sifive,ccache0", n);

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {

      emit_compat("sifive,ccache0");
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_CONFIG_LABEL, 0x0000);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_WAYENABLE_LABEL,
                  0x0008);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_ECCINJECTERROR_LABEL,
                  0X0040);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DIRECCFIXLOW_LABEL,
                  0X0100);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DIRECCFIXHIGH_LABEL,
                  0X0104);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DIRECCFIXCOUNT_LABEL,
                  0X0108);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DIRECCFAILLOW_LABEL,
                  0X0120);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DIRECCFAILHIGH_LABEL,
                  0X0124);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DIRECCFAILCOUNT_LABEL,
                  0X0128);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DATECCFIXLOW_LABEL,
                  0X0140);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DATECCFIXHIGH_LABEL,
                  0X0144);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DATECCFIXCOUNT_LABEL,
                  0X0148);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DATECCFAILLOW_LABEL,
                  0X0160);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DATECCFAILHIGH_LABEL,
                  0X0164);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_DATECCFAILCOUNT_LABEL,
                  0X0168);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_FLUSH64_LABEL, 0X0200);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_FLUSH32_LABEL, 0X0240);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_WAYMASK0_LABEL, 0X0800);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_PMEVENTSELECT0_LABEL,
                  0X2000);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_PMCLIENTFILTER_LABEL,
                  0X2800);
      emit_offset("sifive,ccache0", METAL_SIFIVE_CCACHE_PMEVENTCOUNTER0_LABEL,
                  0X3000);

      os << std::endl;
    }
  }
};

#endif
