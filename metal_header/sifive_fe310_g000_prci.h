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

	  emit_struct_field_platform_define("base", n, "BASE_ADDRESS");
	  emit_struct_field_platform_define("size", n, "SIZE");

	  emit_struct_end();
	});
    }
};

#endif
