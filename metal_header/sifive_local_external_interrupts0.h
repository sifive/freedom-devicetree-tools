/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_LOCAL_EXTERNAL_INTERRUPTS0__H
#define __METAL_HEADER_SIFIVE_LOCAL_EXTERNAL_INTERRUPTS0__H

#include "metal_header/device.h"

#include <regex>

class sifive_local_external_interrupts0 : public Device {
  public:
    uint32_t max_interrupts = 0;

    sifive_local_external_interrupts0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,local-external-interrupts0")
    {}

    void create_defines()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  uint32_t num_interrupts = n.get_fields_count<uint32_t>("interrupts");

	  emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS", std::to_string(num_interrupts));

	  if(num_interrupts > max_interrupts) {
	    max_interrupts = num_interrupts;
	  }
	});
 
      emit_def("METAL_MAX_LOCAL_EXT_INTERRUPTS", std::to_string(max_interrupts));
    }

    void include_headers()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_include(compat_string);
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
	    func = create_inline_dec("interrupt_parent",
				     "struct metal_interrupt *",
				     "struct metal_interrupt *controller");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("num_interrupts",
				     "int",
				     "struct metal_interrupt *controller");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("interrupt_lines",
				     "int",
				     "struct metal_interrupt *controller", "int idx");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_local_external_interrupts0 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "sifive_local_external_interrupts0");
	delete func;
      }
      os << "\n";
    }

    void define_inlines()
    {
      Inline* func;
      std::list<Inline *> extern_inlines;

      int count = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  n.maybe_tuple(
	    "interrupt-parent", tuple_t<node>(),
	    [&](){
	      if (count == 0) {
		func = create_inline_def("interrupt_parent",
					 "struct metal_interrupt *",
					 "empty",
					 "NULL",
					 "struct metal_interrupt *controller");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](node m) {
	      if (count == 0) {
		std::string value = "(struct metal_interrupt *)&__metal_dt_";
		if(m.handle().compare("interrupt_controller") == 0) {
		  value += m.parent().handle() + "_" + m.handle() + ".controller";		  
		} else {
		  value += m.handle() + ".controller";
		}
		func = create_inline_def("interrupt_parent",
					 "struct metal_interrupt *",
					 "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
					 value,
					 "struct metal_interrupt *controller");
		add_inline_body(func, "else", "NULL");
		extern_inlines.push_back(func);
	      }
	    });

	  if (max_interrupts == 0) {
	    func = create_inline_def("num_interrupts",
				     "int",
				     "empty",
				     "0",
				     "struct metal_interrupt *controller");
	    extern_inlines.push_back(func);
	  } else {
	    if (count == 0) {
	      func = create_inline_def("num_interrupts",
				       "int",
				       "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
				       "METAL_MAX_LOCAL_EXT_INTERRUPTS",
				       "struct metal_interrupt *controller");
	      add_inline_body(func, "else", "0");
	      extern_inlines.push_back(func);
	    }
	  }

	  n.maybe_tuple_index(
	    "interrupts", tuple_t<uint32_t>(),
	    [&](){
	      if (count == 0) {
		func = create_inline_def("interrupt_lines",
					 "int",
					 "empty",
					 "0",
					 "struct metal_interrupt *controller", "int idx");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](int i, uint32_t irline){
	      if (i == 0) {
		func = create_inline_def("interrupt_lines",
					 "int",
					 "idx == " + std::to_string(i),
					 std::to_string(irline),
					 "struct metal_interrupt *controller", "int idx");
		extern_inlines.push_back(func);
	      } else if ((i + 1) == max_interrupts) {
		add_inline_body(func, "idx == " + std::to_string(i), std::to_string(irline));
		add_inline_body(func, "else", "0");
	      } else {
		add_inline_body(func, "idx == " + std::to_string(i), std::to_string(irline));
	      }
	    });

	  count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_local_external_interrupts0 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "sifive_local_external_interrupts0");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("sifive_local_external_interrupts0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_local_external_interrupts0", n);

	  emit_struct_field("irc.vtable", "&__metal_driver_vtable_sifive_local_external_interrupts0.local0_vtable");

	  emit_struct_field("init_done", "0");

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
  	  emit_def_handle("__METAL_DT_SIFIVE_LOCAL_EXINTR0_HANDLE", n, ".irc");
	});
    }
};

#endif
