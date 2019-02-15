/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_PLIC0__H
#define __METAL_HEADER_RISCV_PLIC0__H

#include "metal_header/device.h"

#include <regex>

class riscv_plic0 : public Device {
  public:
    riscv_plic0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "riscv,plic0")
    {}

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
 
      emit_def("METAL_MAX_PLIC_INTERRUPTS", std::to_string(max_interrupts));
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
	  emit_struct_decl("riscv_plic0_data", n, "_data");
	  emit_const_struct_decl("riscv_plic0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("riscv_plic0_data", n, "_data");
	  emit_struct_field("init_done", "0");
	  emit_struct_end();

	  emit_const_struct_begin("riscv_plic0", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_riscv_plic0");
	  emit_struct_field("controller.vtable", "&__metal_driver_vtable_riscv_plic0.plic_vtable");

	  emit_struct_field("data", "&__metal_dt_" + n.handle() + "_data");

	  n.maybe_tuple(
	    "interrupts-extended", tuple_t<node, uint32_t>(),
	    [&](){
		emit_struct_field_null("interrupt_parent");
		emit_struct_field("interrupt_line", "");
	    },
	    [&](node n, uint32_t line) {
		emit_struct_field_node("interrupt_parent", n, ".controller");
		emit_struct_field_u32("interrupt_line", line);
	    });

	  n.named_tuples(
	    "reg-names", "reg",
	    "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
	      emit_struct_field_ta("control_base", base);
	      emit_struct_field_ts("control_size", size);
	    });

	  emit_struct_field_u32("max_priority", n.get_field<uint32_t>("riscv,max-priority"));
	  emit_struct_field_u32("num_interrupts", n.get_field<uint32_t>("riscv,ndev"));

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
