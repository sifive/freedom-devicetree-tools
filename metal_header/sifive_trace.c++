/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_trace.h>

#include <regex>

sifive_trace::sifive_trace(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,trace") {}

void sifive_trace::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_trace::declare_inlines() {
  Inline *func;

  os << "\n/* --------------------- sifive_trace ------------ */\n";
  if (dtb.match(std::regex(compat_string), [](node n) {}) != 0) {
    func = create_inline_dec("base", "unsigned long",
                             "const struct metal_uart *uart");
    emit_inline_dec(func, "sifive_trace");
    delete func;

    func = create_inline_dec("size", "unsigned long",
                             "const struct metal_uart *uart");
    emit_inline_dec(func, "sifive_trace");
    delete func;
  }
}

void sifive_trace::define_inlines() {
  Inline *base_func;
  Inline *size_func;

  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      base_func = create_inline_def(
          "base", "unsigned long",
          "(uintptr_t)uart == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_BASE_ADDRESS_LABEL),
          "const struct metal_uart *uart");
      size_func = create_inline_def(
          "size", "unsigned long",
          "(uintptr_t)uart == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_SIZE_LABEL),
          "const struct metal_uart *uart");
    } else {
      add_inline_body(base_func,
                      "(uintptr_t)uart == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_BASE_ADDRESS_LABEL));
      add_inline_body(size_func,
                      "(uintptr_t)uart == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_SIZE_LABEL));
    }

    count += 1;
  });

  os << "\n/* --------------------- sifive_trace ------------ */\n";
  if (count != 0) {
    add_inline_body(base_func, "else", "0");
    emit_inline_def(base_func, "sifive_trace");
    delete base_func;

    add_inline_body(size_func, "else", "0");
    emit_inline_def(size_func, "sifive_trace");
    delete size_func;
  }
}

void sifive_trace::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_trace", n); });
}

void sifive_trace::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_trace", n);

    emit_struct_field("uart.vtable",
                      "&__metal_driver_vtable_sifive_trace.uart");

    emit_struct_end();
  });
}
