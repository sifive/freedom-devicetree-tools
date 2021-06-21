/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_prci0.h>
#include <stdio.h>

sifive_prci0::sifive_prci0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,(fu540|fu740)-c000-prci0") {}

void sifive_prci0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include("sifive,prci0"); });
}

void sifive_prci0::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;

  dtb.match(std::regex(compat_string), [&](node n) {
    func = create_inline_dec("control_base", "unsigned long",
                             "struct metal_prci *prci");
    extern_inlines.push_back(func);

    func = create_inline_dec("control_size", "unsigned long",
                             "struct metal_prci *prci");
    extern_inlines.push_back(func);
  });

  os << "\n";
  os << "/* --------------------- sifive_prci0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_prci0");
    delete func;
  }
  os << "\n";
}

void sifive_prci0::define_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;

  dtb.match(std::regex(compat_string), [&](node n) {
    func = create_inline_def("control_base", "unsigned long",
                             "(uintptr_t)prci == (uintptr_t)&__metal_dt_" +
                                 n.handle(),
                             platform_define(n, METAL_BASE_ADDRESS_LABEL),
                             "struct metal_prci *prci");
    add_inline_body(func, "else", "0");
    extern_inlines.push_back(func);

    func = create_inline_def(
        "control_size", "unsigned long",
        "(uintptr_t)prci == (uintptr_t)&__metal_dt_" + n.handle(),
        platform_define(n, METAL_SIZE_LABEL), "struct metal_prci *prci");
    add_inline_body(func, "else", "0");
    extern_inlines.push_back(func);
  });

  os << "\n";
  os << "/* --------------------- sifive_prci0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "sifive_prci0");
    delete func;
  }
  os << "\n";
}

void sifive_prci0::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_prci0", n); });
}

void sifive_prci0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_prci0", n);

    emit_struct_field("prci.vtable",
                      "&__metal_driver_vtable_sifive_prci0.prci");
    emit_struct_field("init_done", "0");

    emit_struct_end();
  });
}

void sifive_prci0::create_handles() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_def_handle("__METAL_DT_SIFIVE_PRCI0_HANDLE", n, "");
  });
}
