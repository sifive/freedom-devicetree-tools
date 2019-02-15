/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_FE310_G000_HFROSC__H
#define __METAL_HEADER_SIFIVE_FE310_G000_HFROSC__H

#include "metal_header/device.h"

#include <regex>

class sifive_fe310_g000_hfrosc : public Device {
  public:
    sifive_fe310_g000_hfrosc(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fe310-g000,hfrosc")
    {}

    void include_headers()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_include(compat_string);
	});
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_const_struct_decl("sifive_fe310_g000_hfrosc", n);
	  os << "struct __metal_clock_data __metal_dt_" << n.handle() << "_data;" << std::endl << std::endl;
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_const_struct_begin("sifive_fe310_g000_hfrosc", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_fe310_g000_hfrosc");
	  emit_struct_field("clock.vtable", "&__metal_driver_vtable_sifive_fe310_g000_hfrosc.clock");
	  emit_struct_field("clock.data", "&__metal_dt_" + n.handle() + "_data");

	  emit_struct_field_node("ref", n.get_field<node>("clocks"), ".clock");

	  n.named_tuples(
	    "reg-names", "reg",
	    "config", tuple_t<node, target_size>(), [&](node base, target_size offset) {
	      emit_struct_field_node("config_base", base, "");
	      emit_struct_field_ts("config_offset", offset);
	  });

	  emit_struct_end();
	});
    }
};

#endif
