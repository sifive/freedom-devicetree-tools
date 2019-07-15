/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_TRACE__H
#define __BARE_HEADER_SIFIVE_TRACE__H

#include "bare_header/device.h"

#include <regex>

class sifive_trace : public Device {
  public:
    sifive_trace(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,trace")
    {}

    void emit_defines() {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_comment(n);

	  emit_base(n);
	  emit_size(n);

	  /* Add more properties here */

	  os << std::endl;
	});
    }

    void emit_offsets() {
      if(dtb.match(std::regex(compat_string), [](const node n){}) != 0) {
	emit_compat();

	/* Add offsets here */

	os << std::endl;
      }
    }
};

#endif
