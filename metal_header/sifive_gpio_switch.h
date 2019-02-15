/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_GPIO_SWITCH__H
#define __METAL_HEADER_SIFIVE_GPIO_SWITCH__H

#include "metal_header/device.h"

#include <regex>

class sifive_gpio_switch : public Device {
  public:
	
    int num_switches;

    sifive_gpio_switch(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,gpio-switches")
    {
      /* Count the number of SWITCHs */
      num_switches = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  num_switches += 1;
	});
    }

    void create_defines()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	});
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
	  emit_const_struct_decl("sifive_gpio_switch", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_const_struct_begin("sifive_gpio_switch", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_switch");
	  emit_struct_field("flip.vtable", "&__metal_driver_vtable_sifive_switch.switch_vtable");

	  n.maybe_tuple(
	    "gpios", tuple_t<node, uint32_t>(),
	    [&](){
		emit_struct_field_null("gpio");
		emit_struct_field("pin", "0");
	    },
	    [&](node n, uint32_t line) {
		emit_struct_field_node("gpio", n, "");
		emit_struct_field_u32("pin", line);
	    });

	  n.maybe_tuple(
	    "interrupts-extended", tuple_t<node, uint32_t>(),
	    [&](){
		emit_struct_field_null("interrupt_parent");
		emit_struct_field("interrupt_line", "0");
	    },
	    [&](node n, uint32_t line) {
		emit_struct_field_node("interrupt_parent", n, ".irc");
		emit_struct_field_u32("interrupt_line", line);
	    });

	  emit_struct_field("label", "\"" + n.get_field<std::string>("label") + "\"");

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      emit_def("__METAL_DT_MAX_SWITCHES", std::to_string(num_switches));

      emit_const_struct_pointer_begin("sifive_gpio_switch", "__metal_switch_table", "[]");
      if (num_switches) {
	for (int i=0; i < num_switches; i++) {
	  emit_struct_pointer_element("switch", i, "",
	      ((i + 1) == num_switches) ? "};\n\n" : ",\n");
	}
      } else {
	emit_struct_pointer_end("NULL");
      }
    }
};

#endif
