/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_CPU_INTC__H
#define __METAL_HEADER_RISCV_CPU_INTC__H

#include "metal_header/device.h"

#include <regex>

using std::string;

class riscv_cpu_intc : public Device {
  public:
    riscv_cpu_intc(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "riscv,cpu-intc")
    {}

    string handle(node n)
    {
      return n.parent().handle() + "_" + n.handle();
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  os << "asm (\".weak __metal_dt_" << handle(n) << "\");\n";
	  os << "struct __metal_driver_riscv_cpu_intc __metal_dt_" << handle(n) << ";\n\n";
	});
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_comment(n);
	  os << "struct __metal_driver_riscv_cpu_intc __metal_dt_" << handle(n) << " = {\n";

	  emit_struct_field("vtable", "&__metal_driver_vtable_riscv_cpu_intc");
	  emit_struct_field("controller.vtable", "&__metal_driver_vtable_riscv_cpu_intc.controller_vtable");

	  emit_struct_field("init_done", "0");

	  if (n.field_exists("interrupt-controller")) {
	    emit_struct_field("interrupt_controller", "1");
	  }

	  emit_struct_end();
	});
    }
};

#endif
