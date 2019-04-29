/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_FIXED_FACTOR_CLOCK__H
#define __METAL_HEADER_FIXED_FACTOR_CLOCK__H

#include "metal_header/device.h"

#include <regex>

class fixed_factor_clock : public Device {
  public:
    fixed_factor_clock(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "fixed-factor-clock")
    {}

    void include_headers()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_include(compat_string);
	}
      );
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("fixed_factor_clock", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("fixed_factor_clock", n);
	  emit_struct_field("vtable", "&__metal_driver_vtable_fixed_factor_clock");
	  emit_struct_field("clock.vtable", "&__metal_driver_vtable_fixed_factor_clock.clock");

	  n.maybe_tuple_index(
	    "clocks", tuple_t<node>(),
	    [&]() {
	      emit_struct_field_null("parent");
	    },
	    [&](int i, node c) {
	      emit_struct_field_node("parent", c, ".clock");
	    });

	  emit_struct_field_platform_define("mult", n, "CLOCK_MULT");
	  emit_struct_field_platform_define("div", n, "CLOCK_DIV");

	  emit_struct_end();
	});
    }
};

#endif
