/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_CCACHE0__H
#define __BARE_HEADER_SIFIVE_CCACHE0__H

#include "bare_header/device.h"

#include <regex>

class sifive_ccache0 : public Device {
public:
  sifive_ccache0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,ccache0") {}

  int get_index(const node &n) { return Device::get_index(n, compat_string); }

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
      emit_offset(METAL_SIFIVE_CCACHE_CONFIG_LABEL, 0x0000);
      emit_offset(METAL_SIFIVE_CCACHE_WAYENABLE_LABEL, 0x0008);
      emit_offset(METAL_SIFIVE_CCACHE_ECCINJECTERROR_LABEL, 0X0040);
      emit_offset(METAL_SIFIVE_CCACHE_DIRECCFIXLOW_LABEL, 0X0100);
      emit_offset(METAL_SIFIVE_CCACHE_DIRECCFIXHIGH_LABEL, 0X0104);
      emit_offset(METAL_SIFIVE_CCACHE_DIRECCFIXCOUNT_LABEL, 0X0108);
      emit_offset(METAL_SIFIVE_CCACHE_DIRECCFAILLOW_LABEL, 0X0120);
      emit_offset(METAL_SIFIVE_CCACHE_DIRECCFAILHIGH_LABEL, 0X0124);
      emit_offset(METAL_SIFIVE_CCACHE_DIRECCFAILCOUNT_LABEL, 0X0128);
      emit_offset(METAL_SIFIVE_CCACHE_DATECCFIXLOW_LABEL, 0X0140);
      emit_offset(METAL_SIFIVE_CCACHE_DATECCFIXHIGH_LABEL, 0X0144);
      emit_offset(METAL_SIFIVE_CCACHE_DATECCFIXCOUNT_LABEL, 0X0148);
      emit_offset(METAL_SIFIVE_CCACHE_DATECCFAILLOW_LABEL, 0X0160);
      emit_offset(METAL_SIFIVE_CCACHE_DATECCFAILHIGH_LABEL, 0X0164);
      emit_offset(METAL_SIFIVE_CCACHE_DATECCFAILCOUNT_LABEL, 0X0168);
      emit_offset(METAL_SIFIVE_CCACHE_FLUSH64_LABEL, 0X0200);
      emit_offset(METAL_SIFIVE_CCACHE_FLUSH32_LABEL, 0X0240);
      emit_offset(METAL_SIFIVE_CCACHE_WAYMASK0_LABEL, 0X0800);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTSELECT0_LABEL, 0X2000);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTSELECT1_LABEL, 0X2008);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTSELECT2_LABEL, 0X2010);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTSELECT3_LABEL, 0X2018);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTSELECT4_LABEL, 0X2020);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTSELECT5_LABEL, 0X2028);
      emit_offset(METAL_SIFIVE_CCACHE_PMCLIENTFILTER_LABEL, 0X2800);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTCOUNTER0_LABEL, 0X3000);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTCOUNTER1_LABEL, 0X3008);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTCOUNTER2_LABEL, 0X3010);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTCOUNTER3_LABEL, 0X3018);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTCOUNTER4_LABEL, 0X3020);
      emit_offset(METAL_SIFIVE_CCACHE_PMEVENTCOUNTER5_LABEL, 0X3028);
      os << std::endl;
    }
  }
};

#endif
