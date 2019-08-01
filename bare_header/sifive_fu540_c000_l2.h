/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_FU540_C000_L2__H
#define __BARE_HEADER_SIFIVE_FU540_C000_L2__H

#include "bare_header/device.h"

#include <regex>

class sifive_fu540_c000_l2 : public Device {
  public:
    sifive_fu540_c000_l2(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fu540-c000,l2")
    {}

    int get_index(const node &n) {
      return Device::get_index(n, compat_string);
    }

    void emit_defines() {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_comment(n);

	  emit_base(n);
	  emit_size(n);

	  os << std::endl;
	});
    }

    void emit_offsets() {
      if(dtb.match(std::regex(compat_string), [](const node n){}) != 0) {
	emit_compat();
	emit_offset(METAL_SIFIVE_FU540_C000_L2_CONFIG_LABEL, 0x0);
	emit_offset(METAL_SIFIVE_FU540_C000_L2_WAYENABLE_LABEL, 0x8);

	os << std::endl;
      }
    }
};

#endif
