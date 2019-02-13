/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_GLOBAL_EXTERNAL_INTERRUPTS0__H
#define __METAL_HEADER_SIFIVE_GLOBAL_EXTERNAL_INTERRUPTS0__H

#include "metal_header/device.h"

#include <regex>

class sifive_global_external_interrupts0 : public Device {
  public:
    sifive_global_external_interrupts0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,global-external-interrupts0")
    {}

    void create_defines()
    {
      uint32_t max_interrupts = 0;

      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  uint32_t num_interrupts = n.get_fields_count<uint32_t>("interrupts");

	  emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS", std::to_string(num_interrupts));

	  if(num_interrupts > max_interrupts) {
	    max_interrupts = num_interrupts;
	  }
	});
 
      emit_def("METAL_MAX_GLOBAL_EXT_INTERRUPTS", std::to_string(max_interrupts));
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
	  emit_struct_decl("sifive_global_external_interrupts0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_global_external_interrupts0", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_global_external_interrupts0");
	  emit_struct_field("irc.vtable", "&__metal_driver_vtable_sifive_global_external_interrupts0.global0_vtable");

	  emit_struct_field("init_done", "0");

	  n.maybe_tuple(
	    "interrupt-parent", tuple_t<node>(),
	    [&](){ emit_struct_field_null("interrupt_parent"); },
	    [&](node n) { emit_struct_field_node("interrupt_parent", n, ".controller"); });

	  emit_struct_field("num_interrupts", "METAL_MAX_GLOBAL_EXT_INTERRUPTS");

	  n.maybe_tuple_index(
	    "interrupts", tuple_t<uint32_t>(),
	    [&](){ emit_struct_field("interrupt_lines[0]", "0"); },
	    [&](int i, uint32_t irline){ emit_struct_field_array_elem(i, "interrupt_lines", irline); });

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
  	  emit_def_handle("__METAL_DT_SIFIVE_GLOBAL_EXINTR0_HANDLE", n, ".irc");
	});
    }
};

#endif
