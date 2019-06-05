/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_FE310_G000_HFROSC__H
#define __METAL_HEADER_SIFIVE_FE310_G000_HFROSC__H

#include "metal_header/device.h"

#include <regex>

class sifive_fe310_g000_hfrosc : public Device {
  public:
    sifive_fe310_g000_hfrosc(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fe310-g000,hfrosc")
    {}

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
	    func = create_inline_dec("ref",
				     "struct metal_clock *",
				     "const struct metal_clock *clock");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("config_base",
				     "struct __metal_driver_sifive_fe310_g000_prci *",
				     "const struct metal_clock *clock");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("config_vtable",
				     "const struct __metal_driver_vtable_sifive_fe310_g000_prci *",
				     "struct metal_clock *clock");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("config_offset",
				     "long",
				     "const struct metal_clock *clock");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_fe310_g000_hfrosc ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "sifive_fe310_g000_hfrosc");
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
	    "clocks", tuple_t<node>(),
	    [&](){
	      func = create_inline_def("ref",
				       "struct metal_clock *",
				       "empty",
				       "NULL",
				       "const struct metal_cloc *clock");
	      extern_inlines.push_back(func);
	    },
	    [&](node m) {
	      std::string value = "(struct metal_clock *)&__metal_dt_"
		                  + m.handle() + ".clock";
	      func = create_inline_def("ref",
				       "struct metal_clock *",
				       "empty",
				       value,
				       "const struct metal_clock *clock");
	      extern_inlines.push_back(func);
	    });

	  n.named_tuples(
	    "reg-names", "reg",
	    "config", tuple_t<node, target_size>(),
	    [&](node m, target_size offset) {
	      func = create_inline_def("config_base",
				       "struct __metal_driver_sifive_fe310_g000_prci *",
				       "empty",
			               "(struct __metal_driver_sifive_fe310_g000_prci *)&__metal_dt_"
					+ m.handle(),
				       "const struct metal_clock *clock");
	      extern_inlines.push_back(func);

              func = create_inline_def("config_vtable",
                                       "const struct __metal_driver_vtable_sifive_fe310_g000_prci *",
                                       "empty",
				       "&__metal_driver_vtable_sifive_fe310_g000_prci",
                                       "struct metal_clock *clock");
              extern_inlines.push_back(func);

	      func = create_inline_def("config_offset",
				       "long",
				       "empty",
                                       platform_define_offset(m, METAL_HFROSCCFG_LABEL),
				       "const struct metal_clock *clock");
	      extern_inlines.push_back(func);
	    });
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_fe310_g000_hfrosc ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "sifive_fe310_g000_hfrosc");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("sifive_fe310_g000_hfrosc", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_fe310_g000_hfrosc", n);

	  emit_struct_field("clock.vtable", "&__metal_driver_vtable_sifive_fe310_g000_hfrosc.clock");

	  emit_struct_end();
	});
    }
};

#endif
