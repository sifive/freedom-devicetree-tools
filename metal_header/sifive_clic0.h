/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_CLIC0__H
#define __METAL_HEADER_SIFIVE_CLIC0__H

#include "metal_header/device.h"

#include <regex>

class sifive_clic0 : public Device {
  public:
    sifive_clic0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,clic0")
    {}

    void create_machine_macros()
    {
      dtb.match(
	std::regex("sifive,clic0"),
	[&](node n) {
	  emit_def("__METAL_CLIC_SUBINTERRUPTS", std::to_string(n.get_field<uint32_t>("sifive,numints")));
	});

      /* If no CLIC exists, output 0 as a default value */
      os << "#ifndef __METAL_CLIC_SUBINTERRUPTS\n";
      os << "#define __METAL_CLIC_SUBINTERRUPTS 0\n";
      os << "#endif\n"; 
    }

    void create_defines()
    {
      uint32_t sub_interrupts = 0;
      uint32_t max_interrupts = 0;

      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  uint32_t num_interrupts = n.get_fields_count<std::tuple<node, uint32_t>>("interrupts-extended");
	  emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS", std::to_string(num_interrupts));

          sub_interrupts = n.get_field<uint32_t>("sifive,numints");
	  emit_def("__METAL_CLIC_SUBINTERRUPTS", std::to_string(sub_interrupts));

	  if(num_interrupts > max_interrupts) {
	    max_interrupts = num_interrupts;
	  }
	});
 
      if (sub_interrupts == 0) { 
        os << "#define __METAL_CLIC_SUBINTERRUPTS 0\n";
      }
      emit_def("METAL_MAX_CLIC_INTERRUPTS", std::to_string(max_interrupts));
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
	  emit_struct_decl("sifive_clic0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_clic0", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_clic0");
	  emit_struct_field("controller.vtable", "&__metal_driver_vtable_sifive_clic0.clic_vtable");

	  emit_struct_field_platform_define("control_base", n, "BASE_ADDRESS");
	  emit_struct_field_platform_define("control_size", n, "SIZE");

	  emit_struct_field("init_done", "0");
	  emit_struct_field("num_interrupts", "METAL_MAX_CLIC_INTERRUPTS");

	  n.maybe_tuple_size(
	    "interrupts-extended", tuple_t<node, uint32_t>(),
	    [&](){
		emit_struct_field_null("interrupt_parent");
		emit_struct_field("interrupt_lines[0]", "0");
	    },
	    [&](int s, node c, uint32_t line) {
		emit_struct_container_node_and_array(s, "interrupt_parent", c, ".controller",
						     "interrupt_lines", line);
	    });

	  emit_struct_field_platform_define("num_subinterrupts", n, "SIFIVE_NUMINTS");
	  emit_struct_field_platform_define("num_intbits", n, "SIFIVE_NUMINTBITS");
	  emit_struct_field_platform_define("max_levels", n, "SIFIVE_NUMLEVELS");

	  if (n.field_exists("interrupt-controller")) {
	      emit_struct_field("interrupt_controller", "1");
	  }

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_def_handle("__METAL_DT_SIFIVE_CLIC0_HANDLE", n, ".controller");
      });
    }
};

#endif
