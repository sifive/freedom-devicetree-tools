/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_FIXED_CLOCK__H
#define __METAL_HEADER_FIXED_CLOCK__H

#include "metal_header/device.h"

#include <regex>

class fixed_clock : public Device {
  public:
    int num_clocks;

    fixed_clock(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "fixed-clock")
    {
      /* Count the number of Fixed-Clocks */
      num_clocks = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  num_clocks += 1;
	});
    }

    void include_headers()
    {
      emit_include(compat_string);
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
	    func = create_inline_dec("rate",
				     "unsigned long",
				     "const struct metal_clock *clock");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- fixed_clock ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "fixed_clock");
	delete func;
      }
      os << "\n";
    }

    void define_inlines()
    {
      Inline* rate_func;
      std::list<Inline *> extern_inlines;

      int count = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  if (count == 0) {
	    rate_func = create_inline_def("rate",
				          "unsigned long",
				          "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
				          platform_define(n, METAL_CLOCK_FREQUENCY_LABEL),
				          "const struct metal_clock *clock");
	  }
	  if (count > 0) {
	    add_inline_body(rate_func,
			    "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
			    platform_define(n, METAL_CLOCK_FREQUENCY_LABEL));
	  }
	  if ((count + 1) == num_clocks) {
	    add_inline_body(rate_func, "else", "0");
	  }
	  count++;
	}
      );

      if (count > 0) {
	extern_inlines.push_back(rate_func);
      }

      os << "\n";
      os << "/* --------------------- fixed_clock ------------ */\n";
      while (!extern_inlines.empty()) {
	Inline *func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "fixed_clock");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("fixed_clock", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("fixed_clock", n);
	  emit_struct_field("clock.vtable", "&__metal_driver_vtable_fixed_clock.clock");
	  emit_struct_end();
	});
    }
};

#endif
