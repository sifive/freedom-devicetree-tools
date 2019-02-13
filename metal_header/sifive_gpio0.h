/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_GPIO0__H
#define __METAL_HEADER_SIFIVE_GPIO0__H

#include "metal_header/device.h"

#include <regex>

class sifive_gpio0 : public Device {
  public:
    sifive_gpio0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,gpio0")
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
 
      emit_def("METAL_MAX_GPIO_INTERRUPTS", std::to_string(max_interrupts));
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
	  emit_struct_decl("sifive_gpio0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_gpio0", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_gpio0");

	  n.named_tuples(
	    "reg-names", "reg",
	    "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
	      emit_struct_field_ta("base", base);
	      emit_struct_field_ts("size", size);
	    });

	  n.maybe_tuple(
	    "interrupt-parent", tuple_t<node>(),
	    [&](){ emit_struct_field_null("interrupt_parent"); },
	    [&](node n) { emit_struct_field_node("interrupt_parent", n, ".controller"); });

	  emit_struct_field("num_interrupts", "METAL_MAX_GPIO_INTERRUPTS");

	  n.maybe_tuple_index(
	    "interrupts", tuple_t<uint32_t>(),
	    [&](){ emit_struct_field("interrupt_lines[0]", "0"); },
    	    [&](int i, uint32_t irline){ emit_struct_field_array_elem(i, "interrupt_lines", irline); });

	  emit_struct_end();
	});
    }
};

#endif
