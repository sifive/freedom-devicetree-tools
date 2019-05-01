/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_RISCV_PMP__H
#define __BARE_HEADER_RISCV_PMP__H

#include "bare_header/device.h"

#include <regex>

class riscv_pmp : public Device {
  public:
    riscv_pmp(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "riscv,pmp")
    {}

    void emit_defines() {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_comment(n);

	  emit_property_u32(n, METAL_NUM_REGIONS_LABEL, n.get_field<uint32_t>("regions"));

	  os << std::endl;
	});
    }

    void emit_offsets() {
      if(dtb.match(std::regex(compat_string), [](const node n){}) != 0) {
	emit_compat();

	os << std::endl;
      }
    }
};

#endif
