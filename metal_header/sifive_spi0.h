/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_SPI0__H
#define __METAL_HEADER_SIFIVE_SPI0__H

#include "metal_header/device.h"

#include <regex>

class sifive_spi0 : public Device {
  public:
    int num_spis;

    sifive_spi0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,spi0")
    {
      /* Count the number of SPIs */
      num_spis = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  num_spis += 1;
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
	  emit_struct_decl("sifive_spi0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_spi0", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_spi0");
	  emit_struct_field("spi.vtable", "&__metal_driver_vtable_sifive_spi0.spi");

	  emit_struct_field_platform_define("control_base", n, "BASE_ADDRESS");
	  emit_struct_field_platform_define("control_size", n, "SIZE");

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

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      emit_def("__METAL_DT_MAX_SPIS", std::to_string(num_spis));

      emit_struct_pointer_begin("sifive_spi0", "__metal_spi_table", "[]");
      if (num_spis) {
	int i = 0;
	dtb.match(
	  std::regex(compat_string),
	  [&](node n) {
	    os << "\t\t\t\t\t&__metal_dt_" << n.handle();

	    if ((i + 1) == num_spis) {
	      os << "};\n\n";
	    } else {
	      os << ",\n";
	    }

	    i++;
	  });
      } else {
	emit_struct_pointer_end("NULL");
      }
    }
};

#endif
