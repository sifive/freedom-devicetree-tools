/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_test0.h>

#include <regex>

sifive_test0::sifive_test0(std::ostream &os, const fdt &dtb)
  : Device(os, dtb, "sifive,test0")
{}

void sifive_test0::include_headers()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_include(compat_string);
    });
}

void sifive_test0::declare_inlines()
{
  Inline* func;
  std::list<Inline *> extern_inlines;
  int count = 0;
  
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      n.named_tuples(
	"reg-names", "reg",
	"control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
	  if (count == 0) {
	    func = create_inline_dec("base",
				   "unsigned long",
				   " ");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("size",
				   "unsigned long",
				   " ");
	    extern_inlines.push_back(func);
	  }
	});
	count++;
      }
  );
  os << "\n";
  os << "/* --------------------- sifive_test0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_test0");
    delete func;
  }
  os << "\n";
}

void sifive_test0::define_inlines()
{
  Inline* func;
  std::list<Inline *> extern_inlines;

  int count = 0;
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      n.named_tuples(
	"reg-names", "reg",
	"control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
	  if (count == 0) {
	    func = create_inline_def("base",
				     "unsigned long",
				     "empty",
				     std::to_string(base),
				     " ");
	    extern_inlines.push_back(func);

	    func = create_inline_def("size",
				     "unsigned long",
				     "empty",
				     std::to_string(size),
				     " ");
	    extern_inlines.push_back(func);
	  }
	});

      count++;
    }
  );
  os << "\n";
  os << "/* --------------------- sifive_test0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "sifive_test0");
    delete func;
  }
  os << "\n";
}

void sifive_test0::declare_structs()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_struct_decl("sifive_test0", n);
    }
  );
}

void sifive_test0::define_structs()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_struct_begin("sifive_test0", n);

      emit_struct_field("shutdown.vtable", "&__metal_driver_vtable_sifive_test0.shutdown");

      emit_struct_end();
    });
}

void sifive_test0::create_handles()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_def_handle("__METAL_DT_SHUTDOWN_HANDLE", n, ".shutdown");
    });
}

