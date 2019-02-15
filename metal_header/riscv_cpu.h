/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_CPU__H
#define __METAL_HEADER_RISCV_CPU__H

#include "metal_header/device.h"

#include <regex>

class riscv_cpu : public Device {
  public:

    int num_cpus;

    riscv_cpu(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "cpu")
    {
	/* Count the number of CPUs */
	num_cpus = 0;
	dtb.match(
	  std::regex(compat_string),
	  [&](node n) {
	    num_cpus += 1;
	  });
    }

    void include_headers()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_include("riscv,cpu");
	});
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_const_struct_decl("cpu", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_const_struct_begin("cpu", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_cpu");
	  emit_struct_field("cpu.vtable", "&__metal_driver_vtable_cpu.cpu_vtable");

	  emit_struct_field_u32("timebase", n.get_field<uint32_t>("timebase-frequency"));

	  emit_struct_field("interrupt_controller", "&__metal_dt_interrupt_controller.controller");

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_def_handle("__METAL_DT_RISCV_CPU_HANDLE", n, ".cpu");
	});

      emit_def("__METAL_DT_MAX_HARTS", std::to_string(num_cpus));

      emit_const_struct_pointer_begin("cpu", "__metal_cpu_table", "[]");
      for(int i = 0; i < num_cpus; i++) {
	emit_struct_pointer_element("cpu", i, "",
				    ((i + 1) == num_cpus) ? "};\n\n" : ",\n");
      }
    }
};

#endif
