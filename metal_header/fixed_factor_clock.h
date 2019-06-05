/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_FIXED_FACTOR_CLOCK__H
#define __METAL_HEADER_FIXED_FACTOR_CLOCK__H

#include "metal_header/device.h"

#include <regex>

class fixed_factor_clock : public Device {
  public:
    int num_clocks;

    fixed_factor_clock(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "fixed-factor-clock")
    {
      /* Count the number of Fixed-Factor-Clocks */
      num_clocks = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  num_clocks += 1;
	});
    }

    void include_headers()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_include(compat_string);
	}
      );
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
	    func = create_inline_dec("parent",
				     "struct metal_clock *",
				     "struct metal_clock *clock");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("mult",
				     "unsigned long",
				     "struct metal_clock *clock");
	    extern_inlines.push_back(func);
	    func = create_inline_dec("div",
				     "unsigned long",
				     "struct metal_clock *clock");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- fixed_factor_clock ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "fixed_factor_clock");
	delete func;
      }
      os << "\n";
    }

    void define_inlines()
    {
      Inline* func;
      Inline *func1, *func2;
      std::list<Inline *> extern_inlines;

      int count = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  n.maybe_tuple_index(
	    "clocks", tuple_t<node>(),
	    [&](){
	      if (count == 0) {
		func = create_inline_def("parent",
					 "struct metal_clock *",
					 "empty",
					 "NULL",
					 "struct metal_clock *clock");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](int i, node m) {
	      if (count == 0) {
		std::string value = "(struct metal_clock *)&__metal_dt_"
		                  + m.handle() + ".clock";
		func = create_inline_def("parent",
					 "struct metal_clock *",
					 "empty",
					 value,
					 "struct metal_clock *clock");
		extern_inlines.push_back(func);
	      }
	    });

	  if (count == 0) {
	    func1 = create_inline_def("mult",
		       		     "unsigned long",
				    "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
                                     platform_define(n, METAL_CLOCK_MULT_LABEL),
				     "struct metal_clock *clock");
	    extern_inlines.push_back(func1);
	    func2 = create_inline_def("div",
			       	     "unsigned long",
				    "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
                                     platform_define(n, METAL_CLOCK_DIV_LABEL),
				     "struct metal_clock *clock");
	    extern_inlines.push_back(func2);
	  }
	  if (count > 0) {
	    add_inline_body(func1,
			    "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
			    platform_define(n, METAL_CLOCK_MULT_LABEL));
	    add_inline_body(func2,
			    "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
			    platform_define(n, METAL_CLOCK_DIV_LABEL));
	  }
	  if ((count + 1) == num_clocks) {
	    add_inline_body(func1, "else", "0");
	    add_inline_body(func2, "else", "0");
	  }
	  count++;
	}
      );
      os << "\n";
      os << "/* --------------------- fixed_factor_clock ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "fixed_factor_clock");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("fixed_factor_clock", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("fixed_factor_clock", n);
	  emit_struct_field("clock.vtable", "&__metal_driver_vtable_fixed_factor_clock.clock");
	  emit_struct_end();
	});
    }
};

#endif
