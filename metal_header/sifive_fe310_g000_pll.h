/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_FE310_G000_PLL__H
#define __METAL_HEADER_SIFIVE_FE310_G000_PLL__H

#include "metal_header/device.h"

#include <regex>

class sifive_fe310_g000_pll : public Device {
  public:
    sifive_fe310_g000_pll(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fe310-g000,pll")
    {}

    void create_defines()
    {
    }

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
	  emit_struct_decl("sifive_fe310_g000_pll", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_fe310_g000_pll", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_fe310_g000_pll");
	  emit_struct_field("clock.vtable", "&__metal_driver_vtable_sifive_fe310_g000_pll.clock");

	  n.named_tuples(
	    "clock-names", "clocks",
	    "pllref", tuple_t<node>(), [&](node base) {
	    emit_struct_field_node("pllref", base, ".clock");
	    },
	    "pllsel0", tuple_t<node>(), [&](node base) {
	      emit_struct_field_node("pllsel0", base, ".clock");
	    });

	  n.named_tuples(
	    "reg-names", "reg",
	    "config", tuple_t<node, target_size>(), [&](node base, target_size off) {
	      emit_struct_field_node("config_base", base, "");
	      emit_struct_field_ts("config_offset", off);
	    },
	    "divider", tuple_t<node, target_size>(), [&](node base, target_size off) {
	      emit_struct_field_node("divider_base", base, "");
	      emit_struct_field_ts("divider_offset", off);
	    });

	  emit_struct_field_u32("init_rate", n.get_field<uint32_t>("clock-frequency"));

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_def_handle("__METAL_DT_SIFIVE_FE310_G000_PLL_HANDLE", n, "");
	});
    }
};

#endif
