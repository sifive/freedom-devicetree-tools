/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_CLINT0__H
#define __METAL_HEADER_RISCV_CLINT0__H

#include "metal_header/device.h"

#include <regex>

class riscv_clint0 : public Device {
  public:
    int num_parents;

    riscv_clint0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "riscv,clint0")
    {
      num_parents = 0;

      dtb.match(
        std::regex("riscv,clint0"),
        [&](node n) {
	  n.maybe_tuple_size(
	    "interrupts-extended", tuple_t<node, uint32_t>(),
	    [&](){},
	    [&](int s, node c, uint32_t line) {
	      num_parents += 1;
	    });
        });
    }

    void create_machine_macros()
    {
      dtb.match(
        std::regex("riscv,clint0"),
        [&](node n) {
          emit_def("__METAL_CLINT_NUM_PARENTS", std::to_string(num_parents));
        });

      os << "#ifndef __METAL_CLINT_NUM_PARENTS\n";
      os << "#define __METAL_CLINT_NUM_PARENTS 0\n";
      os << "#endif\n";
    }

    void create_defines()
    {
      uint32_t max_interrupts = 0;

      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  uint32_t num_interrupts = n.get_fields_count<std::tuple<node, uint32_t>>("interrupts-extended");

	  emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS", std::to_string(num_interrupts));

	  if(num_interrupts > max_interrupts) {
	    max_interrupts = num_interrupts;
	  }
	});
 
      emit_def("METAL_MAX_CLINT_INTERRUPTS", std::to_string(max_interrupts));
      emit_def("__METAL_CLINT_NUM_PARENTS", std::to_string(num_parents));
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
	  emit_struct_decl("riscv_clint0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("riscv_clint0", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_riscv_clint0");
	  emit_struct_field("controller.vtable", "&__metal_driver_vtable_riscv_clint0.clint_vtable");

	  emit_struct_field_platform_define("control_base", n, "BASE_ADDRESS");
	  emit_struct_field_platform_define("control_size", n, "SIZE");

	  emit_struct_field("init_done", "0");
	  emit_struct_field("num_interrupts", "METAL_MAX_CLINT_INTERRUPTS");

	  n.maybe_tuple_index(
	    "interrupts-extended", tuple_t<node, uint32_t>(),
	    [&](){
		emit_struct_field_null("interrupt_parents[0]");
		emit_struct_field("interrupt_lines[0]", "0");
	    },
	    [&](int i, node c, uint32_t line) {
		os << "    .interrupt_parents[" + std::to_string(i) + "] = &__metal_dt_" + c.parent().handle() + "_" + c.handle() + ".controller,\n";
		os << "    .interrupt_lines[" + std::to_string(i) + "] = " + std::to_string(line) + ",\n";
	    });

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_def_handle("__METAL_DT_RISCV_CLINT0_HANDLE", n, ".controller");
	});
    }
};

#endif
