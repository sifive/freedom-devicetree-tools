/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_TEST0__H
#define __METAL_HEADER_SIFIVE_TEST0__H

#include "metal_header/device.h"

#include <regex>

class sifive_test0 : public Device {
  public:
    sifive_test0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,test0")
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
	  emit_struct_decl("sifive_test0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_test0", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_test0");
	  emit_struct_field("shutdown.vtable", "&__metal_driver_vtable_sifive_test0.shutdown");

	  n.named_tuples(
	    "reg-names", "reg",
	    "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
	      emit_struct_field_ta("base", base);
	      emit_struct_field_ts("size", size);
	    });

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_def_handle("__METAL_DT_SHUTDOWN_HANDLE", n, ".shutdown");
	});
    }
};

#endif
