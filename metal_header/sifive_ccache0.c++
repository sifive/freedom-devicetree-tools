/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_ccache0.h>

#include <regex>

sifive_ccache0::sifive_ccache0(std::ostream &os, const fdt &dtb)
  : Device(os, dtb, "sifive,ccache0")
{}

void sifive_ccache0::include_headers()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_include(compat_string);
    });
}

void sifive_ccache0::declare_inlines()
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
  os << "/* --------------------- sifive_ccache0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_ccache0");
    delete func;
  }
  os << "\n";
}

void sifive_ccache0::define_inlines()
{
  Inline* func;
  std::list<Inline *> extern_inlines;

  int count = 0;
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      if (count == 0) {
	func = create_inline_def("control_base",
				 "uintptr_t",
				 "(uintptr_t)cache == (uintptr_t)&__metal_dt_" + n.handle(),
				 platform_define(n, METAL_BASE_ADDRESS_LABEL),
				 "struct metal_cache *cache");
	add_inline_body(func, "else", "0");
	extern_inlines.push_back(func);
      }
    count++;
    }
  );
  os << "\n";
  os << "/* --------------------- sifive_ccache0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "sifive_ccache0");
    delete func;
  }
  os << "\n";
}

void sifive_ccache0::declare_structs()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_struct_decl("sifive_ccache0", n);
    }
  );
}

void sifive_ccache0::define_structs()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_struct_begin("sifive_ccache0", n);

      emit_struct_field("cache.vtable", "&__metal_driver_vtable_sifive_ccache0.cache");

      emit_struct_end();
    });
}

void sifive_ccache0::create_handles()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_def_handle("__METAL_DT_SIFIVE_CCACHE0_HANDLE", n, ".cache");
    });
}

