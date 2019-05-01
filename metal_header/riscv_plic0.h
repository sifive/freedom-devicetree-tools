/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_PLIC0__H
#define __METAL_HEADER_RISCV_PLIC0__H

#include "metal_header/device.h"

#include <regex>

class riscv_plic0 : public Device {
  public:
    int num_parents;

    riscv_plic0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "riscv,plic0")
    {
      num_parents = 0;

      dtb.match(
        std::regex("riscv,plic0"),
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
        std::regex("riscv,plic0"),
        [&](node n) {
	  /* Add 1 to number of interrupts for 0 base software index */
          emit_def("__METAL_PLIC_SUBINTERRUPTS", std::to_string(n.get_field<uint32_t>("riscv,ndev") + 1));

          emit_def("__METAL_PLIC_NUM_PARENTS", std::to_string(num_parents));
        });

      /* If no PLIC exists, output 0 as a default value */
      os << "#ifndef __METAL_PLIC_SUBINTERRUPTS\n";
      os << "#define __METAL_PLIC_SUBINTERRUPTS 0\n";
      os << "#endif\n";
      os << "#ifndef __METAL_PLIC_NUM_PARENTS\n";
      os << "#define __METAL_PLIC_NUM_PARENTS 0\n";
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

          /* Add 1 to number of interrupts for 0 base software index */
          sub_interrupts = n.get_field<uint32_t>("riscv,ndev") + 1;
          emit_def("__METAL_PLIC_SUBINTERRUPTS", std::to_string(sub_interrupts));

	  if(num_interrupts > max_interrupts) {
	    max_interrupts = num_interrupts;
	  }
	});
      if (sub_interrupts == 0) { 
        os << "#define __METAL_PLIC_SUBINTERRUPTS 0\n";
      }
      emit_def("METAL_MAX_PLIC_INTERRUPTS", std::to_string(max_interrupts));

      emit_def("__METAL_PLIC_NUM_PARENTS", std::to_string(num_parents));
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
	  emit_struct_decl("riscv_plic0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("riscv_plic0", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_riscv_plic0");
	  emit_struct_field("controller.vtable", "&__metal_driver_vtable_riscv_plic0.plic_vtable");

	  emit_struct_field("init_done", "0");

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

	  emit_struct_field_platform_define("control_base", n, METAL_BASE_ADDRESS_LABEL);
	  emit_struct_field_platform_define("control_size", n, METAL_SIZE_LABEL);

	  emit_struct_field_platform_define("max_priority", n, METAL_RISCV_MAX_PRIORITY_LABEL);
	  emit_struct_field_platform_define("num_interrupts", n, METAL_RISCV_NDEV_LABEL);

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
	  emit_def_handle("__METAL_DT_RISCV_PLIC0_HANDLE", n, ".controller");
	});
    }
};

#endif
