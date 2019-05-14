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

    void declare_inlines()
    {
      Inline* func;
      std::list<Inline *> extern_inlines;
      int count = 0;
      
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  if (count == 0) {
	    func = create_inline_dec("timebase",
				     "int",
				     "struct metal_cpu *cpu");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("interrupt_controller",
				     "struct metal_interrupt *",
				     "struct metal_cpu *cpu");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- cpu ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "cpu");
	delete func;
      }
      os << "\n";
    }

    void define_inlines()
    {
      Inline* func;
      Inline* funci;
      std::list<Inline *> extern_inlines;

      int count = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  if (count == 0) {
	    int tf;
	    n.maybe_tuple(
	      "timebase-frequency", tuple_t<uint32_t>(),
	      [&]() {
		tf = n.parent().get_field<uint32_t>("timebase-frequency");
	      },
	      [&](uint32_t timebase) {
		tf = timebase;
	      });
	    func = create_inline_def("timebase",
				     "int",
				     "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
				     std::to_string(tf),
				     "struct metal_cpu *cpu");
	    extern_inlines.push_back(func);

	    funci = create_inline_def("interrupt_controller",
				     "struct metal_interrupt *",
				     "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
				     "&__metal_dt_" + n.handle() + "_interrupt_controller.controller",
				     "struct metal_cpu *cpu");
	    extern_inlines.push_back(funci);
	  }
	  if ((count + 1) == num_cpus) {
	    add_inline_body(func, "else", "0");
	    add_inline_body(funci, "else", "NULL");
	  } else {
	    int tf;
	    n.maybe_tuple(
	      "timebase-frequency", tuple_t<uint32_t>(),
	      [&]() {
		tf = n.parent().get_field<uint32_t>("timebase-frequency");
	      },
	      [&](uint32_t timebase) {
		tf = timebase;
	      });
	    add_inline_body(func,
			    "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
			    std::to_string(tf));
	    add_inline_body(funci, "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
			    "&__metal_dt_" + n.handle()
			     + "_interrupt_controller.controller");
	  }
	  count++;
	}
      );
      os << "\n";
      os << "/* --------------------- cpu ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "cpu");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("cpu", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("cpu", n);

	  emit_struct_field("cpu.vtable", "&__metal_driver_vtable_cpu.cpu_vtable");

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      emit_def("__METAL_DT_MAX_HARTS", std::to_string(num_cpus));

      emit_struct_pointer_begin("cpu", "__metal_cpu_table", "[]");
      for(int i = 0; i < num_cpus; i++) {
	emit_struct_pointer_element("cpu", i, "",
				    ((i + 1) == num_cpus) ? "};\n\n" : ",\n");
      }
    }
};

#endif
