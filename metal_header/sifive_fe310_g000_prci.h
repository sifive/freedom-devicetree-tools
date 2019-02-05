/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_FE310_G000_PRCI__H
#define __METAL_HEADER_SIFIVE_FE310_G000_PRCI__H

#include "metal_header/device.h"

#include <regex>

class sifive_fe310_g000_prci : public Device {
  public:
    sifive_fe310_g000_prci(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fe310-g000,prci")
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
	  emit_struct_decl("sifive_fe310_g000_prci", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_fe310_g000_prci", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_fe310_g000_prci");

	  n.named_tuples(
	    "reg-names", "reg",
	    "mem", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
	      emit_struct_field_ta("base", base);
	      emit_struct_field_ts("size", size);
	  });

	  emit_struct_end();
	});
    }

    void create_handles()
    {
    }
};

#endif
