/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_GPIO_LED__H
#define __METAL_HEADER_SIFIVE_GPIO_LED__H

#include "metal_header/device.h"

#include <regex>

class sifive_gpio_led : public Device {
  public:
	
    int num_leds;

    sifive_gpio_led(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,gpio-leds")
    {
      /* Count the number of LEDs */
      num_leds = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  num_leds += 1;
	});
    }

    void create_machine_macros() {}

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
	  emit_struct_decl("sifive_gpio_led", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_gpio_led", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_led");
	  emit_struct_field("led.vtable", "&__metal_driver_vtable_sifive_led.led_vtable");

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

	  emit_struct_field("label", "\"" + n.get_field<std::string>("label") + "\"");

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      emit_def("__METAL_DT_MAX_LEDS", std::to_string(num_leds));

      emit_struct_pointer_begin("sifive_gpio_led", "__metal_led_table", "[]");
      if (num_leds) {
	for (int i=0; i < num_leds; i++) {
	  emit_struct_pointer_element("led", i/3,
				    ((i % 3) == 0) ? "red" : ((i % 3) == 1) ? "green" : "blue",
				    ((i + 1) == num_leds) ? "};\n\n" : ",\n");
	}
      } else {
	emit_struct_pointer_end("NULL");
      }
    }
};

#endif
