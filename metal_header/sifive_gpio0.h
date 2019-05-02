/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_GPIO0__H
#define __METAL_HEADER_SIFIVE_GPIO0__H

#include "metal_header/device.h"

#include <regex>

class sifive_gpio0 : public Device {
  public:
    int num_gpios = 0;

    sifive_gpio0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,gpio0")
    {
      /* Count the number of GPIOs */
      num_gpios = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  num_gpios += 1;
	});
    }

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
	  emit_struct_field("gpio.vtable", "&__metal_driver_vtable_sifive_gpio0.gpio");

	  emit_struct_field_platform_define("base", n, METAL_BASE_ADDRESS_LABEL);
	  emit_struct_field_platform_define("size", n, METAL_SIZE_LABEL);

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

    void create_handles()
    {
      emit_def("__MEE_DT_MAX_GPIOS", std::to_string(num_gpios));

      emit_struct_pointer_begin("sifive_gpio0", "__metal_gpio_table", "[]");
      if (num_gpios) {
	int i = 0;
	dtb.match(
	  std::regex(compat_string),
	  [&](node n) {
	    os << "\t\t\t\t\t&__metal_dt_" << n.handle();

	    if ((i + 1) == num_gpios) {
	      os << "};\n\n";
	    } else {
	      os << ",\n";
	    }

	    i += 1;
	  });
      } else {
	emit_struct_pointer_end("NULL");
}
    }
};

#endif
