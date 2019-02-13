/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_UART0__H
#define __METAL_HEADER_SIFIVE_UART0__H

#include "metal_header/device.h"

#include <regex>

class sifive_uart0 : public Device {
  public:
    sifive_uart0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,uart0")
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
 
      emit_def("METAL_MAX_UART_INTERRUPTS", std::to_string(max_interrupts));
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
	  emit_struct_decl("sifive_uart0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_uart0", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_uart0");
	  emit_struct_field("uart.vtable", "&__metal_driver_vtable_sifive_uart0.uart");

	  n.named_tuples(
	    "reg-names", "reg",
	    "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
	      emit_struct_field_ta("control_base", base);
	      emit_struct_field_ts("control_size", size);
	    });

	  n.maybe_tuple(
	    "clocks", tuple_t<node>(),
	    [&](){ emit_struct_field_null("clock"); },
	    [&](node n) { emit_struct_field_node("clock", n, ".clock"); });

	  n.maybe_tuple(
	    "pinmux", tuple_t<node, uint32_t, uint32_t>(),
	    [&](){ emit_struct_field_null("pinmux"); },
	    [&](node n, uint32_t dest, uint32_t source) {
	      emit_struct_field_node("pinmux", n, "");
	      emit_struct_field_u32("pinmux_output_selector", dest);
	      emit_struct_field_u32("pinmux_source_selector", source);
	    });

	  n.maybe_tuple(
	    "interrupt-parent", tuple_t<node>(),
	    [&](){ emit_struct_field_null("interrupt_parent"); },
	    [&](node n) { emit_struct_field_node("interrupt_parent", n, ".controller"); });

	  emit_struct_field("num_interrupts", "METAL_MAX_UART_INTERRUPTS");

	  n.maybe_tuple(
	    "interrupts", tuple_t<uint32_t>(),
	    [&](){ },
	    [&](uint32_t line){ emit_struct_field_u32("interrupt_line", line); });

	  emit_struct_end();
	});
    }
};

#endif
