/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_TEMPLATE_DEVICE__H
#define __METAL_HEADER_TEMPLATE_DEVICE__H

#include "metal_header/device.h"

#include <regex>

class sifive_fu540_c000_l2 : public Device {
  public:
    sifive_fu540_c000_l2(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fu540-c000,l2")
    {}

    void include_headers()
    {
      dtb.match(
	std::regex("sifive,ccache0"),
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
	    func = create_inline_dec("control_base",
				     "uintptr_t",
				     "struct metal_cache *cache");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_fu540_c000_l2 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "sifive_fu540_c000_l2");
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
	    n.named_tuples(
	      "reg-names", "reg",
	      "config", tuple_t<node>(), [&](node m) {
	      func = create_inline_def("control_base",
				       "uintptr_t",
				       "(uintptr_t)cache == (uintptr_t)&__metal_dt_" + n.handle(),
				       "&__metal_dt_" + m.handle(),
				       "struct metal_cache *cache");
	      add_inline_body(func, "else", "NULL");
	      extern_inlines.push_back(func);
	      });
	  }
	count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_fu540_c000_l2 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "sifive_fu540_c000_l2");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex("sifive,ccache0"),
	[&](node n) {
	  emit_struct_decl("sifive_fu540_c000_l2", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex("sifive,ccache0"),
	[&](node n) {
	  emit_struct_begin("sifive_fu540_c000_l2", n);

	  emit_struct_field("cache.vtable", "&__metal_driver_vtable_sifive_fu540_c000_l2.cache");

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex("sifive,ccache0"),
	[&](node n) {
	  emit_def_handle("__METAL_DT_SIFIVE_FU540_C000_L2_HANDLE", n, "");
	});
    }
};

#endif
