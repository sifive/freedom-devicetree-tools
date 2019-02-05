/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_FIXED_CLOCK__H
#define __METAL_HEADER_FIXED_CLOCK__H

#include "metal_header/device.h"

#include <regex>

class fixed_clock : public Device {
  public:
    fixed_clock(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "fixed-clock")
    {}

    void create_machine_macros() {}

    void create_defines()
    {
    }

    void include_headers()
    {
      emit_include(compat_string);
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("fixed_clock", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("fixed_clock", n);
	  emit_struct_field("vtable", "&__metal_driver_vtable_fixed_clock");
	  emit_struct_field("clock.vtable", "&__metal_driver_vtable_fixed_clock.clock");
	  emit_struct_field_u32("rate", n.get_field<uint32_t>("clock-frequency"));
	  emit_struct_end();
	});
    }

    void create_handles()
    {
    }
};

#endif
