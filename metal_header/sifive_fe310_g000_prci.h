/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_FE310_G000_PRCI__H
#define __METAL_HEADER_SIFIVE_FE310_G000_PRCI__H

#include "metal_header/device.h"

#include <regex>

class sifive_fe310_g000_prci : public Device {
  public:
    sifive_fe310_g000_prci(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fe310-g000,prci")
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
	    func = create_inline_dec("base",
				     "long",
				     " ");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("size",
				     "long",
				     " ");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("vtable",
				     "const struct __metal_driver_vtable_sifive_fe310_g000_prci *",
				     " ");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- fe310_g000_prci ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "sifive_fe310_g000_prci");
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
	  if (count == 0) {
	    func = create_inline_def("base",
				     "long",
				     "empty",
                                     platform_define(n, METAL_BASE_ADDRESS_LABEL),
				     " ");
	    extern_inlines.push_back(func);

	    func = create_inline_def("size",
				     "long",
				     "empty",
                                     platform_define(n, METAL_SIZE_LABEL),
				     " ");
	    extern_inlines.push_back(func);

	    func = create_inline_def("vtable",
				     "const struct __metal_driver_vtable_sifive_fe310_g000_prci *",
				     "empty",
				     "&__metal_driver_vtable_sifive_fe310_g000_prci",
				     " ");
	    extern_inlines.push_back(func);
	  }
	  count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_fe310_g000_prci ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "sifive_fe310_g000_prci");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("sifive_fe310_g000_prci", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_fe310_g000_prci", n);

	  emit_struct_end();
	});
    }
};

#endif
