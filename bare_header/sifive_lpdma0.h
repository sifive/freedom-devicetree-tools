/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_LPDMA0__H
#define __BARE_HEADER_SIFIVE_LPDMA0__H

#include "bare_header/device.h"

#include <regex>

class sifive_lpdma0 : public Device {
  public:
    sifive_lpdma0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,lpdma0")
    {}

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

	emit_offset("CONFIG", 0x0);
	emit_offset("CONTROL", 0x4);
	emit_offset("STATUS", 0x8);

	emit_offset("JOBQ_PTR", 0xC);
	emit_offset("EXEC", 0x10);

	os << std::endl;
      }
    }
};

#endif
